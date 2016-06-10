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

#include <common/engine/server.h>
#include "genericmenu.h"
#include "settings.h"

using namespace Urho3D;

namespace spacel {

class Game: public GenericMenu {
URHO3D_OBJECT(Game, GenericMenu);

public:
	Game(Context *context, ClientSettings *config, engine::Server *server);
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
	void MoveCamera(float timeStep);

	SharedPtr<Scene> m_scene;
	SharedPtr<Node> m_camera_node;
	SharedPtr<Node> m_terrain_node;
	SharedPtr<Terrain> m_terrain;
	SharedPtr<Node> m_stone_node;

	SharedPtr<Node> m_reflection_camera_node;
	/// Water body scene node.
	SharedPtr<Node> m_water_node;
	/// Reflection plane representing the water surface.
	Plane m_water_plane;
	/// Clipping plane for reflection rendering. Slightly biased downward from the reflection plane to avoid artifacts.
	Plane m_water_clip_plane;

	float m_yaw;
	float m_pitch;
};
}
