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

#include <Urho3D/Core/Thread.h>
#include <string>
#include <atomic>
#include "network/networkprotocol.h"
#include "../threadsafe_utils.h"

namespace spacel {
namespace engine {

class Database;

/*
 * Started and failed states should be at the end of the end
 * Client awaits for STARTED or FAILED state to continue
 */
enum ServerLoadingStep
{
	SERVERLOADINGSTEP_NOT_STARTED = 0,
	SERVERLOADINGSTEP_BEGIN_START,
	SERVERLOADINGSTEP_DB_INITED,
	SERVERLOADINGSTEP_GAMEDATAS_LOADED,
	SERVERLOADINGSTEP_STARTED,
	SERVERLOADINGSTEP_FAILED,
	SERVERLOADINGSTEP_COUNT,
};

class Server: public Urho3D::Thread
{
public:
	Server(const std::string &gamedatapath, const std::string &datapath,
		   const std::string &universe_name);
	void ThreadFunction();

	const ServerLoadingStep GetLoadingStep() const { return m_loading_step; }
	void SetSinglePlayerMode(const bool s) { m_singleplayer_mode = s; }
	void ReceivePacket(network::NetworkPacket *packet)
	{
		m_packet_receive_queue.push_back(packet);
	}

	void SendPacket(network::NetworkPacket *packet)
	{
		m_packet_sending_queue.push_back(packet);
	}

	const bool IsSendingQueueEmpty() { return m_packet_sending_queue.empty(); }
	network::NetworkPacket *PopSendingQueue() { return m_packet_sending_queue.pop_front(); }

	void handlePacket_Null(network::NetworkPacket *packet) {};
	void handlePacket_Hello(network::NetworkPacket *packet);
	void handlePacket_Chat(network::NetworkPacket *packet);
	void handlePacket_Auth(network::NetworkPacket *packet);
	void handlePacket_CharacterCreate(network::NetworkPacket *packet);
	void handlePacket_CharacterRemove(network::NetworkPacket *packet);
	void handlePacket_CharacterConnect(network::NetworkPacket *packet);
private:
	const bool InitServer();
	const bool LoadGameDatas();
	void StopServer();
	void Step(const float dtime);
	void ProcessPacket(network::NetworkPacket *packet);
	void RoutePacket(network::NetworkPacket *packet);

	bool m_singleplayer_mode = false;
	std::string m_gamedatapath = "";
	std::string m_datapath = "";
	std::string m_universe_name = "";
	Database *m_db = nullptr;
	std::atomic<ServerLoadingStep> m_loading_step;

	SafeQueue<network::NetworkPacket *> m_packet_sending_queue;
	SafeQueue<network::NetworkPacket *> m_packet_receive_queue;
};

}
}
