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
#include "generators.h"

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

}
}

