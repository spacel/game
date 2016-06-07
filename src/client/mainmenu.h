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
	MainMenu(Context *context, ClientSettings *config);
	~MainMenu();
	void Start();

private:
	// Handlers
	void HandleClosePressed(StringHash eventType, VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);
	void HandleSingleplayerPressed(StringHash eventType, VariantMap &eventData);
	void HandleSettingsPressed(StringHash eventType, VariantMap &eventData);
	void HandleGraphicsPressed(StringHash eventType, VariantMap &eventData);
	void HandleSoundsPressed(StringHash eventType, VariantMap &eventData);

	void HandleUpdate(StringHash eventType, VariantMap &eventData);
	void HandleMusicPressed(StringHash eventType, VariantMap &eventData);
	void Background();
	void Title();
	void HandleMasterMenu(StringHash, VariantMap &);

	// Helpers
	Button *CreateMainMenuButton(const String &label,
			const String &button_style = "Button",
			const String &label_style = "TextButton");
	void SetTitle(const String &t);
	// Attributes
	uint8_t m_menu_id = 0;
	SharedPtr<UIElement> m_ui_elem;
	static const uint s_mainmenu_button_space = 20;
	SharedPtr<Window> m_window_menu;
	SharedPtr<Text> m_title;
	SharedPtr<Button> m_music_button;
	bool m_enable_menu_music;
	SharedPtr<Sprite> m_menu_background;
};

}
