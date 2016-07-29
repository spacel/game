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
#include <common/engine/space.h>
#include "spacelgame.h"

namespace spacel {

using namespace engine::network;

enum ClientLoadingStep {
	CLIENTLOADINGSTEP_NOT_STARTED = 0,
	CLIENTLOADINGSTEP_BEGIN_START,
	CLIENTLOADINGSTEP_CONNECTED,
	CLIENTLOADINGSTEP_AUTHED,
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
	~Client();

	void ThreadFunction();
	inline const ClientLoadingStep GetLoadingStep() const { return m_loading_step; }
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
	void SetUniverseName(const std::string &universe_name) { m_universe_name = universe_name; }
	void SetGameDataPath(const std::string &data_path) { m_gamedata_path = data_path; }
	void SetDataPath(const std::string &data_path) { m_data_path = data_path; }
	void SetUIEventHandler(SpacelGame *event_handler) { m_ui_event_handler = event_handler; }

	void ReceivePacket(NetworkPacket *packet)
	{
		m_packet_receive_queue.push_back(packet);
	}

	void handlePacket_Null(engine::network::NetworkPacket *packet) {}
	void handlePacket_Hello(engine::network::NetworkPacket *packet);
	void handlePacket_Chat(engine::network::NetworkPacket *packet);
	void handlePacket_GalaxySystems(engine::network::NetworkPacket *packet);
	void handlePacket_Auth(engine::network::NetworkPacket *packet);
	void handlePacket_CharacterList(engine::network::NetworkPacket *packet);
	void handlePacket_CharacterCreate(engine::network::NetworkPacket *packet);
	void handlePacket_CharacterRemove(engine::network::NetworkPacket *packet);
	void handlePacket_Kick(engine::network::NetworkPacket *packet);

	// Queue event incoming from UI to client
	void QueueClientUiEvent(ClientUIEventPtr e)
	{
		m_clientui_event_queue.push_back(e);
	}

	void handleClientUiEvent_ChararacterAdd(ClientUIEventPtr event);
	void handleClientUiEvent_ChararacterRemove(ClientUIEventPtr event);
private:
	void Step(const float dtime);
	void ProcessPacket(engine::network::NetworkPacket *packet);
	void RoutePacket(engine::network::NetworkPacket *packet);
	void SendPacket(engine::network::NetworkPacket *packet);

	void SendInitPacket();

	// Queue event incoming from client to UI
	inline void QueueUIEvent(UIEvent *event)
	{
		m_ui_event_handler->QueueUiEvent(
				UIEventPtr(event));
	}

	static Client *s_client;
	std::atomic<ClientLoadingStep> m_loading_step;
	SessionState m_state = SESSION_STATE_NONE;

	bool m_singleplayer_mode = false;
	std::string m_universe_name = "";
	std::string m_gamedata_path = "";
	std::string m_data_path = "";
	SpacelGame *m_ui_event_handler = nullptr;

	engine::Server *m_server = nullptr;

	SafeQueue<engine::network::NetworkPacket *> m_packet_sending_queue;
	SafeQueue<engine::network::NetworkPacket *> m_packet_receive_queue;

	ClientUIEventQueue m_clientui_event_queue;

	engine::SolarSystemMap m_solar_systems;
};
}
