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

#include "../../exception_utils.h"

namespace spacel {

namespace engine {

struct Galaxy;
struct SolarSystem;

class Database
{
public:
	Database() {}
	virtual ~Database() {}

	// transactions
	virtual void BeginTransaction() = 0;
	virtual void CommitTransaction() = 0;

	virtual void CreateGalaxy(Galaxy *galaxy) = 0;
	virtual Galaxy *LoadGalaxy(const uint64_t &galaxy_id) = 0;
	virtual void CreateSolarSystem(engine::SolarSystem *ss) = 0;
	virtual SolarSystem *LoadSolarSystem(const uint64_t &ss_id) = 0;
	virtual void LoadSolarSystemsForGalaxy(Galaxy *galaxy) = 0;
	virtual void SetUniverseGenerated(const std::string &name, bool generated) = 0;
	virtual const bool IsUniverseGenerated(const std::string &name) = 0;

private:
	virtual void Open() = 0;
	virtual void UpdateSchema() = 0;
	virtual bool Close() = 0;
	virtual void CheckDatabase() = 0;

};
}
}
