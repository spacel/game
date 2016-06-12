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
#include "namegenerator.h"
#include "objectmanager.h"

namespace spacel {
namespace engine {

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
	}
	catch (SQLiteException &e) {
		URHO3D_LOGERROR(e.what());
		m_loading_step = SERVERLOADINGSTEP_FAILED;
		return false;
	}

	m_loading_step = SERVERLOADINGSTEP_DB_INITED;

	for (uint8_t i = 0; i < 100; i++) {
		std::cout << generate_world_name() << std::endl;
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
				URHO3D_LOGERRORF("Invalid item found! '%s' is not valid", item.c_str());
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

}
}
}
