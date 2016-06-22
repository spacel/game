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

#pragma once

#include <atomic>
#include <Urho3D/Core/Thread.h>
#include <queue>
#include <common/threadsafe_utils.h>
#include <common/engine/network/networkprotocol.h>

namespace spacel {

enum ClientLoadingStep {
	CLIENTLOADINGSTEP_NOT_STARTED = 0,
	CLIENTLOADINGSTEP_BEGIN_START,
	CLIENTLOADINGSTEP_CONNECTED,
	CLIENTLOADINGSTEP_GAMEDATAS_LOADED,
	CLIENTLOADINGSTEP_STARTED,
	CLIENTLOADINGSTEP_FAILED,
	CLIENTLOADINGSTEP_ENDED,
	CLIENTLOADINGSTEP_COUNT,
};

class Client: public Urho3D::Thread
{
public:
	Client();
	~Client() {}

	void ThreadFunction();
	inline const ClientLoadingStep getLoadingStep() const { return m_loading_step; }
	bool InitClient();

	inline static Client *instance()
	{
		if (!Client::s_client) {
			Client::s_client = new Client();
		}

		return Client::s_client;
	}

	inline static void deinstance()
	{
		if (Client::s_client) {
			delete Client::s_client;
			Client ::s_client = nullptr;
		}
	}

	void SetSinglePlayerMode(const bool s) { m_singleplayer_mode = s; }


	void handlePacket_Null(engine::network::NetworkPacket *data) {}
	void handlePacket_Hello(engine::network::NetworkPacket *data);
	void handlePacket_Chat(engine::network::NetworkPacket *data);
	void handlePacket_GalaxySystems(engine::network::NetworkPacket *data);
private:
	void Step(const float dtime);
	void ProcessPacket(engine::network::NetworkPacket *packet);
	void RoutePacket(engine::network::NetworkPacket *packet);

	static Client *s_client;
	std::atomic<ClientLoadingStep> m_loading_step;
	bool m_singleplayer_mode = false;

	SafeQueue<engine::network::NetworkPacket *> m_packet_sending_queue;
	SafeQueue<engine::network::NetworkPacket *> m_packet_receive_queue;
};
}
