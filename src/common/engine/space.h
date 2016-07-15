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

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace spacel {

namespace engine {

// Generic objects
struct StellarObject
{
	uint64_t id;
	std::string name;
	double radius;
};

struct StellarPositionnedObject: public StellarObject
{
	double pos_x, pos_y, pos_z;
};

// Type definitions
enum PlanetType
{
	PLANET_TYPE_BINARY,
	PLANET_TYPE_CARBON,
	PLANET_TYPE_CORELESS,
	PLANET_TYPE_DESERT,
	PLANET_TYPE_EARTH,
	PLANET_TYPE_GAS_GIANT,
	PLANET_TYPE_HELIUM,
	PLANET_TYPE_ICE_GIANT,
	PLANET_TYPE_IRON,
	PLANET_TYPE_LAVA,
	PLANET_TYPE_OCEAN,
	PLANET_TYPE_MAX
};

// https://en.wikipedia.org/wiki/Stellar_classification
enum SolarType
{
	SOLAR_TYPE_CLASSIC,
	SOLAR_TYPE_WHITE_DWARF,
	SOLAR_TYPE_BLACK_DWARF,
	SOLAR_TYPE_BROWN_DWARF,
	SOLAR_TYPE_RED_DWARF,
	SOLAR_TYPE_BIG_BLUE,
	SOLAR_TYPE_PULSAR,
	SOLAR_TYPE_BLACK_HOLE,
	SOLAR_TYPE_SUPERNOVAE,
	SOLAR_TYPE_MAX,
};

/**
 * Stellar objects
 */

/*
 * Moons
 */
struct Moon: public StellarObject
{
	PlanetType type;
	double distance_to_parent;
};

/*
 * Planets
 */
struct Planet: public Moon
{
	~Planet();
	std::vector<Moon *> moons;
};

/*
 * Solar systems
 */
struct Galaxy; // Predefine for circular dep

struct SolarSystem: public StellarPositionnedObject
{
	~SolarSystem();
	SolarType type;
	std::vector<Planet *> planets;
	Galaxy *galaxy = nullptr;
};
typedef std::unordered_map<uint64_t, SolarSystem *> SolarSystemMap;

/*
 * Galaxies
 */
struct Galaxy: public StellarPositionnedObject
{
	~Galaxy();
	SolarSystemMap solar_systems;
};
typedef std::unordered_map<uint64_t, Galaxy *> GalaxyMap;

/*
 * The whole universe
 */
class Universe
{
public:
	Universe() {}
	~Universe();

	bool SetGalaxy(Galaxy *galaxy);
	Galaxy *CreateGalaxy(const uint64_t &max_solar_systems);
	bool RemoveGalaxy(const uint64_t &id);
	Galaxy *GetGalaxy(const uint64_t &id);

	SolarSystem *CreateSolarSystem(Galaxy *galaxy);
	void CreateSolarSystemPhase2(SolarSystem *ss);
	bool RemoveSolarSystem(const uint64_t &id);

	void SetUniverseName(const std::string &name) {	m_name = name; }
	const std::string GetUniverseName() const { return m_name; }

	void SetUniverseSeed(const uint64_t &seed) { m_seed = seed;	}
	const uint64_t GetUniverseSeed() const { return m_seed; }

	void SetUniverseBirth(const uint32_t birth) { m_birth = birth; }
	const uint32_t GetUniverseBirth() const { return m_birth; }

	inline static Universe *instance()
	{
		if (!Universe::s_universe) {
			Universe::s_universe = new Universe();
		}

		return Universe::s_universe;
	}
private:
	SolarSystemMap m_solar_systems;
	GalaxyMap m_galaxies;

	uint64_t m_next_solarsystem_id = 1;
	uint64_t m_next_galaxy_id = 1;
	uint64_t m_next_planet_id = 1;

	std::string m_name;
	uint64_t m_seed;
	uint32_t m_birth;

	static Universe *s_universe;
};

}

}
