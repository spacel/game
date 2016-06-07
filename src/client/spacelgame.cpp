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
#include "spacelgame.h"

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(spacel::SpacelGame)

namespace spacel {

engine::Universe *engine::Universe::s_universe = nullptr;

void SpacelGame::Setup()
{
	m_config = new ClientSettings(context_);
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
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "logs") +
		"SpacelGame.log";
	GetSubsystem<Input>()->SetMouseVisible(true);
	InitLocales();
}

void SpacelGame::Start()
{
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

void SpacelGame::ChangeGameGlobalUI(const GlobalUIId ui_id)
{
	GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();

	switch (ui_id) {
		case GLOBALUI_MAINMENU: {
				MainMenu *main_menu = new MainMenu(context_, m_config, this);
				GetSubsystem<UI>()->GetRoot()->AddChild(main_menu);
				main_menu->Start();
			}
			break;

		default: assert(false);
	}
}
}
