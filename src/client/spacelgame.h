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
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "settings.h"
#include "uievents.h"

using namespace Urho3D;

namespace spacel {

namespace engine {
	class Server;
}

enum GlobalUIId
{
	GLOBALUI_MAINMENU,
	GLOBALUI_LOADINGSCREEN,
	GLOBALUI_GAME,
};

class SpacelGame : public Application
{
	URHO3D_OBJECT(SpacelGame, Application);

public:
	SpacelGame(Context *context): Application(context) {}
	virtual void Setup();
	virtual void Start();
	virtual void Stop();

	// Urho handlers
	void HandleBeginFrame(StringHash, VariantMap &eventData);

	// UI event handlers
	void HandleCharacterList(UIEventPtr event);

	void ChangeGameGlobalUI(const GlobalUIId ui_id, void *param = nullptr);
	void QueueUiEvent(UIEventPtr e) { m_ui_event_queue.push_back(e); }

	void QueueClientUIEvent(ClientUIEvent *event);

private:
	void InitLocales();

	ClientSettings *m_config = nullptr;
	UIEventQueue m_ui_event_queue;
};

}
