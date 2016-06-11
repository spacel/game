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
#include "namegenerator.h"

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
		"ad", "af", "am", "ass", "ast", "atr", "al", "aj",
		"ed", "ef", "em", "ess", "est", "etr", "el", "ej",
		"id", "if", "im", "iss", "ist", "itr", "il", "ij",
		"od", "of", "om", "oss", "ost", "otr", "ol", "oj",
		"ud", "uf", "um", "uss", "ust", "utr", "ul", "uj",
		"yd", "yf", "ym", "yss", "yst", "ytr", "yl", "yj"
};

std::string generate_world_name()
{
	std::string res = name_prefixes[rand() % (ARRLEN(name_prefixes) - 1)];
	uint8_t stem_number = rand() % 2;

	for (uint8_t i = 0; i < stem_number; i++) {
		res += name_stem[rand() % (ARRLEN(name_stem) - 1)];
	}

	res += name_suffixes[rand() % (ARRLEN(name_suffixes) - 1)];

	return res;
}
}
}

