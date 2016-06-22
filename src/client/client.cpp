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
#include <Urho3D/IO/Log.h>
#include <thread>
#include "project_defines.h"
#include "network/clientpackethandler.h"

namespace spacel {

using namespace engine::network;

#define CLIENT_LOOP_TIME 0.025f

Client::Client()
{
	m_loading_step = CLIENTLOADINGSTEP_NOT_STARTED;
}

void Client::ThreadFunction()
{
	if (!InitClient()) {
		URHO3D_LOGERROR("Failed to init client, aborting!");
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
		if (runtime < CLIENT_LOOP_TIME) {
			// Convert back to ms from s
			std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)((CLIENT_LOOP_TIME - runtime) * 1000.0f)));
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

	/*kNet::DataSerializer s;
	s.Add<uint8_t>(0);
	s.Add<uint8_t>(0);
	s.Add<uint8_t>(PROJECT_VERSION_PATCH);*/
	m_loading_step = CLIENTLOADINGSTEP_CONNECTED;

	m_loading_step = CLIENTLOADINGSTEP_GAMEDATAS_LOADED;

	m_loading_step = CLIENTLOADINGSTEP_STARTED;
	return true;
}

void Client::Step(const float dtime)
{
	while (!m_packet_receive_queue.empty()) {
		std::unique_ptr<NetworkPacket> pkt(m_packet_receive_queue.pop_front());
		ProcessPacket(pkt.get());
	}
}

void Client::ProcessPacket(NetworkPacket *packet)
{
	// Ignore invalid opcode
	if (packet->opcode >= MSG_MAX) {
		return;
	}

	// @TODO verify current client state

	RoutePacket(packet);
}

void Client::RoutePacket(NetworkPacket *packet)
{
	const CMsgHandler &opHandle = cmsgHandlerTable[packet->opcode];
	(this->*opHandle.handler)(packet);
}

void Client::handlePacket_Hello(NetworkPacket *packet)
{

}

void Client::handlePacket_Chat(NetworkPacket *packet)
{

}

void Client::handlePacket_GalaxySystems(NetworkPacket *packet)
{

}


}
