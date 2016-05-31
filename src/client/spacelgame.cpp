#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>

#include <common/porting.h>
#include <common/engine/space.h>
#include "mainmenu.h"
#include "spacelgame.h"

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(spacel::SpacelGame)

namespace spacel {

engine::Universe* engine::Universe::s_universe = nullptr;

void SpacelGame::Setup()
{
	m_config = new ClientSettings(context_);

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
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "logs") + GetTypeName() + ".log";
	GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
	//if (!engineParameters_.Contains("ResourcePrefixPaths"))
			//engineParameters_["ResourcePrefixPaths"] = ";./bin";
}

void SpacelGame::Start()
{
	// Called after engine initialization. Setup application & subscribe to events here
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(SpacelGame, HandleKeyDown));

	MainMenu *mainMenu = new MainMenu(context_);
	UI *ui = GetSubsystem<UI>();
	ui->GetRoot()->AddChild(mainMenu);
	mainMenu->Start();
}

void SpacelGame::Stop()
{
	m_config->save(fs::path_config + DIR_DELIM + "client.json");
	delete m_config;
	engine_->DumpResources(true);
}

void SpacelGame::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
	using namespace KeyDown;
	// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
	int key = eventData[P_KEY].GetInt();
	if (key == KEY_ESC)
		engine_->Exit();
	/*if (key == KEY_S)
		if (!scene_->GetChild("Music"))
			MusicMenu(true);
		else
			MusicMenu(false);*/
}

void SpacelGame::HandleClosePressed(StringHash eventType,VariantMap &eventData)
{
	engine_->Exit();
}
}
