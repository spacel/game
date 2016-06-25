/*
 * This file is part of Spacel game.
 *
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
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Window.h>

#include "genericmenu.h"
#include "settings.h"
#include "spacelgame.h"

using namespace Urho3D;

namespace spacel {

class Game: public GenericMenu {
URHO3D_OBJECT(Game, GenericMenu);

public:
	Game(Context *context, ClientSettings *config, SpacelGame *main);
	~Game();
	void Start();

private:
	void CreateConsoleAndDebugHud();
	void CreateSkybox();
	void CreateCamera();
	void GenerateTerrain();
	void SetupViewport();
	void SubscribeToEvents();
	void HandleUpdate(StringHash eventType, VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);
	void HandlePostUpdate(StringHash eventType, VariantMap &eventData);
	void HandleMouseClicked(StringHash eventType, VariantMap &eventData);
	void HandleMenu(StringHash eventType, VariantMap &eventData);
	void HandleResume(StringHash eventType, VariantMap &eventData);
	void HandleBackMainMenu(StringHash eventType, VariantMap &eventData);
	void HandleExitGame(StringHash eventType, VariantMap &eventData);
	void CreateMenu();
	void MoveCamera(float timeStep);

	//Helper
	Button *CreateMenuButton(const String &label,
			const String &button_style = "Button",
			const String &label_style = "TextButton");

	SpacelGame *m_main;
	SharedPtr<Scene> m_scene;
	SharedPtr<Node> m_camera_node;
	SharedPtr<Node> m_terrain_node;
	SharedPtr<Terrain> m_terrain;
	SharedPtr<Node> m_stone_node;
	SharedPtr<UIElement> m_ui_elem;
	SharedPtr<Window> m_window_menu;

	SharedPtr<Node> m_reflection_camera_node;
	/// Water body scene node.
	SharedPtr<Node> m_water_node;
	/// Reflection plane representing the water surface.
	Plane m_water_plane;
	/// Clipping plane for reflection rendering. Slightly biased downward from the reflection plane to avoid artifacts.
	Plane m_water_clip_plane;

	float m_yaw;
	float m_pitch;
	bool m_move_camera = true;
	bool m_gamemenu_created = false;
};
}
