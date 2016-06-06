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

namespace spacel {
namespace engine {

#define SERVER_LOOP_TIME 0.025f

void* Server::run()
{
	float dtime = 0.0f;
	auto prev_time = std::chrono::system_clock::now();

	while (!StopRequested()) {
		step(dtime);
		auto step_time = std::chrono::system_clock::now().time_since_epoch() -
				prev_time.time_since_epoch();
		URHO3D_LOGINFOF("time: %d", std::chrono::duration_cast<std::chrono::microseconds>(step_time).count());
		// sleep for SERVER_LOOP_TIME (ms) - loop duration (ms)
		std::this_thread::sleep_for(
				std::chrono::milliseconds(((int)(SERVER_LOOP_TIME * 1000)) -
				std::chrono::duration_cast<std::chrono::milliseconds>(step_time).count())
		);
		prev_time = std::chrono::system_clock::now();
	}

	return nullptr;
}

void Server::step(const float dtime)
{

}
}
}
