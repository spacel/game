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

#include "databases/database-sqlite3.h"
#include "namegenerator.h"
#include "objectmanager.h"

namespace spacel {
namespace engine {

#define SERVER_LOOP_TIME 0.025f

Server::Server(const std::string &datapath, const std::string &universe_name):
		Thread(),
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
	ItemDefPtr test_def = std::make_shared<ItemDef>();
	test_def->type = ITEMTYPE_USELESS;
	test_def->name = "engine:stone";
	test_def->description = "A raw stone";
	ObjectMgr::instance()->RegisterItem(test_def);
	return true;
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
