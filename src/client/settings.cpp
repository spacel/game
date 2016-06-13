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

static SettingDefault<bool> s_bsettings[BSETTINGS_MAX] = {
		{ "enable_sound", true },
		{ "enable_music", true },
		{ "fullscreen", false },
		{ "resizable_window", true },
		{ "triple_buffer", true },
		{ "log_quiet", false },
};

static SettingDefault<uint32_t> s_u32settings[U32SETTINGS_MAX] = {
		{ "window_height", 720 },
		{ "window_width", 1280 },
};

static SettingDefault<float> s_floatsettings[FLOATSETTINGS_MAX] = {
		{ "sound_ui_gain", 0.75f },
		{ "sound_master_gain", 0.75f },
		{ "sound_effect_gain", 0.75f },
		{ "sound_music_gain", 0.75f },
		{ "sound_ambient_gain", 0.75f },
		{ "sound_voice_gain", 0.75f },
		{ "timer_error_bubble", 15000.0f },
};

void ClientSettings::init()
{
	for (uint8_t i = 0; i < BSETTINGS_MAX; ++i) {
		registerBool(i, s_bsettings[i].default_value, s_bsettings[i].name);
	}

	for (uint8_t i = 0; i < U32SETTINGS_MAX; ++i) {
		registerU32(i, s_u32settings[i].default_value, s_u32settings[i].name);
	}

	for (uint8_t i = 0; i < FLOATSETTINGS_MAX; ++i) {
		registerFloat(i, s_floatsettings[i].default_value, s_floatsettings[i].name);
	}
}

}
