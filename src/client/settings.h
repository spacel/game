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

#include <common/config.h>

namespace spacel {

enum BoolSetting {
	BSETTING_ENABLE_SOUND = 0,
	BSETTING_ENABLE_MUSIC,
	BSETTING_FULLSCREEN,
	BSETTING_RESIZABLE_WINDOW,
	BSETTING_TRIPLEBUFFER,
	BSETTING_LOGQUIET,
	BSETTINGS_MAX,
};

enum U32Setting {
	U32SETTING_WINDOW_HEIGHT = 0,
	U32SETTING_WINDOW_WIDTH,
	U32SETTINGS_MAX,
};

enum FloatSetting {
	FLOATSETTINGS_SOUND_UI_GAIN = 0,
	FLOATSETTINGS_SOUND_MASTER_GAIN,
	FLOATSETTINGS_SOUND_EFFECT_GAIN,
	FLOATSETTINGS_SOUND_MUSIC_GAIN,
	FLOATSETTINGS_SOUND_AMBIENT_GAIN,
	FLOATSETTINGS_SOUND_VOICE_GAIN,
	FLOATSETTINGS_TIMER_ERROR_BUBBLE,
	FLOATSETTINGS_MAX,
};

class ClientSettings: public Config
{
public:
	ClientSettings(): Config(BSETTINGS_MAX, U32SETTINGS_MAX, FLOATSETTINGS_MAX)
	{
		init();
	}

private:
	virtual void init();
};

}
