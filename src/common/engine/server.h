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

#include "../thread_utils.h"

namespace spacel {
namespace engine {

class Database;

class Server: public Thread
{
public:
	Server(const std::string &datapath): Thread(), m_datapath(datapath) {}
	void* run();
private:
	const bool InitServer();
	void StopServer();
	void Step(const float dtime);

	std::string m_datapath;
	Database *m_db = nullptr;
};

}
}
