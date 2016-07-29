/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
 *
 * Spacel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spacel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spacel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "server.h"

#include <Urho3D/IO/Log.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <json/json.h>

#include "databases/database-sqlite3.h"
#include "generators.h"
#include "objectmanager.h"
#include "space.h"
#include "../../project_defines.h"
#include "player.h"

namespace spacel {
namespace engine {

using namespace network;

#define SERVER_LOOP_TIME 0.025f

Server::Server(const std::string &gamedatapath, const std::string &datapath,
		const std::string &universe_name):
		Thread(),
		m_gamedatapath(gamedatapath),
		m_datapath(datapath),
		m_universe_name(universe_name)
{
	m_loading_step = SERVERLOADINGSTEP_NOT_STARTED;
}

const bool Server::InitServer()
{
	m_loading_step = SERVERLOADINGSTEP_BEGIN_START;
	try {
		m_db = new DatabaseSQLite3(m_datapath + m_universe_name);

		m_loading_step = SERVERLOADINGSTEP_DB_INITED;

		// @TODO get the seed from database
		UnivGen->SetSeed(180);
		bool galaxy_generated = m_db->IsUniverseGenerated(m_universe_name);

		const auto start = std::chrono::system_clock::now();
		if (!galaxy_generated) {
			// Generate 1 galaxy with 1M solar systems
			Galaxy *galaxy = Universe::instance()->CreateGalaxy(1000 * 1000);
			// Save the galaxy and solar systems
			m_db->BeginTransaction();
			m_db->CreateGalaxy(galaxy);
			for (const auto &ss: galaxy->solar_systems) {
				m_db->CreateSolarSystem(ss.second);
			}
			m_db->SetUniverseGenerated(m_universe_name, true);
			m_db->CommitTransaction();
		}
		else {
			Galaxy *galaxy = m_db->LoadGalaxy(1);
			m_db->LoadSolarSystemsForGalaxy(galaxy);
			Universe::instance()->SetGalaxy(galaxy);
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "Loading time: " << elapsed_seconds.count() << "s" << std::endl;
	}
	catch (SQLiteException &e) {
		URHO3D_LOGERROR(e.what());
		m_loading_step = SERVERLOADINGSTEP_FAILED;
		return false;
	}

	if (!LoadGameDatas()) {
		m_loading_step = SERVERLOADINGSTEP_FAILED;
	}

	m_loading_step = SERVERLOADINGSTEP_GAMEDATAS_LOADED;
	// @TODO more ?

	m_loading_step = SERVERLOADINGSTEP_STARTED;
	return true;
}

const bool Server::LoadGameDatas()
{
	try {
		std::ifstream cfg_file(m_gamedatapath + "items.json", std::ifstream::binary);
		if (!cfg_file.good()) {
			URHO3D_LOGERROR("Unable to read items from game datas");
			return false;
		}
		Json::Value root;
		cfg_file >> root;
		cfg_file.close();

		if (!root.isMember("items") && !root["items"].isObject()) {
			URHO3D_LOGERROR("No valid items root key found for items game datas");
			return false;
		}

		Json::Value item_root = root["items"];
		for (const auto &item_name: item_root.getMemberNames()) {
			Json::Value item_v = item_root[item_name];

			if (!item_v.isObject() || !item_v.isMember("id") || !item_v["id"].isUInt()) {
				std::cout << "error !" << std::endl;
				URHO3D_LOGERRORF("Invalid item found! '%s' is not valid", item_name.c_str());
				continue;
			}

			ItemDefPtr def = std::make_shared<ItemDef>();
			def->name = item_name;
			if (item_v.isMember("description")) {
				if (!item_v["description"].isString()) {
					URHO3D_LOGWARNINGF("Invalid description for %d (%s)", def->id,
						def->type);
					continue;
				}
				def->description = item_v["description"].asString();
			}

			if (item_v.isMember("icon")) {
				if (!item_v["icon"].isString()) {
					URHO3D_LOGWARNINGF("Invalid icon for %d (%s)", def->id, def->type);
					continue;
				}
				def->icon = item_v["icon"].asString();
			}

			if (item_v.isMember("type")) {
				if (!item_v["type"].isString()) {
					URHO3D_LOGWARNINGF("Invalid type for %d (%s)", def->id,
						def->type);
					continue;
				}

				std::string item_v_type = item_v["type"].asString();
				if (item_v_type.compare("resource") == 0) {
					def->type = ITEMTYPE_RESOURCE;
				}
				else if (item_v_type.compare("tool") == 0) {
					def->type = ITEMTYPE_TOOL;
				}
				else if (item_v_type.compare("currency") == 0) {
					def->type = ITEMTYPE_CURRENCY;
				}
				else if (item_v_type.compare("useless") == 0) {
					def->type = ITEMTYPE_USELESS;
				}
				else {
					URHO3D_LOGWARNINGF("Invalid item type %s for item %d (%s)",
						item_v_type.c_str(), def->id, def->name.c_str());
					continue;
				}
			}

			if (item_v.isMember("stack_max")) {
				if (!item_v["stack_max"].isUInt()) {
					URHO3D_LOGWARNINGF("Invalid stack_max for %d (%s)", def->id,
					   def->type);
					continue;
				}

					def->stack_max = item_v["stack_max"].asUInt();
			}

			ObjectMgr::instance()->RegisterItem(def);
		}

		URHO3D_LOGINFOF("%d items registered", ObjectMgr::instance()->GetRegisteredItemsCount());
		return true;
	}
	catch (std::exception &e) {
		return false;
	}
}

void Server::StopServer()
{
	delete m_db;
	m_db = nullptr;
}

void Server::ThreadFunction()
{
	URHO3D_LOGINFOF("Starting server for universe %s", m_universe_name.c_str());
	if (!InitServer()) {
		URHO3D_LOGERROR("Failed to init server, aborting!");
		return;
	}

	float dtime = 0.0f;
	while (shouldRun_) {
		const auto prev_time = std::chrono::system_clock::now();
		Step(dtime);
		const auto step_time = std::chrono::system_clock::now().time_since_epoch() -
				prev_time.time_since_epoch();

		// runtime is a float sec time
		float runtime = std::chrono::duration_cast<std::chrono::milliseconds>(step_time).count() / 1000.0f;

		// If step runtime < LOOP TIME, sleep for the diff time
		if (runtime < SERVER_LOOP_TIME) {
			// Convert back to ms from s
			std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)((SERVER_LOOP_TIME - runtime) * 1000.0f)));
			dtime = SERVER_LOOP_TIME;
		}
		else {
			URHO3D_LOGWARNINGF("Server thread lagging. Runtime %f > %f",
				runtime, SERVER_LOOP_TIME);
			dtime = runtime;
		}
	}

	StopServer();
}

void Server::Step(const float dtime)
{
	// @TODO limit packet processing time
	while (!m_packet_receive_queue.empty()) {
		std::unique_ptr<NetworkPacket> pkt(m_packet_receive_queue.pop_front());
		pkt->Seek(2);
		ProcessPacket(pkt.get());
	}
}

void Server::ProcessPacket(network::NetworkPacket *packet)
{
	// Ignore invalid opcode
	if (packet->GetOpcode() >= network::MSG_MAX) {
		return;
	}

#if 0
	// @TODO verify if session already exists
	Session *sess = SessionMgr::instance()->GetSession(packet->session_id);
	if (!sess) {
		// LOG
		return;
	}
#endif
	// @TODO verify different packet states depending on session state
	RoutePacket(packet);
}

void Server::RoutePacket(network::NetworkPacket *packet)
{
	const SMsgHandler &opHandle = smsgHandlerTable[packet->GetOpcode()];
	(this->*opHandle.handler)(packet);
}

void Server::handlePacket_Hello(NetworkPacket *packet)
{
	uint8_t major_version = packet->ReadUByte(),
		minor_version = packet->ReadUByte(), patch_version = packet->ReadUByte();
	uint16_t protocol_version = packet->ReadUShort();
	URHO3D_LOGINFOF("Client version %d.%d.%d (proto %d) tell us hello",
		major_version, minor_version, patch_version, protocol_version);

	NetworkPacket *resp_packet = new NetworkPacket(SMSG_HELLO);
	resp_packet->WriteUByte(0);
	resp_packet->WriteUByte(0);
	resp_packet->WriteUByte(PROJECT_VERSION_PATCH);
	resp_packet->WriteUShort(PROTOCOL_VERSION);
	SendPacket(resp_packet);
}

void Server::handlePacket_Auth(NetworkPacket *packet)
{
	Urho3D::String login = packet->ReadString();
	Urho3D::String password = packet->ReadString();
	URHO3D_LOGDEBUGF("Login %s, password %s", login.CString(), password.CString());

// Prepare auth handling
#if 0

	if (!auth_success) {
		NetworkPacket *resp_packet = new NetworkPacket(SMSG_AUTH);
		uint8_t resp_code = 0;
		resp_packet->WriteUByte(resp_code);
		if (resp_code == 2) {
			resp_packet->WriteString("custom string");
		}
		SendPacket(resp_packet);
		return;
	}
#endif

	NetworkPacket *resp_packet = new NetworkPacket(SMSG_CHARACTER_LIST);
	static const uint8_t character_number = 1;
	resp_packet->WriteUByte(character_number);

	for (uint8_t i = 0; i < character_number; i++) {
		resp_packet->WriteUInt64(6); // GUID
		resp_packet->WriteUByte(PLAYER_RACE_HUMAN); // Race
		resp_packet->WriteUByte(PLAYER_SEX_MALE); // Sex
		resp_packet->WriteString("TestCharacter");
	}

	SendPacket(resp_packet);

	// @TODO Change this place in the future
	NetworkPacket *galaxy_packet = new NetworkPacket(SMSG_GALAXY_SYSTEMS);
	Galaxy *galaxy = Universe::instance()->GetGalaxy(1);
	assert(galaxy);
	galaxy_packet->WriteUInt(galaxy->solar_systems.size());
	for (const auto &ss: galaxy->solar_systems) {
		galaxy_packet->WriteUInt64(ss.first); // ID
		galaxy_packet->WriteUByte(ss.second->type);
		galaxy_packet->WriteDouble(ss.second->radius);
		galaxy_packet->WriteDouble(ss.second->pos_x);
		galaxy_packet->WriteDouble(ss.second->pos_y);
		galaxy_packet->WriteDouble(ss.second->pos_z);
		galaxy_packet->WriteString(Urho3D::String(ss.second->name.c_str()));
	}
	SendPacket(galaxy_packet);
}

void Server::handlePacket_Chat(NetworkPacket *packet)
{

}

void Server::handlePacket_CharacterCreate(NetworkPacket *packet)
{
	PlayerRace playerRace = (PlayerRace) packet->ReadUByte();
	PlayerSex playerSex = (PlayerSex) packet->ReadUByte();
	Urho3D::String playerName = packet->ReadString();

	// @TODO handle this packet
}

void Server::handlePacket_CharacterRemove(NetworkPacket *packet)
{
	uint64_t characterId = packet->ReadUInt64();
}

void Server::handlePacket_CharacterConnect(NetworkPacket *packet)
{

}
}
}
