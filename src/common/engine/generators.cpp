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

#include <macro_utils.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include "generators.h"
#include "space.h"

namespace spacel {
namespace engine {
static const char *name_prefixes[] = {
		"ak",
		"bac", "bak",
		"car"
		"dam", "dem", "dom", "did", "dud", "det", "den", "dav",
		"plis", "pram", "prom",
		"tran", "tep", "trin",
		"vat", "vin", "vop"
};

static const char *name_suffixes[] = {
		"ar", "as", "at",
		"er", "es", "es",
		"ir", "is", "ix",
		"or", "ous",
		"ur", "us",
};

static const char *name_stem[] = {
		"d", "f", "m", "ss", "st", "tr", "l", "j"
};

static const char *vowels[] = {
		"a", "e", "i", "o", "u", "y", "ou", "ae", "ie", "io"
};

void UniverseGenerator::InitRandomGeneratorIfNot()
{
	if (!m_random_generator_inited) {
		m_random_generator = std::mt19937(s_seed);
		m_random_generator_galaxy_positions = std::mt19937(s_seed);

		m_vowel_generator = std::uniform_int_distribution<uint8_t>(0, ARRLEN(vowels) - 1);
		m_name_generators[0] = std::uniform_int_distribution<uint16_t>(0, ARRLEN(name_prefixes) - 1);
		m_name_generators[1] = std::uniform_int_distribution<uint16_t>(0, ARRLEN(name_suffixes) - 1);
		m_name_generators[2] = std::uniform_int_distribution<uint16_t>(0, ARRLEN(name_stem) - 1);
		m_name_generators[3] = std::uniform_int_distribution<uint16_t>(0, 2);
		m_random_generator_inited = true;
	}
}

std::string UniverseGenerator::generate_world_name()
{
	InitRandomGeneratorIfNot();

	std::string res = name_prefixes[m_name_generators[0](m_random_generator)];
	uint8_t stem_number = m_name_generators[3](m_random_generator) % 2;

	for (uint8_t i = 0; i < stem_number; i++) {
		res += vowels[m_vowel_generator(m_random_generator)];
		res += name_stem[m_name_generators[2](m_random_generator)];
	}

	res += name_suffixes[m_name_generators[1](m_random_generator)];

	return res;
}

struct SolarSystemGeneratorDef {
	float chance;
	uint8_t max_planets;
};

static constexpr SolarSystemGeneratorDef ss_defs[SOLAR_TYPE_MAX] = {
	{ 36.0f, 9 }, // SOLAR_TYPE_CLASSIC,
	{ 10.0f, 7 }, // SOLAR_TYPE_WHITE_DWARF,
	{ 1.75f, 6 }, // SOLAR_TYPE_BLACK_DWARF,
	{ 7.0f, 6 }, // SOLAR_TYPE_BROWN_DWARF,
	{ 19.0f, 9 }, // SOLAR_TYPE_RED_DWARF,
	{ 17.0f, 4 }, // SOLAR_TYPE_BIG_BLUE,
	{ 0.05f, 4 }, // SOLAR_TYPE_PULSAR,
	{ 0.1f, 0 }, // SOLAR_TYPE_BLACK_HOLE,
	{ 0.1f, 0 }, // SOLAR_TYPE_SUPERNOVAE,
};

constexpr float solarsystem_chance_max()
{
	float accumulator = 0.0f;
	for (uint8_t i = 0; i < SOLAR_TYPE_MAX; i++) {
		accumulator += ss_defs[i].chance;
	}
	return accumulator;
}

uint8_t UniverseGenerator::generate_solarsystem_type(const uint64_t &ss_id)
{
	std::mt19937 rndgen(s_seed + ss_id + 256);
	std::uniform_real_distribution<double> rnd(0.0f, solarsystem_chance_max());

	float chance_value = rnd(rndgen);
	float chance_accumulator = 0.0f;

	// Increment the accumulator with the current planet chance, when the accumulator was
	// greater than value, we have our planet type
	for (uint8_t pt = 0; pt < SOLAR_TYPE_MAX; pt++) {
		chance_accumulator += ss_defs[pt].chance;
		if (chance_value < chance_accumulator) {
			return pt;
		}
	}

	// This should not happen
	assert(false);
}

double UniverseGenerator::generate_solarsystem_radius(const uint64_t &ss_id)
{
	std::mt19937 rndgen(s_seed + ss_id + 256 * 256);
	// 10 Billion to 20 Trillion kilometers
	std::uniform_real_distribution<double> rnd(10 * 1000.0f * 1000.0f * 1000.0f,
		20.0f * 1000.0f * 1000.0f * 1000.0f * 1000.0f);
	return rnd(rndgen);
}

uint8_t UniverseGenerator::generate_solarsystem_planetnumber(const SolarSystem *ss)
{
	if (ss_defs[ss->type].max_planets == 0) {
		return 0;
	}

	std::mt19937 rndgen(s_seed + ss->id + 256 * 256 * 256);
	std::uniform_int_distribution<uint8_t> rnd(0, ss_defs[ss->type].max_planets);
	return rnd(rndgen);
}

struct PlanetGeneratorDef {
	float distance_scaling_factor_min;
	float radius_scaling_factor_min;
	float radius_scaling_factor_max;
	float chance;
};

// Chance field addition should match 100.0f for convenience
static constexpr PlanetGeneratorDef pg_defs[PLANET_TYPE_MAX] = {
	{ 2.0f,		1.0f,	1.0f,	9.0f }, // PLANET_TYPE_BINARY
	{ 1.75f, 	1.0f,	1.0f,	9.0f }, // PLANET_TYPE_CARBON
	{ 1.8f,		0.5f,	0.75f,	5.0f }, // PLANET_TYPE_CORELESS
	{ 1.6f,		1.0f,	2.0f,	11.0f }, // PLANET_TYPE_DESERT
	{ 10.0f,	1.0f,	1.0f,	10.0f }, // PLANET_TYPE_EARTH
	{ 20.0f,	50.0f,	2.0f,	11.0f }, // PLANET_TYPE_GAS_GIANT
	{ 30.0f,	2.0f,	1.0f,	9.0f }, // PLANET_TYPE_HELIUM
	{ 40.0f,	45.0f,	2.0f,	9.0f }, // PLANET_TYPE_ICE_GIANT
	{ 1.2f,		0.5f,	0.8f,	9.0f }, // PLANET_TYPE_IRON
	{ 1.0f,		1.0f,	1.0f,	10.0f }, // PLANET_TYPE_LAVA
	{ 2.2f,		1.0f,	1.0f,	8.0f }, // PLANET_TYPE_OCEAN
};

constexpr float planet_chance_max()
{
	float accumulator = 0.0f;
	for (uint8_t i = 0; i < PLANET_TYPE_MAX; i++) {
		accumulator += pg_defs[i].chance;
	}
	return accumulator;
}

uint8_t UniverseGenerator::generate_planet_type(const uint64_t &pl_id)
{
	std::mt19937 rndgen(s_seed + pl_id + 256 * pl_id);
	std::uniform_real_distribution<double> rnd(0.0f, planet_chance_max());

	float chance_value = rnd(rndgen);
	float chance_accumulator = 0.0f;

	// Increment the accumulator with the current planet chance, when the accumulator was
	// greater than value, we have our planet type
	for (uint8_t pt = 0; pt < PLANET_TYPE_MAX; pt++) {
		chance_accumulator += pg_defs[pt].chance;
		if (chance_value < chance_accumulator) {
			return pt;
		}
	}

	// This should not happen
	assert(false);
}

double UniverseGenerator::generate_planet_distance(const uint64_t &pl_id,
		const uint8_t planet_type, const SolarSystem *ss)
{
	assert(pg_defs[planet_type].distance_scaling_factor_min * 10 * 1000.0f * 1000.0f
		   < ss->radius); // This should not happen

	std::mt19937 rndgen(s_seed + pl_id + 1024 * pl_id);
	// 10 Billion to max_distance km
	std::uniform_real_distribution<double> rnd(10 * 1000.0f * 1000.0f *
		pg_defs[planet_type].distance_scaling_factor_min, ss->radius);
	return rnd(rndgen);
}

double UniverseGenerator::generate_planet_radius(const uint64_t &pl_id,
		const uint8_t planet_type)
{
	std::mt19937 rndgen(s_seed + pl_id + 512 * pl_id);
	// 200 km to 100k km
	std::uniform_real_distribution<double> rnd(200.0f * pg_defs[planet_type].radius_scaling_factor_min,
			100 * 1000.0f * pg_defs[planet_type].radius_scaling_factor_max);
	return rnd(rndgen);
}

uint64_t UniverseGenerator::generate_seed()
{
	std::mt19937 rndgen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<uint64_t> rnd(0, UINT64_MAX);
	return rnd(rndgen);
}

float UniverseGenerator::generate_galaxypos_radius(const float min_radius,
		const float max_radius)
{
	std::uniform_real_distribution<float> rnd(min_radius, max_radius);
	return rnd(m_random_generator_galaxy_positions);
}

float UniverseGenerator::generate_galaxypos_gauss_random(const float param)
{
	std::normal_distribution<float> rnd(param);
	return rnd(m_random_generator_galaxy_positions);
}

uint32_t UniverseGenerator::generate_galaxypos_urange(const uint32_t param)
{
	std::uniform_int_distribution<uint32_t> rnd(0, param);
	return rnd(m_random_generator_galaxy_positions);
}

}
}

