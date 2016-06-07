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
#include "databases/database-sqlite3.h"

namespace spacel {
namespace engine {

#define SERVER_LOOP_TIME 0.025f

const bool Server::InitServer()
{
	try {
		m_db = new DatabaseSQLite3(m_datapath);
	}
	catch (SQLiteException &e) {
		URHO3D_LOGERROR(e.what());
		return false;
	}

	return true;
}

void Server::StopServer()
{
	delete m_db;
}

void* Server::run()
{
	if (!InitServer()) {
		URHO3D_LOGERROR("Failed to init server, aborting!");
		return nullptr;
	}

	float dtime = 0.0f;
	while (!StopRequested()) {
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

	return nullptr;
}

void Server::Step(const float dtime)
{

}
}
}
