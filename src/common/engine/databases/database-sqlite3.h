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

class SQLiteException: public Exception
{
public:
	SQLiteException(const std::string &s): Exception("SQLite3: " + s) {}
};

enum SQLite3Stmt
{
	SQLITE3STMT_BEGIN = 0,
	SQLITE3STMT_END = 1,
	SQLITE3STMT_COUNT,
};

class DatabaseSQLite3: public Database
{
public:
	DatabaseSQLite3(const std::string &db_path): m_db_path(db_path) { Open(); }
	~DatabaseSQLite3() { Close(); }
private:
	void Open();
	bool Close();
	void UpdateSchema();
	void CheckDatabase() {}

	// Transactions related
	void BeginTransaction();
	void CommitTransaction();


	static int busyHandler(void *data, int count);

	inline bool sqlite3_verify(const int s, const int r = SQLITE_OK) const
	{
		return s == r;
	}

	inline int stmt_step(const SQLite3Stmt s)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_step(m_stmt[s]);
	}

	inline int reset_stmt(const SQLite3Stmt s)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_reset(m_stmt[s]);
	}

	std::string m_db_path = "";
	sqlite3 *m_database;
	int64_t m_busy_handler_data[2];
	sqlite3_stmt *m_stmt[SQLITE3STMT_COUNT];
};

}

}