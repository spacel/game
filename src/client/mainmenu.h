#ifndef MAINMENU_H
#define MAINMENU_H

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>

#include "spacelgame.h"

using namespace Urho3D;

namespace spacel {

class MainMenu : public UIElement {
URHO3D_OBJECT(MainMenu, UIElement);

public:
	MainMenu(Context *context);
	~MainMenu() { };
	void Start();

protected:
	SharedPtr<Engine> engine_;
	SharedPtr<Log> log_;
	SharedPtr<Sprite> backgroundSprite_;

private:
	void HandleClosePressed(StringHash eventType, VariantMap &eventData);
	void HandleSettingsPressed(StringHash eventType, VariantMap &eventData);
	void HandleGraphicsPressed(StringHash eventType, VariantMap &eventData);
	void HandleSoundsPressed(StringHash eventType, VariantMap &eventData);
	void HandleExitPressed(StringHash eventType, VariantMap &eventData);
	void HandleUpdate(StringHash eventType, VariantMap &eventData);
	void Music(const bool active);
	void Background();
	void Title();
	void Menu();

	ResourceCache *cache;
	UIElement *uiElem;
	static const uint space_button = 20;
	Window *m_window_menu;
	Text *m_title;
};
}
#endif // MAINMENU_H
