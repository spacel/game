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

#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Sprite.h>

#include <common/engine/server.h>
#include "genericmenu.h"

using namespace Urho3D;

namespace spacel {

class LoadingScreen: public GenericMenu {
URHO3D_OBJECT(LoadingScreen, GenericMenu);

public:
	LoadingScreen(Context *context, ClientSettings *config, engine::Server *server);
	~LoadingScreen() {};
	void Start();

private:
	void ShowBackground();
	void LoadingBar();
	void ShowTips();
	void SetLoadingPercentText(const float &range);
	void HandleUpdate(StringHash, VariantMap &eventData);

	SharedPtr<UIElement> m_ui_elem;
	SharedPtr<Sprite> m_loading_background;
	SharedPtr<Text> m_tips_text;
	SharedPtr<Slider> m_loading_bar;
	SharedPtr<Text> m_loading_text;
	SharedPtr<Text> m_loading_percent_text;
	// Server
	engine::Server *m_server;
	engine::ServerLoadingStep server_loading_step;
};
}
