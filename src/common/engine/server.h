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
#include <kNet/DataDeserializer.h>

namespace spacel {
namespace engine {

class Database;

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
	inline const ServerLoadingStep getLoadingStep() const { return m_loading_step; }

	void handlePacket_Null(kNet::DataDeserializer *data) {};
	void handlePacket_Hello(kNet::DataDeserializer *data);
	void handlePacket_Chat(kNet::DataDeserializer *data);
	void handlePacket_Auth(kNet::DataDeserializer *data);
private:
	const bool InitServer();
	const bool LoadGameDatas();
	void StopServer();
	void Step(const float dtime);

	std::string m_gamedatapath = "";
	std::string m_datapath = "";
	std::string m_universe_name = "";
	Database *m_db = nullptr;
	std::atomic<ServerLoadingStep> m_loading_step;
};

}
}
