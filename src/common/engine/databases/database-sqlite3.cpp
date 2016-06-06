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

#include <sqlite3.h>
#include "database-sqlite3.h"
#include "time_utils.h"

namespace spacel {
namespace engine {

class SQLiteException: public Exception
{
public:
	SQLiteException(const std::string &s): Exception("SQLite3: " + s) {}
};

#define BUSY_INFO_THRESHOLD	100	// Print first informational message after 100ms.
#define BUSY_WARNING_THRESHOLD	250	// Print warning message after 250ms. Lag is increased.
#define BUSY_ERROR_THRESHOLD	1000	// Print error message after 1000ms. Significant lag.
#define BUSY_FATAL_THRESHOLD	3000	// Allow SQLITE_BUSY to be returned
#define BUSY_ERROR_INTERVAL	10000	// Safety net: report again every 10 seconds

void DatabaseSQLite3::open()
{
	if (!sqlite3_verify(sqlite3_open_v2(m_db_path.c_str(), &m_database,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))) {
		throw SQLiteException("Unable to open " + m_db_path + " universe file");
	}

	if (!sqlite3_verify(sqlite3_busy_handler(m_database, DatabaseSQLite3::busyHandler,
			m_busy_handler_data))) {
		throw SQLiteException("Unable to bind the busy handler");
	}

	updateSchema();
}

bool DatabaseSQLite3::close()
{
	return sqlite3_verify(sqlite3_close(m_database));
}

void DatabaseSQLite3::updateSchema()
{
	throw SQLiteException("Schema not implemented");
}

int DatabaseSQLite3::busyHandler(void *data, int count)
{
	int64_t &first_time = reinterpret_cast<int64_t *>(data)[0];
	int64_t &prev_time = reinterpret_cast<int64_t *>(data)[1];
	int64_t cur_time = get_current_time_ms();


	if (count == 0) {
		first_time = cur_time;
		prev_time = first_time;
	} else {
		while (cur_time < prev_time)
			cur_time += int64_t(1)<<32;
	}

	if (cur_time - first_time < BUSY_INFO_THRESHOLD) {
		; // do nothing
	} else if (cur_time - first_time >= BUSY_INFO_THRESHOLD &&
			   prev_time - first_time < BUSY_INFO_THRESHOLD) {
		URHO3D_LOGINFOF("SQLite3 database has been locked for %d ms.",
			cur_time - first_time);
	} else if (cur_time - first_time >= BUSY_WARNING_THRESHOLD &&
			   prev_time - first_time < BUSY_WARNING_THRESHOLD) {
		URHO3D_LOGWARNINGF("SQLite3 database has been locked for %d ms.",
			cur_time - first_time);
	} else if (cur_time - first_time >= BUSY_ERROR_THRESHOLD &&
			   prev_time - first_time < BUSY_ERROR_THRESHOLD) {
		URHO3D_LOGERRORF("SQLite3 database has been locked for %d ms; this causes lag.",
			cur_time - first_time);
	} else if (cur_time - first_time >= BUSY_FATAL_THRESHOLD &&
			   prev_time - first_time < BUSY_FATAL_THRESHOLD) {
		URHO3D_LOGERRORF("SQLite3 database has been locked for %d ms - giving up!",
			cur_time - first_time);
	} else if ((cur_time - first_time) / BUSY_ERROR_INTERVAL !=
			   (prev_time - first_time) / BUSY_ERROR_INTERVAL) {
		// Safety net: keep reporting every BUSY_ERROR_INTERVAL
		URHO3D_LOGERRORF("SQLite3 database has been locked for %d seconds!",
		(cur_time - first_time) / 1000);
	}

	prev_time = cur_time;

	// Make sqlite transaction fail if delay exceeds BUSY_FATAL_THRESHOLD
	return cur_time - first_time < BUSY_FATAL_THRESHOLD;
}

}
}
