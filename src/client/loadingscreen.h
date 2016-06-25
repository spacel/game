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

#include <Urho3D/UI/Sprite.h>
#include "ui/ProgressBar.h"

#include <common/engine/server.h>
#include "genericmenu.h"
#include "spacelgame.h"
#include "client.h"

using namespace Urho3D;

namespace spacel {

class LoadingScreen: public GenericMenu {
URHO3D_OBJECT(LoadingScreen, GenericMenu);

public:
	LoadingScreen(Context *context, ClientSettings *config, SpacelGame *main);
	~LoadingScreen() {};
	void Start();

private:
	void ShowBackground();
	void ShowProgressBar();
	void ShowTips();
	void HandleUpdate(StringHash, VariantMap &eventData);
	void LaunchGame();

	SpacelGame *m_main;
	SharedPtr<UIElement> m_ui_elem;
	SharedPtr<Sprite> m_loading_background;
	SharedPtr<engine::ui::ProgressBar> m_progress_bar;
	SharedPtr<Text> m_loading_text;

	ClientLoadingStep m_last_loading_step = CLIENTLOADINGSTEP_NOT_STARTED;
};
}
