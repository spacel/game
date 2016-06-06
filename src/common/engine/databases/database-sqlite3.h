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

#include <string>
#include <Urho3D/IO/Log.h>
#include <sqlite3.h>
#include "database.h"

namespace spacel {

namespace engine {

class DatabaseSQLite3: public Database
{
public:
	DatabaseSQLite3(const std::string &db_path): m_db_path(db_path) { open(); }
	~DatabaseSQLite3() { close(); }
private:
	void open();
	bool close();
	void updateSchema();

	void checkDatabase() {}

	static int busyHandler(void *data, int count);

	inline bool sqlite3_verify(const int s, const int r = SQLITE_OK) const
	{
		return s == r;
	}
	std::string m_db_path = "";
	sqlite3 *m_database;
	int64_t m_busy_handler_data[2];
};

}

}
