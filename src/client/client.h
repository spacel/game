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
#include "../../lib/Urho3D/Source/ThirdParty/kNet/include/kNet/DataDeserializer.h"

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

	void handlePacket_Null(kNet::DataDeserializer *data) {}
	void handlePacket_Hello(kNet::DataDeserializer *data);
	void handlePacket_Chat(kNet::DataDeserializer *data);
	void handlePacket_GalaxySystems(kNet::DataDeserializer *data);
private:
	void Step(const float dtime);

	static Client *s_client;
	std::atomic<ClientLoadingStep> m_loading_step;
};
}
