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
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>

#include <common/porting.h>
#include <common/engine/space.h>
#include "spacelgame.h"

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(spacel::SpacelGame)

namespace spacel {

engine::Universe *engine::Universe::s_universe = nullptr;

void SpacelGame::Setup()
{
	m_config = new ClientSettings(context_);
	m_path_config = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "configs") + "client.json";
	m_config->load(fs::path_config + DIR_DELIM + "client.json");

	// Called before engine initialization. engineParameters_ member variable can be modified here
	engineParameters_["WindowTitle"] = "Spacel Game";
	engineParameters_["FullScreen"] = m_config->getBool(BSETTING_FULLSCREEN);;
	engineParameters_["WindowWidth"] = m_config->getU32(U32SETTING_WINDOW_WIDTH);
	engineParameters_["WindowHeight"] = m_config->getU32(U32SETTING_WINDOW_HEIGHT);
	engineParameters_["WindowResizable"] = m_config->getBool(BSETTING_RESIZABLE_WINDOW);
	engineParameters_["Sound"] = m_config->getBool(BSETTING_ENABLE_SOUND);
	engineParameters_["TripleBuffer"] = m_config->getBool(BSETTING_TRIPLEBUFFER);
	engineParameters_["LogLevel"] = LOG_DEBUG;
	engineParameters_["LogQuiet"] = m_config->getBool(BSETTING_LOGQUIET);
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "logs") + "SpacelGame.log";
	GetSubsystem<Input>()->SetMouseVisible(true);
	//if (!engineParameters_.Contains("ResourcePrefixPaths"))
				//engineParameters_["ResourcePrefixPaths"] = ";./bin";
	InitLocalizationSystem();

}

void SpacelGame::Start()
{
	// Called after engine initialization. Setup application & subscribe to events here
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(SpacelGame, HandleKeyDown));
	Localization *l10n = GetSubsystem<Localization>();

	m_mainMenu = new MainMenu(context_);
	UI *ui = GetSubsystem<UI>();
	ui->GetRoot()->AddChild(m_mainMenu);
	m_mainMenu->Start();
}

void SpacelGame::Stop()
{
	m_config->save(m_path_config.CString());
	delete m_config;
	engine_->DumpResources(true);
}

void SpacelGame::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
	using namespace KeyDown;
	UI *ui = GetSubsystem<UI>();
	// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
	int key = eventData[P_KEY].GetInt();
	if (key == KEY_ESC)
		if (m_mainMenu->isMain())
			engine_->Exit();
}

void SpacelGame::HandleClosePressed(StringHash eventType, VariantMap &eventData)
{
	engine_->Exit();
}

void SpacelGame::InitLocalizationSystem()
{
	m_cache = GetSubsystem<ResourceCache>();
	Localization *l10n = GetSubsystem<Localization>();
	l10n->LoadJSONFile("Data/locales/strings.json");
}
}
