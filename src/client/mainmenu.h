/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
 * Copyright 2016, Jeremy Lomoro <jeremy.lomoro@tuxsrv.fr>
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

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>

#include "genericmenu.h"

using namespace Urho3D;

namespace spacel {

class MainMenu: public GenericMenu  {
URHO3D_OBJECT(MainMenu, GenericMenu);

public:
	MainMenu(Context *context);
	~MainMenu() {}
	void Start();
	inline const bool isMain() const { return m_main; };

protected:
	SharedPtr<Engine> engine_;
	SharedPtr<Log> log_;
	SharedPtr<Sprite> backgroundSprite_;

private:
	void HandleClosePressed(StringHash eventType, VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);
	void HandleSettingsPressed(StringHash eventType, VariantMap &eventData);
	void HandleGraphicsPressed(StringHash eventType, VariantMap &eventData);
	void HandleSoundsPressed(StringHash eventType, VariantMap &eventData);
	void HandleExitPressed(StringHash eventType, VariantMap &eventData);
	void HandleUpdate(StringHash eventType, VariantMap &eventData);
	void HandleMusicPressed(StringHash eventType, VariantMap &eventData);
	void Music(const bool active);
	void Background();
	void Title();
	void Menu();

	ResourceCache *m_cache;
	UIElement *m_ui_elem;
	Localization *m_l10n ;
	static const uint s_space_button = 20;
	Window *m_window_menu;
	Text *m_title;
	bool m_main;
	bool m_music_active;
};

}
