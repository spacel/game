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

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>

#include <common/porting.h>
#include <common/engine/space.h>
#include <project_defines.h>
#include <common/engine/server.h>
#include <common/engine/objectmanager.h>
#include <common/engine/generators.h>
#include <Urho3D/Core/CoreEvents.h>
#include <iostream>

#include "game.h"
#include "loadingscreen.h"
#include "mainmenu.h"
#include "spacelgame.h"

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(spacel::SpacelGame)

namespace spacel {

// Init singletons
engine::Universe *engine::Universe::s_universe = nullptr;
engine::ObjectMgr *engine::ObjectMgr::s_objmgr = nullptr;
engine::UniverseGenerator *engine::UniverseGenerator::s_univgen = nullptr;
uint64_t engine::UniverseGenerator::s_seed = 0;
Client *Client::s_client = nullptr;

void SpacelGame::Setup()
{
	m_config = new ClientSettings();
	m_config->load(GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "config") +
		"client.json");

	// Called before engine initialization. engineParameters_ member variable can be modified here
	engineParameters_["WindowTitle"] = PROJECT_LABEL;
	engineParameters_["FullScreen"] = m_config->getBool(BSETTING_FULLSCREEN);
	engineParameters_["WindowWidth"] = m_config->getU32(U32SETTING_WINDOW_WIDTH);
	engineParameters_["WindowHeight"] = m_config->getU32(U32SETTING_WINDOW_HEIGHT);
	engineParameters_["WindowResizable"] = m_config->getBool(BSETTING_RESIZABLE_WINDOW);
	engineParameters_["Sound"] = m_config->getBool(BSETTING_ENABLE_SOUND);
	engineParameters_["TripleBuffer"] = m_config->getBool(BSETTING_TRIPLEBUFFER);
	engineParameters_["LogLevel"] = LOG_DEBUG;
	engineParameters_["LogQuiet"] = m_config->getBool(BSETTING_LOGQUIET);
	engineParameters_["Headless"] = false;
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "logs") +
		"SpacelGame_" + Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".log";
	InitLocales();
	engine::ui::ModalWindow::RegisterObject(context_);
}

void SpacelGame::Start()
{
	SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(SpacelGame, HandleBeginFrame));
	ChangeGameGlobalUI(GLOBALUI_MAINMENU);
}

void SpacelGame::Stop()
{
	m_config->save(GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "config") +
		"client.json");
	delete m_config;
	engine_->DumpResources(true);
}

inline void SpacelGame::InitLocales()
{
	GetSubsystem<Localization>()->LoadJSONFile(
			GetSubsystem<FileSystem>()->GetProgramDir() + "Data/locales/strings.json");
}

void SpacelGame::ChangeGameGlobalUI(const GlobalUIId ui_id, void *param)
{
	GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();

	switch (ui_id) {
		case GLOBALUI_MAINMENU: {
			MainMenu *main_menu = new MainMenu(context_, m_config, this);
			GetSubsystem<UI>()->GetRoot()->AddChild(main_menu);
			main_menu->Start();
			break;
		}
		case GLOBALUI_LOADINGSCREEN: {
			assert(param != nullptr);
			const String gamedatapath = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/game/";
			const String path_universe =
				GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "universe");
			const String universe_name((const char *) param);

			Client::instance()->SetSinglePlayerMode(true);
			Client::instance()->SetGameDataPath(std::string(gamedatapath.CString()));
			Client::instance()->SetDataPath(std::string(path_universe.CString()));
			Client::instance()->SetUniverseName(universe_name.CString());
			Client::instance()->SetUIEventHandler(this);
			Client::instance()->Run();

			LoadingScreen *loading_screen = new LoadingScreen(context_, m_config, this);
			GetSubsystem<UI>()->GetRoot()->AddChild(loading_screen);
			loading_screen->Start();
			break;
		}
		case GLOBALUI_GAME: {
			Game *game = new Game(context_, m_config, this);
			GetSubsystem<UI>()->GetRoot()->AddChild(game);
			game->Start();
			break;
		}
		default: assert(false);
	}
}

void SpacelGame::QueueClientUIEvent(ClientUIEvent *event)
{
	assert(Client::instance()->GetLoadingStep() >= CLIENTLOADINGSTEP_CONNECTED);
	Client::instance()->QueueClientUiEvent(ClientUIEventPtr(event));
}


/**
 * This function handle UI events pushed from Client
 * @param eventData
 */
void SpacelGame::HandleBeginFrame(StringHash, VariantMap &eventData)
{
	{
		static const uint8_t MAX_UI_EVENT_ITERATIONS = 5;
		uint8_t i = 0;
		while (!m_ui_event_queue.empty() && i < MAX_UI_EVENT_ITERATIONS) {
			i++;
			UIEventPtr event = m_ui_event_queue.pop_front();

			// Invalid event, ignore it
			if (event->id >= UI_EVENT_MAX) {
				URHO3D_LOGWARNINGF("Invalid UI event id %d received, ignoring", event->id);
				continue;
			}

			const UIEventHandler &eventHandle = UIEventHandlerTable[event->id];
			(this->*eventHandle.handler)(event);
		}
	}
}

void SpacelGame::HandleCharacterList(UIEventPtr event)
{
	UIEvent_CharacterList *r_event = dynamic_cast<UIEvent_CharacterList *>(event.get());
	assert(r_event);

	for (const auto &player: r_event->player_list) {
		// @TODO handle this properly in GUI or send this to proper UI component
	}
}
}
