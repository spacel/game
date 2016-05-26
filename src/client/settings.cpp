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
#include "settings.h"

namespace spacel {

struct BoolSettingDefault {
	std::string name;
	bool default_value;
};

struct U32SettingDefault {
	std::string name;
	uint32_t default_value;
};

static BoolSettingDefault s_bsettings[BSETTINGS_MAX] = {
		{ "enable_sound", true },
		{ "fullscreen", false },
		{ "resizable_window", true },
};

static U32SettingDefault s_u32settings[U32SETTINGS_MAX] = {
		{ "window_height", 720 },
		{ "window_width", 1280 },
};

void ClientSettings::init()
{
	for (uint8_t i = 0; i < BSETTINGS_MAX; ++i) {
		registerBool(i, s_bsettings[i].default_value, s_bsettings[i].name);
	}

	for (uint8_t i = 0; i < U32SETTINGS_MAX; ++i) {
		registerU32(i, s_u32settings[i].default_value, s_u32settings[i].name);
	}
}

}
