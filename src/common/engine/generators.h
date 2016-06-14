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
#include <cassert>
#include <random>

namespace spacel {
namespace engine {

class UniverseGenerator
{
public:
	UniverseGenerator() {}
	inline static UniverseGenerator *instance()
	{
		if (!UniverseGenerator::s_univgen) {
			// Don't permit to use non setted seed
			assert(s_seed != 0);
			UniverseGenerator::s_univgen = new UniverseGenerator();
		}

		return UniverseGenerator::s_univgen;
	}

	inline static void SetSeed(uint64_t seed) { s_seed = seed; }

	std::string generate_world_name();
	uint8_t generate_solarsystem_type(const uint64_t &ss_id);
	double generate_solarsystem_double(const uint64_t &ss_id);
	static uint64_t generate_seed();
private:
	void InitRandomGeneratorIfNot();

	bool m_random_generator_inited = false;
	std::mt19937 m_random_generator;
	std::uniform_int_distribution<uint8_t> m_vowel_generator;
	std::uniform_int_distribution<uint16_t> m_name_generators[4];
	static UniverseGenerator *s_univgen;
	static uint64_t s_seed;
};
}
}

