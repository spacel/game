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
	SQLITE3STMT_END,
	SQLITE3STMT_CREATE_GALAXY,
	SQLITE3STMT_LOAD_GALAXY,
	SQLITE3STMT_CREATE_SOLARSYSTEM,
	SQLITE3STMT_LOAD_SOLARSYSTEM,
	SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY,
	SQLITE3STMT_CREATE_UNIVERSE,
	SQLITE3STMT_LOAD_UNIVERSE,
	SQLITE3STMT_LOAD_UNIVERSE_GENERATED_FLAG,
	SQLITE3STMT_SET_UNIVERSE_GENERATED_FLAG,
	SQLITE3STMT_COUNT,
};

class DatabaseSQLite3: public Database
{
public:
	DatabaseSQLite3(const std::string &db_path);
	~DatabaseSQLite3() { Close(); }

	// Transactions related
	void BeginTransaction();
	void CommitTransaction();

	void CreateGalaxy(Galaxy *galaxy);
	Galaxy *LoadGalaxy(const uint64_t &galaxy_id);
	void CreateSolarSystem(engine::SolarSystem *ss);
	SolarSystem *LoadSolarSystem(const uint64_t &ss_id);
	void LoadSolarSystemsForGalaxy(Galaxy *galaxy);
	void CreateUniverse(const std::string &name, const uint64_t &seed);
	void LoadUniverse(const std::string &name);
	void SetUniverseGenerated(const std::string &name, bool generated);
	const bool IsUniverseGenerated(const std::string &name);

private:
	void Open();
	bool Close();
	void UpdateSchema();
	void CheckDatabase() {}

	// Common Sqlite interfaces
	static int busyHandler(void *data, int count);

	inline void sqlite3_verify(const int s, const int r = SQLITE_OK) const
	{
		if (s != r) {
			throw SQLiteException(sqlite3_errmsg(m_database));
		}
	}

	int stmt_step(const SQLite3Stmt s);

	inline int reset_stmt(const SQLite3Stmt s)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_reset(m_stmt[s]);
	}

	inline void bool_to_sqlite(const SQLite3Stmt s, const int iCol, const bool val) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_int(m_stmt[s], iCol, (val ? 1 : 0)));
	}

	inline const bool sqlite_to_bool(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return (sqlite3_column_int(m_stmt[s], iCol) > 0);
	}

	inline void string_to_sqlite(const SQLite3Stmt s, const int iCol, const std::string &str) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_text(m_stmt[s], iCol, str.c_str(), str.size(), NULL));
	}

	inline void uint64_to_sqlite(const SQLite3Stmt s, const int iCol, const uint64_t &val) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_int64(m_stmt[s], iCol, (sqlite3_int64)val));
	}

	inline void uint16_to_sqlite(const SQLite3Stmt s, const int iCol, const uint16_t val) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_int(m_stmt[s], iCol, (int)val));
	}

	inline const std::string sqlite_to_string(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		const char *text = reinterpret_cast<const char *>(sqlite3_column_text(m_stmt[s], iCol));
		return std::string(text ? text : "");
	}

	inline const uint16_t sqlite_to_uint16(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return (uint16_t)sqlite3_column_int(m_stmt[s], iCol);
	}

	inline const uint32_t sqlite_to_uint32(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return (uint32_t)sqlite3_column_int(m_stmt[s], iCol);
	}

	inline const int32_t sqlite_to_int(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_column_int(m_stmt[s], iCol);
	}

	inline const uint64_t sqlite_to_uint64(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return (uint64_t)sqlite3_column_int64(m_stmt[s], iCol);
	}

	inline const float sqlite_to_float(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_column_double(m_stmt[s], iCol);
	}

	inline const double sqlite_to_double(const SQLite3Stmt s, int iCol)
	{
		assert(s < SQLITE3STMT_COUNT);
		return sqlite3_column_double(m_stmt[s], iCol);
	}

	inline void double_to_sqlite(const SQLite3Stmt s, const int iCol, const float val) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_double(m_stmt[s], iCol, (double)val));
	}

	inline void double_to_sqlite(const SQLite3Stmt s, const int iCol, const double val) const
	{
		assert(s < SQLITE3STMT_COUNT);
		sqlite3_verify(sqlite3_bind_double(m_stmt[s], iCol, val));
	}

	std::string m_db_path = "";
	sqlite3 *m_database;
	int64_t m_busy_handler_data[2];
	sqlite3_stmt *m_stmt[SQLITE3STMT_COUNT];
};

}

}
