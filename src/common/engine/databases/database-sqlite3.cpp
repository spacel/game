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
#include "porting.h"
#include "database-sqlite3.h"
#include "time_utils.h"

namespace spacel {
namespace engine {

static const char* stmt_list[SQLITE3STMT_COUNT] = {
		"BEGIN",
		"END"
		"INSERT INTO `galaxies`(`galaxy_name`,`pos_x`,`pos_y`,`pos_z`) VALUES (?, ?, ?, ?)",
		"SELECT `galaxy_name`,`pos_x`,`pos_y`,`pos_z` FROM `galaxies` WHERE galaxy_id = ?",
		"INSERT INTO `solar_systems`(`galaxy_id`,`solarsystem_name`,`type`,`pos_x`,`pos_y`,`pos_z`,`radius`) VALUES (?, ?, ?, ?, ?, ?, ?)",
		"SELECT `galaxy_id`,`solarsystem_name`,`type`,`pos_x`,`pos_y`,`pos_z`,`radius` FROM `solar_systems` WHERE `solarsystem_id` = ?"
};

#define BUSY_INFO_THRESHOLD	100	// Print first informational message after 100ms.
#define BUSY_WARNING_THRESHOLD	250	// Print warning message after 250ms. Lag is increased.
#define BUSY_ERROR_THRESHOLD	1000	// Print error message after 1000ms. Significant lag.
#define BUSY_FATAL_THRESHOLD	3000	// Allow SQLITE_BUSY to be returned
#define BUSY_ERROR_INTERVAL	10000	// Safety net: report again every 10 seconds

DatabaseSQLite3::DatabaseSQLite3(const std::string &db_path):
	m_db_path(db_path + DIR_DELIM + "universe.db")
{
	Open();
}

void DatabaseSQLite3::Open()
{
	sqlite3_verify(sqlite3_open_v2(m_db_path.c_str(), &m_database,
	   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL));
	sqlite3_verify(sqlite3_busy_handler(m_database, DatabaseSQLite3::busyHandler,
		m_busy_handler_data));

	UpdateSchema();

	for (uint16_t i = 0; i < SQLITE3STMT_COUNT; i++) {
		URHO3D_LOGINFOF("Loading statement %d %s", i, stmt_list[i]);
		sqlite3_verify(sqlite3_prepare_v2(m_database, stmt_list[i], -1, &m_stmt[i], NULL));
	}
}

bool DatabaseSQLite3::Close()
{
	try {
		sqlite3_verify(sqlite3_close(m_database));
		return true;
	}
	catch (SQLiteException &) {
		return false;
	}
}

void DatabaseSQLite3::UpdateSchema()
{
	static const char *dbcreate_sql = "CREATE TABLE IF NOT EXISTS `gameversion` ("
		 "	version INT NOT NULL"
		 ");"
		 "INSERT INTO `gameversion` (`version`) VALUES (1);";

	sqlite3_verify(sqlite3_exec(m_database, dbcreate_sql, NULL, NULL, NULL));

	static const char *gameconfig_table_sql = "CREATE TABLE IF NOT EXISTS `gameconfig` ("
			"	universe_id INTEGER NOT NULL PRIMARY KEY,"
			"	universe_name VARCHAR(32) NOT NULL,"
			"	seed BIGINT NOT NULL"
			");";

	sqlite3_verify(sqlite3_exec(m_database, gameconfig_table_sql, NULL, NULL, NULL));

	static const char *galaxy_table_sql = "CREATE TABLE IF NOT EXISTS `galaxies` ("
		"	galaxy_id BIGINT NOT NULL PRIMARY KEY,"
		"	galaxy_name VARCHAR(32) NOT NULL,"
		"	pos_x REAL NOT NULL,"
		"	pos_y REAL NOT NULL,"
		"	pos_z REAL NOT NULL"
		");";

	sqlite3_verify(sqlite3_exec(m_database, galaxy_table_sql, NULL, NULL, NULL));

	static const char *solarsystem_table_sql = "CREATE TABLE IF NOT EXISTS `solar_systems` ("
		"	solarsystem_id BIGINT NOT NULL PRIMARY KEY,"
		"	galaxy_id INTEGER NOT NULL,"
		"	solarsystem_name VARCHAR(48) NOT NULL,"
		"	type SMALLINT NOT NULL,"
		"	pos_x REAL NOT NULL,"
		"	pos_y REAL NOT NULL,"
		"	pos_z REAL NOT NULL,"
		"	radius REAL NOT NULL,"
		"	FOREIGN KEY (galaxy_id) REFERENCES `galaxies` (galaxy_id) ON DELETE CASCADE"
		");";

	sqlite3_verify(sqlite3_exec(m_database, solarsystem_table_sql, NULL, NULL, NULL));

	static const char *planet_table_sql = "CREATE TABLE IF NOT EXISTS `planets` ("
			"	planet_id BIGINT NOT NULL PRIMARY KEY,"
			"	solarsystem_id INTEGER NOT NULL,"
			"	type SMALLINT NOT NULL,"
			"	pos_x REAL NOT NULL,"
			"	pos_y REAL NOT NULL,"
			"	pos_z REAL NOT NULL,"
			"	distance_to_parent REAL NOT NULL,"
			"	radius REAL NOT NULL,"
			"	FOREIGN KEY (solarsystem_id) REFERENCES `solar_systems` (solarsystem_id) ON DELETE CASCADE"
			");";

	sqlite3_verify(sqlite3_exec(m_database, planet_table_sql, NULL, NULL, NULL));

	static const char *moon_table_sql = "CREATE TABLE IF NOT EXISTS `moons` ("
			"	moon_id BIGINT NOT NULL PRIMARY KEY,"
			"	planet_id INTEGER NOT NULL,"
			"	type SMALLINT NOT NULL,"
			"	pos_x REAL NOT NULL,"
			"	pos_y REAL NOT NULL,"
			"	pos_z REAL NOT NULL,"
			"	distance_to_parent REAL NOT NULL,"
			"	radius REAL NOT NULL,"
			"	FOREIGN KEY (planet_id) REFERENCES `planets` (planet_id) ON DELETE CASCADE"
			");";

	sqlite3_verify(sqlite3_exec(m_database, moon_table_sql, NULL, NULL, NULL));
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

void DatabaseSQLite3::BeginTransaction()
{
	CheckDatabase();
	sqlite3_verify(stmt_step(SQLITE3STMT_BEGIN), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_BEGIN);
}

void DatabaseSQLite3::CommitTransaction()
{
	CheckDatabase();
	sqlite3_verify(stmt_step(SQLITE3STMT_END), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_END);
}

void DatabaseSQLite3::CreateGalaxy(Galaxy *galaxy)
{

}

bool DatabaseSQLite3::LoadGalaxy(const uint64_t &galaxy_id)
{
	return false;
}

void DatabaseSQLite3::CreateSolarSystem(engine::SolarSystem *ss)
{

}

bool DatabaseSQLite3::LoadSolarSystem(const uint64_t &ss_id)
{
	return false;
}

}
}
