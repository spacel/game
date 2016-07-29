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

#include <iostream>
#include <sqlite3.h>
#include "porting.h"
#include "database-sqlite3.h"
#include "time_utils.h"
#include "../../engine/space.h"

namespace spacel {
namespace engine {

static const char *stmt_list[SQLITE3STMT_COUNT] = {
		"BEGIN",
		"END",
		"INSERT INTO galaxies(galaxy_id, galaxy_name, pos_x, pos_y, pos_z) VALUES (?, ?, ?, ?, ?)",
		"SELECT `galaxy_name`,`pos_x`,`pos_y`,`pos_z` FROM `galaxies` WHERE galaxy_id = ?",
		"INSERT INTO `solar_systems`(`solarsystem_id`,`galaxy_id`,`solarsystem_name`,`type`,`pos_x`,`pos_y`,`pos_z`,`radius`) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
		"SELECT `galaxy_id`,`solarsystem_name`,`type`,`pos_x`,`pos_y`,`pos_z`,`radius` FROM `solar_systems` WHERE `solarsystem_id` = ?",
		"SELECT `solarsystem_id`,`solarsystem_name`,`type`,`pos_x`,`pos_y`,`pos_z`,`radius` FROM `solar_systems` WHERE `galaxy_id` = ?",
		"INSERT INTO `gameconfig` (`universe_name`, `seed`, `universe_birth`) VALUES (?, ?, ?)",
		"SELECT `seed`, `universe_birth` FROM `gameconfig` WHERE `universe_name` = ?",
		"SELECT `universe_generated` FROM `gameconfig` WHERE `universe_name` = ?",
		"UPDATE `gameconfig` SET `universe_generated` = ? WHERE `universe_name` = ?"
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
	URHO3D_LOGDEBUGF("Opening DB at %s", m_db_path.c_str());

	sqlite3_verify(sqlite3_open_v2(m_db_path.c_str(), &m_database,
	   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL));
	sqlite3_verify(sqlite3_busy_handler(m_database, DatabaseSQLite3::busyHandler,
		m_busy_handler_data));

	UpdateSchema();

	for (uint16_t i = 0; i < SQLITE3STMT_COUNT; i++) {
		URHO3D_LOGDEBUGF("Loading statement %d %s", i, stmt_list[i]);
		sqlite3_verify(sqlite3_prepare_v2(m_database, stmt_list[i], -1, &m_stmt[i], NULL));
	}
}

bool DatabaseSQLite3::Close()
{
	try {
		for (uint16_t i = 0; i < SQLITE3STMT_COUNT; i++) {
			URHO3D_LOGDEBUGF("Closing statement %d %s", i, stmt_list[i]);
			sqlite3_verify(sqlite3_finalize(m_stmt[i]));
		}
		sqlite3_verify(sqlite3_close_v2(m_database));
		URHO3D_LOGDEBUG("Database closed.");
		return true;
	}
	catch (SQLiteException &e) {
		URHO3D_LOGERRORF("Database closing error %s", e.what());
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
		"	universe_name VARCHAR(32) NOT NULL UNIQUE,"
		"	seed INTEGER NOT NULL,"
		"	universe_generated SMALLINT NOT NULL DEFAULT(0),"
		"	universe_birth BIGINT NOT NULL"
		");";

	sqlite3_verify(sqlite3_exec(m_database, gameconfig_table_sql, NULL, NULL, NULL));

	static const char *galaxy_table_sql = "CREATE TABLE IF NOT EXISTS `galaxies` ("
		"	galaxy_id INTEGER NOT NULL PRIMARY KEY,"
		"	galaxy_name VARCHAR(32) NOT NULL,"
		"	pos_x REAL NOT NULL,"
		"	pos_y REAL NOT NULL,"
		"	pos_z REAL NOT NULL"
		");";

	sqlite3_verify(sqlite3_exec(m_database, galaxy_table_sql, NULL, NULL, NULL));

	static const char *solarsystem_table_sql = "CREATE TABLE IF NOT EXISTS `solar_systems` ("
		"	solarsystem_id INTEGER NOT NULL PRIMARY KEY,"
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
		"	planet_id INTEGER NOT NULL PRIMARY KEY,"
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
		"	moon_id INTEGER NOT NULL PRIMARY KEY,"
		"	planet_id INTEGER NOT NULL,"
		"	type INTEGER NOT NULL,"
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

inline int DatabaseSQLite3::stmt_step(const SQLite3Stmt s)
{
	assert(s < SQLITE3STMT_COUNT);
	return sqlite3_step(m_stmt[s]);
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
	uint64_to_sqlite(SQLITE3STMT_CREATE_GALAXY, 1, galaxy->id);
	string_to_sqlite(SQLITE3STMT_CREATE_GALAXY, 2, galaxy->name.c_str());
	double_to_sqlite(SQLITE3STMT_CREATE_GALAXY, 3, galaxy->pos_x);
	double_to_sqlite(SQLITE3STMT_CREATE_GALAXY, 4, galaxy->pos_y);
	double_to_sqlite(SQLITE3STMT_CREATE_GALAXY, 5, galaxy->pos_z);

	sqlite3_verify(stmt_step(SQLITE3STMT_CREATE_GALAXY), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_CREATE_GALAXY);
}

Galaxy *DatabaseSQLite3::LoadGalaxy(const uint64_t &galaxy_id)
{
	Galaxy *galaxy = nullptr;
	uint64_to_sqlite(SQLITE3STMT_LOAD_GALAXY, 1, galaxy_id);
	if (stmt_step(SQLITE3STMT_LOAD_GALAXY) == SQLITE_ROW) {
		galaxy = new Galaxy();
		galaxy->id = galaxy_id;
		galaxy->name = sqlite_to_string(SQLITE3STMT_LOAD_GALAXY, 0);
		galaxy->pos_x = sqlite_to_double(SQLITE3STMT_LOAD_GALAXY, 1);
		galaxy->pos_y = sqlite_to_double(SQLITE3STMT_LOAD_GALAXY, 2);
		galaxy->pos_z = sqlite_to_double(SQLITE3STMT_LOAD_GALAXY, 3);
	}

	reset_stmt(SQLITE3STMT_LOAD_GALAXY);

	return galaxy;
}

void DatabaseSQLite3::CreateSolarSystem(engine::SolarSystem *ss)
{
	assert(ss->galaxy);

	uint64_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 1, ss->id);
	uint64_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 2, ss->galaxy->id);
	string_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 3, ss->name.c_str());
	uint16_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 4, ss->type);
	double_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 5, ss->pos_x);
	double_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 6, ss->pos_y);
	double_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 7, ss->pos_z);
	double_to_sqlite(SQLITE3STMT_CREATE_SOLARSYSTEM, 8, ss->radius);

	sqlite3_verify(stmt_step(SQLITE3STMT_CREATE_SOLARSYSTEM), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_CREATE_SOLARSYSTEM);
}

SolarSystem *DatabaseSQLite3::LoadSolarSystem(const uint64_t &ss_id)
{
	SolarSystem *solar_system = nullptr;
	uint64_to_sqlite(SQLITE3STMT_LOAD_SOLARSYSTEM, 1, ss_id);
	if (stmt_step(SQLITE3STMT_LOAD_SOLARSYSTEM) == SQLITE_ROW) {
		solar_system = new SolarSystem();
		// Warning we don't load solar system galaxy_id there
		solar_system->id = ss_id;
		solar_system->name = sqlite_to_string(SQLITE3STMT_LOAD_SOLARSYSTEM, 1);
		solar_system->type = (SolarType)sqlite_to_uint16(SQLITE3STMT_LOAD_SOLARSYSTEM, 2);
		solar_system->pos_x = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEM, 3);
		solar_system->pos_y = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEM, 4);
		solar_system->pos_z = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEM, 5);
		solar_system->radius = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEM, 6);
	}

	reset_stmt(SQLITE3STMT_LOAD_GALAXY);

	return solar_system;
}

void DatabaseSQLite3::LoadSolarSystemsForGalaxy(Galaxy *galaxy)
{
	uint64_to_sqlite(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 1, galaxy->id);
	while (stmt_step(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY) == SQLITE_ROW) {
		SolarSystem *solar_system = new SolarSystem();
		solar_system->id = sqlite_to_uint64(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 0);
		solar_system->name = sqlite_to_string(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 1);
		solar_system->type = (SolarType)sqlite_to_uint16(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 2);
		solar_system->pos_x = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 3);
		solar_system->pos_y = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 4);
		solar_system->pos_z = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 5);
		solar_system->radius = sqlite_to_double(SQLITE3STMT_LOAD_SOLARSYSTEMS_FOR_GALAXY, 6);
		galaxy->solar_systems[solar_system->id] = solar_system;
	}

	reset_stmt(SQLITE3STMT_LOAD_GALAXY);
}

void DatabaseSQLite3::CreateUniverse(const std::string &name, const uint64_t &seed)
{
	CheckDatabase();
	string_to_sqlite(SQLITE3STMT_CREATE_UNIVERSE, 1, name);
	uint64_to_sqlite(SQLITE3STMT_CREATE_UNIVERSE, 2, seed);
	uint64_to_sqlite(SQLITE3STMT_CREATE_UNIVERSE, 3, get_current_time());
	sqlite3_verify(stmt_step(SQLITE3STMT_CREATE_UNIVERSE), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_CREATE_UNIVERSE);
}

void DatabaseSQLite3::LoadUniverse(const std::string &name)
{
	CheckDatabase();
	string_to_sqlite(SQLITE3STMT_LOAD_UNIVERSE, 1, name);
	if (stmt_step(SQLITE3STMT_LOAD_UNIVERSE) == SQLITE_ROW) {
		engine::Universe::instance()->SetUniverseName(name);
		engine::Universe::instance()->SetUniverseSeed(sqlite_to_uint64(SQLITE3STMT_LOAD_UNIVERSE, 0));
		engine::Universe::instance()->SetUniverseBirth(sqlite_to_uint32(SQLITE3STMT_LOAD_UNIVERSE, 1));
		reset_stmt(SQLITE3STMT_LOAD_UNIVERSE);
		return;
	}
	reset_stmt(SQLITE3STMT_LOAD_UNIVERSE);
}

const bool DatabaseSQLite3::IsUniverseGenerated(const std::string &name)
{
	CheckDatabase();
	string_to_sqlite(SQLITE3STMT_LOAD_UNIVERSE_GENERATED_FLAG, 1, name);
	if (stmt_step(SQLITE3STMT_LOAD_UNIVERSE_GENERATED_FLAG) == SQLITE_ROW) {
		return sqlite_to_bool(SQLITE3STMT_LOAD_UNIVERSE_GENERATED_FLAG, 0);
	}
	return false;
}

void DatabaseSQLite3::SetUniverseGenerated(const std::string &name, bool generated)
{
	CheckDatabase();
	bool_to_sqlite(SQLITE3STMT_SET_UNIVERSE_GENERATED_FLAG, 1, generated);
	string_to_sqlite(SQLITE3STMT_SET_UNIVERSE_GENERATED_FLAG, 2, name);
	sqlite3_verify(stmt_step(SQLITE3STMT_SET_UNIVERSE_GENERATED_FLAG), SQLITE_DONE);
	reset_stmt(SQLITE3STMT_SET_UNIVERSE_GENERATED_FLAG);
}
}
}
