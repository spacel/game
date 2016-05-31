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
	double diameter;
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

// Stellar objects
struct Moon: public StellarObject
{
	PlanetType type;
	double distance_to_parent;
};

struct Planet: public Moon
{
	std::vector<Moon*> moons;
};

struct SolarSystem: public StellarPositionnedObject
{
	SolarType type;
	std::vector<Planet*> planets;
};
typedef std::unordered_map<uint64_t, SolarSystem*> SolarSystemMap;

struct Galaxy: public StellarPositionnedObject
{
	SolarSystemMap solar_systems;
};
typedef std::unordered_map<uint64_t, Galaxy*> GalaxyMap;

class Universe
{
public:
	Universe() {}
	~Universe() {}
private:
	GalaxyMap m_galaxies;
};

}

}
