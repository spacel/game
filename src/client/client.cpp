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

#include "client.h"
#include "network/clientpackethandler.h"
#include "project_defines.h"
#include "player.h"
#include <Urho3D/IO/Log.h>
#include <common/engine/server.h>
#include <thread>
#include <cassert>

namespace spacel {

using namespace engine::network;

#define CLIENT_LOOP_TIME 0.025f

Client::Client()
{
	m_loading_step = CLIENTLOADINGSTEP_NOT_STARTED;
}

Client::~Client()
{
	if (m_server) {
		m_server->Stop();
		delete m_server;
	}
}

void Client::ThreadFunction()
{
	if (!InitClient()) {
		URHO3D_LOGERROR("Failed to init client, aborting!");
		m_loading_step = CLIENTLOADINGSTEP_FAILED;
		return;
	}

	float dtime = 0.0f;
	while (shouldRun_) {
		const auto prev_time = std::chrono::system_clock::now();
		Step(dtime);
		const auto step_time = std::chrono::system_clock::now().time_since_epoch() -
				prev_time.time_since_epoch();

		// runtime is a float sec time
		float runtime =
			std::chrono::duration_cast<std::chrono::milliseconds>(step_time).count() /
			1000.0f;

		// If step runtime < LOOP TIME, sleep for the diff time
		if (runtime < CLIENT_LOOP_TIME) {
			// Convert back to ms from s
			std::this_thread::sleep_for(std::chrono::milliseconds(
				(uint32_t) ((CLIENT_LOOP_TIME - runtime) * 1000.0f)));
			dtime = CLIENT_LOOP_TIME;
		}
		else {
			URHO3D_LOGWARNINGF("Server thread lagging. Runtime %f > %f",
							   runtime, CLIENT_LOOP_TIME);
			dtime = runtime;
		}
	}

	m_loading_step = CLIENTLOADINGSTEP_ENDED;
}

bool Client::InitClient()
{
	m_loading_step = CLIENTLOADINGSTEP_BEGIN_START;

	if (m_singleplayer_mode) {
		m_server = new engine::Server(m_gamedata_path, m_data_path, m_universe_name);
		m_server->Run();

		// Wait for server to be up
		while (m_server->GetLoadingStep() < engine::SERVERLOADINGSTEP_STARTED) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (m_server->GetLoadingStep() == engine::SERVERLOADINGSTEP_FAILED) {
			return false;
		}
	}

	SendInitPacket();

	m_loading_step = CLIENTLOADINGSTEP_GAMEDATAS_LOADED;

	m_loading_step = CLIENTLOADINGSTEP_STARTED;
	return true;
}

/**
 * main Client loop for routing and processing events incoming from UI or server
 * @param dtime
 */
void Client::Step(const float dtime)
{
	// Singleplayer mode read server queue instead of client receive queue
	if (m_singleplayer_mode) {
		assert(m_server);

		while (!m_server->IsSendingQueueEmpty()) {
			std::unique_ptr<NetworkPacket> pkt(m_server->PopSendingQueue());
			pkt->Seek(2);
			ProcessPacket(pkt.get());
		}
	}
	else {
		while (!m_packet_receive_queue.empty()) {
			std::unique_ptr<NetworkPacket> pkt(m_packet_receive_queue.pop_front());
			ProcessPacket(pkt.get());
		}
	}

	{
		static const uint8_t MAX_CLIENT_UI_EVENT_TO_PROCESS = 20;
		uint8_t i = 0;
		while (!m_clientui_event_queue.empty() && i < MAX_CLIENT_UI_EVENT_TO_PROCESS) {
			i++;
			ClientUIEventPtr event = m_clientui_event_queue.pop_front();
			assert(event->id < CLIENT_UI_EVENT_MAX);

			const ClientUIEventHandler &eventHandler = ClientUIEventHandlerTable[event->id];
			(this->*eventHandler.handler)(event);
		}
	}
}

/**
 * Packet processing verifications before routing
 * @param packet
 */
void Client::ProcessPacket(NetworkPacket *packet)
{
	// Ignore invalid opcode
	if (packet->GetOpcode() >= MSG_MAX) {
		return;
	}

	// @TODO verify current client state

	RoutePacket(packet);
}

inline void Client::RoutePacket(NetworkPacket *packet)
{
	const CMsgHandler &opHandle = cmsgHandlerTable[packet->GetOpcode()];
	(this->*opHandle.handler)(packet);
}

void Client::SendPacket(NetworkPacket *packet)
{
	if (m_singleplayer_mode) {
		m_server->ReceivePacket(packet);
	}

	// @TODO network else
}

void Client::handlePacket_Hello(NetworkPacket *packet)
{
	uint8_t major_version = packet->ReadUByte(),
		minor_version = packet->ReadUByte(), patch_version = packet->ReadUByte();
	uint16_t protocol_version = packet->ReadUShort();
	URHO3D_LOGINFOF("Server version %d.%d.%d (proto %d) respond us hello",
		major_version, minor_version, patch_version, protocol_version);

	m_loading_step = CLIENTLOADINGSTEP_CONNECTED;

	NetworkPacket *resp_packet = new NetworkPacket(CMSG_AUTH);
	resp_packet->WriteString("singleplayer");
	resp_packet->WriteString("singleplayer_default");
	SendPacket(resp_packet);
}

void Client::handlePacket_Auth(NetworkPacket *packet)
{

}

void Client::handlePacket_Chat(NetworkPacket *packet)
{

}

void Client::handlePacket_GalaxySystems(NetworkPacket *packet)
{
	// When receive this packet we should clear received solar systems
	for (auto &ss: m_solar_systems) {
		delete ss.second;
	}
	m_solar_systems.clear();

	uint32_t ss_number = packet->ReadUInt();
	for (uint32_t i = 0; i < ss_number; i++) {
		engine::SolarSystem *ss = new engine::SolarSystem();
		ss->id = packet->ReadUInt64();
		ss->type = (engine::SolarType) packet->ReadUByte();
		ss->radius = packet->ReadDouble();
		ss->pos_x = packet->ReadDouble();
		ss->pos_y = packet->ReadDouble();
		ss->pos_z = packet->ReadDouble();
		ss->name = std::string(packet->ReadString().CString());
		m_solar_systems[ss->id] = ss;
	}

	URHO3D_LOGINFOF("Received %d solar systems from server", ss_number);
}

void Client::handlePacket_CharacterList(NetworkPacket *packet)
{
	m_loading_step = CLIENTLOADINGSTEP_AUTHED;

	uint8_t character_number = packet->ReadUByte();

	UIEvent_CharacterList *event = new UIEvent_CharacterList();

	for (uint8_t i = 0; i < character_number; i++) {
		CharacterList_Player c_player;
		c_player.guid = packet->ReadUInt64();
		c_player.race = (engine::PlayerRace) packet->ReadUByte();
		c_player.sex = (engine::PlayerSex) packet->ReadUByte();
		c_player.name = packet->ReadString();

		event->player_list.push_back(c_player);
	}

	QueueUIEvent(event);
}

void Client::handlePacket_CharacterCreate(NetworkPacket *packet)
{

}

void Client::handlePacket_CharacterRemove(NetworkPacket *packet)
{

}

void Client::handlePacket_Kick(NetworkPacket *packet)
{

}

void Client::SendInitPacket()
{
	NetworkPacket *pkt = new NetworkPacket(CMSG_HELLO);
	pkt->WriteUByte(0);
	pkt->WriteUByte(0);
	pkt->WriteUByte(PROJECT_VERSION_PATCH);
	pkt->WriteUShort(PROTOCOL_VERSION);
	SendPacket(pkt);
}

void Client::handleClientUiEvent_ChararacterAdd(ClientUIEventPtr event)
{
	ClientUIEvent_CharacterAdd *r_event = dynamic_cast<ClientUIEvent_CharacterAdd *>(event.get());
	assert(r_event);

	NetworkPacket *pkt = new NetworkPacket(CMSG_CHARACTER_CREATE);
	pkt->WriteUByte(r_event->race);
	pkt->WriteUByte(r_event->sex);
	pkt->WriteString(r_event->name);
	SendPacket(pkt);
}

void Client::handleClientUiEvent_ChararacterRemove(ClientUIEventPtr event)
{
	ClientUIEvent_CharacterRemove *r_event = dynamic_cast<ClientUIEvent_CharacterRemove *>(event.get());
	assert(r_event);

	NetworkPacket *pkt = new NetworkPacket(CMSG_CHARACTER_REMOVE);
	pkt->WriteUInt64(r_event->guid);
}

}
