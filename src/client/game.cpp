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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderSurface.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include "client.h"
#include "mainmenu.h"

#include "genericmenu.h"
#include "game.h"

using namespace Urho3D;

namespace spacel {

#define MENU_BUTTON_SPACE 20

Game::Game(Context *context, ClientSettings *config, SpacelGame *main):
	GenericMenu(context, config),
	m_main(main)
{
	m_scene = new Scene(context_);
	m_terrain_node = new Node(context_);
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/MenuGameStyle.xml"));
}

Game::~Game()
{
	m_window_menu->RemoveAllChildren();
	m_window_menu->Remove();
	Client::instance()->Stop();
	Client::deinstance();
}

void Game::Start()
{
	// Settings
	GetSubsystem<Input>()->SetMouseVisible(false);
	PlayMusic(m_config->getBool(BSETTING_ENABLE_MUSIC));
	CreateConsoleAndDebugHud();

	GenerateTerrain();
	SetupViewport();
	SubscribeToEvents();
}

void Game::CreateConsoleAndDebugHud()
{
	// Get default style
	XMLFile *xmlFile = m_cache->GetResource<XMLFile>("UI/ConsoleStyle.xml");

	// Create console
	Console *console = m_engine->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);

	// Create debug HUD.
	DebugHud *debugHud = m_engine->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
}

void Game::CreateSkybox()
{
	// Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
	// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
	// generate the necessary 3D texture coordinates for cube mapping
	Node *skyNode = m_scene->CreateChild("Sky");
	skyNode->SetScale(500.0f); // The scale actually does not matter
	Skybox *skybox = skyNode->CreateComponent<Skybox>();
	skybox->SetModel(m_cache->GetResource<Model>("Models/Box.mdl"));
	skybox->SetMaterial(m_cache->GetResource<Material>("Materials/Skybox.xml"));
}

void Game::CreateCamera()
{
	 // Create the camera. Set far clip to match the fog. Note: now we actually create the camera node outside
	// the scene, because we want it to be unaffected by scene load / save
	m_camera_node = new Node(context_);
	Camera *camera = m_camera_node->CreateComponent<Camera>();
	camera->SetFarClip(750.0f);

	// Set an initial position for the camera scene node above the ground
	m_camera_node->SetPosition(Vector3(0.0f, 7.0f, -20.0f));
}

void Game::GenerateTerrain()
{
	m_scene->CreateComponent<Octree>();

	// Create a Zone component for ambient lighting & fog control
	Node *zoneNode = m_scene->CreateChild("Zone");
	Zone *zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(1.0f, 1.0f, 1.0f));
	zone->SetFogStart(500.0f);
	zone->SetFogEnd(750.0f);

	// Create a directional light to the world. Enable cascaded shadows on it
	Node *lightNode = m_scene->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
	Light *light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
	light->SetSpecularIntensity(0.5f);
	// Apply slightly overbright lighting to match the skybox
	light->SetColor(Color(1.2f, 1.2f, 1.2f));

	CreateSkybox();

	 // Create heightmap terrain
	m_terrain_node = m_scene->CreateChild("Terrain");
	m_terrain_node->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	m_terrain = m_terrain_node->CreateComponent<Terrain>();
	m_terrain->SetPatchSize(64);
	m_terrain->SetSpacing(Vector3(2.0f, 0.5f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
	m_terrain->SetSmoothing(true);
	m_terrain->SetHeightMap(m_cache->GetResource<Image>("Textures/HeightMap.png"));
	m_terrain->SetMaterial(m_cache->GetResource<Material>("Materials/Terrain.xml"));
	// The terrain consists of large triangles, which fits well for occlusion rendering, as a hill can occlude all
	// terrain patches and other objects behind it
	m_terrain->SetOccluder(true);

	// Create 1000 boxes in the terrain. Always face outward along the terrain normal
	unsigned NUM_OBJECTS = 1000;
	for (unsigned i = 0; i < NUM_OBJECTS; ++i)
	{
		Node *objectNode = m_scene->CreateChild("Box");
		Vector3 position(Random(2000.0f) - 1000.0f, 0.0f, Random(2000.0f) - 1000.0f);
		position.y_ = m_terrain->GetHeight(position) + 2.25f;
		objectNode->SetPosition(position);
		// Create a rotation quaternion from up vector to terrain normal
		objectNode->SetRotation(Quaternion(Vector3(0.0f, 1.0f, 0.0f), m_terrain->GetNormal(position)));
		objectNode->SetScale(5.0f);
		StaticModel *object = objectNode->CreateComponent<StaticModel>();
		object->SetModel(m_cache->GetResource<Model>("Models/Box.mdl"));
		object->SetMaterial(m_cache->GetResource<Material>("Materials/Stone.xml"));
		object->SetCastShadows(true);
	}

	// Create a water plane object that is as large as the terrain
	m_water_node = m_scene->CreateChild("Water");
	m_water_node->SetScale(Vector3(2048.0f, 1.0f, 2048.0f));
	m_water_node->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
	StaticModel *water = m_water_node->CreateComponent<StaticModel>();
	water->SetModel(m_cache->GetResource<Model>("Models/Plane.mdl"));
	water->SetMaterial(m_cache->GetResource<Material>("Materials/Water.xml"));
	// Set a different viewmask on the water plane to be able to hide it from the reflection camera
	water->SetViewMask(0x80000000);

	CreateCamera();
}

void Game::SetupViewport()
{
	Graphics *graphics = GetSubsystem<Graphics>();
	Renderer *renderer = GetSubsystem<Renderer>();

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
	SharedPtr<Viewport> viewport(new Viewport(context_, m_scene, m_camera_node->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);

	// Create a mathematical plane to represent the water in calculations
	m_water_plane = Plane(m_water_node->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), m_water_node->GetWorldPosition());
	// Create a downward biased plane for reflection view clipping. Biasing is necessary to avoid too aggressive clipping
	m_water_clip_plane = Plane(m_water_node->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), m_water_node->GetWorldPosition() -
		Vector3(0.0f, 0.1f, 0.0f));

	// Create camera for water reflection
	// It will have the same farclip and position as the main viewport camera, but uses a reflection plane to modify
	// its position when rendering
	m_reflection_camera_node = m_camera_node->CreateChild();
	Camera *reflectionCamera = m_reflection_camera_node->CreateComponent<Camera>();
	reflectionCamera->SetFarClip(750.0);
	reflectionCamera->SetViewMask(0x7fffffff); // Hide objects with only bit 31 in the viewmask (the water plane)
	reflectionCamera->SetAutoAspectRatio(false);
	reflectionCamera->SetUseReflection(true);
	reflectionCamera->SetReflectionPlane(m_water_plane);
	reflectionCamera->SetUseClipping(true); // Enable clipping of geometry behind water plane
	reflectionCamera->SetClipPlane(m_water_clip_plane);
	// The water reflection texture is rectangular. Set reflection camera aspect ratio to match
	reflectionCamera->SetAspectRatio((float)graphics->GetWidth() / (float)graphics->GetHeight());
	// View override flags could be used to optimize reflection rendering. For example disable shadows
	//reflectionCamera->SetViewOverrideFlags(VO_DISABLE_SHADOWS);

	// Create a texture and setup viewport for water reflection. Assign the reflection texture to the diffuse
	// texture unit of the water material
	int texSize = 1024;
	SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
	renderTexture->SetSize(texSize, texSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture->SetFilterMode(FILTER_BILINEAR);
	RenderSurface *surface = renderTexture->GetRenderSurface();
	SharedPtr<Viewport> rttViewport(new Viewport(context_, m_scene, reflectionCamera));
	surface->SetViewport(0, rttViewport);
	Material *waterMat = m_cache->GetResource<Material>("Materials/Water.xml");
	waterMat->SetTexture(TU_DIFFUSE, renderTexture);
}

void Game::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, HandleUpdate));
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Game, HandleKeyDown));
}

void Game::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
	using namespace Update;
	float timeStep = eventData[P_TIMESTEP].GetFloat();
	MoveCamera(timeStep);
}

void Game::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
	using namespace KeyDown;
	int key = eventData[P_KEY].GetInt();

	switch (key) {
		case KEY_ESCAPE: {
			Console *console = GetSubsystem<Console>();
			if (console->IsVisible()) {
				console->SetVisible(false);
			}
			else if (!m_gamemenu_created || !m_window_menu->IsVisible()) {
				// @TODO show global main menu if no other GUI menu opened
				//m_engine->Exit();
				VariantMap v;
				HandleMenu(StringHash(), v);
			}
			else {
				//Quit menu
				VariantMap v;
				HandleResume(StringHash(), v);
			}
			break;
		}
		case KEY_F9:
			GetSubsystem<Console>()->Toggle();
			break;
		case KEY_F10: {
			DebugHud *debugHud = GetSubsystem<DebugHud>();
			if (debugHud->GetMode() == 0 ||
				debugHud->GetMode() ==  DEBUGHUD_SHOW_MEMORY) {
				debugHud->SetMode(DEBUGHUD_SHOW_ALL);
			}
			else {
				debugHud->SetMode(DEBUGHUD_SHOW_NONE);
			}
			break;
		}
		case KEY_F11: {
				DebugHud *debugHud = GetSubsystem<DebugHud>();
				if (debugHud->GetMode() == 0 || debugHud->GetMode() == DEBUGHUD_SHOW_ALL) {
					debugHud->SetMode(DEBUGHUD_SHOW_MEMORY);
				}
				else {
					debugHud->SetMode(DEBUGHUD_SHOW_NONE);
				}
				break;
		}
		case KEY_F12:
			TakeScreenshot();
			break;
		default:
			break;
	}
}

void Game::MoveCamera(float timeStep)
{
	// Do not move if the UI has a focused element (the console)
	if (!m_move_camera || GetSubsystem<UI>()->GetFocusElement())
		return;

	Input *input = GetSubsystem<Input>();

	// Movement speed as world units per second
	static const float MOVE_SPEED = 200.0f;
	// Mouse sensitivity as degrees per pixel
	static const float MOUSE_SENSITIVITY = 0.1f;

	// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
	IntVector2 mouseMove = input->GetMouseMove();
	m_yaw += MOUSE_SENSITIVITY * mouseMove.x_;
	m_pitch += MOUSE_SENSITIVITY * mouseMove.y_;
	m_pitch = Clamp(m_pitch, -90.0f, 90.0f);

	// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
	m_camera_node->SetRotation(Quaternion(m_pitch, m_yaw, 0.0f));

	// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
	if (input->GetKeyDown(KEY_Z))
		m_camera_node->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_S))
		m_camera_node->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_Q))
		m_camera_node->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_D))
		m_camera_node->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_SPACE))
		m_camera_node->Translate(Vector3::UP * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_SHIFT))
		m_camera_node->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);

	// In case resolution has changed, adjust the reflection camera aspect ratio
	Graphics *graphics = GetSubsystem<Graphics>();
	Camera *reflectionCamera = m_reflection_camera_node->GetComponent<Camera>();
	reflectionCamera->SetAspectRatio((float)graphics->GetWidth() / (float)graphics->GetHeight());
}

void Game::CreateMenu()
{
	if (m_gamemenu_created) {
		return;
	}
	m_gamemenu_created = true;
	m_window_menu = new Window(context_);
	m_window_menu->SetStyle("GameMenu");
	m_window_menu->SetModal(true);
	m_ui_elem->AddChild(m_window_menu);

	Text *text_menu = static_cast<Text *>(m_window_menu->GetChild("title", false));
	Button *resume = static_cast<Button *>(m_window_menu->GetChild("resume_button", true));
	Text *resume_text = static_cast<Text *>(resume->GetChild("resume_button_text", false));
	Button *exit_main_menu = static_cast<Button *>(m_window_menu->GetChild("back_menu_button", true));
	Text *exit_main_menu_text = static_cast<Text *>(exit_main_menu->GetChild("back_menu_button_text", false));
	Button *exit_game = static_cast<Button *>(m_window_menu->GetChild("exit_game_button", true));
	Text *exit_game_text = static_cast<Text *>(exit_game->GetChild("exit_game_button_text", false));

	resume->SetPosition(0, text_menu->GetSize().y_ + text_menu->GetPosition().y_ + MENU_BUTTON_SPACE);
	resume_text->SetText(m_l10n->Get(resume_text->GetText()));
	exit_main_menu->SetPosition(0, resume->GetSize().y_ + resume->GetPosition().y_ + (MENU_BUTTON_SPACE * 3));
	exit_main_menu_text->SetText(m_l10n->Get(exit_main_menu_text->GetText()));
	exit_game->SetPosition(0, exit_main_menu->GetSize().y_ + exit_main_menu->GetPosition().y_ + MENU_BUTTON_SPACE);
	exit_game_text->SetText(m_l10n->Get(exit_game_text->GetText()));

	m_window_menu->SetSize(exit_game->GetSize().x_ + 50, exit_game->GetPosition().y_ + exit_game->GetSize().y_ + MENU_BUTTON_SPACE);
	SubscribeToEvent(resume, E_RELEASED, URHO3D_HANDLER(Game, HandleResume));
	SubscribeToEvent(exit_main_menu, E_RELEASED, URHO3D_HANDLER(Game, HandleBackMainMenu));
	SubscribeToEvent(exit_game, E_RELEASED, URHO3D_HANDLER(Game, HandleExitGame));
}

void Game::HandleMenu(StringHash eventType, VariantMap &eventData)
{
	CreateMenu();
	m_move_camera = false;
	GetSubsystem<Input>()->SetMouseVisible(true);
	m_window_menu->SetVisible(true);
	m_window_menu->SetModal(true);
}

void Game::HandleResume(StringHash eventType, VariantMap &eventData)
{
	GetSubsystem<Input>()->SetMouseVisible(false);
	m_window_menu->SetVisible(false);
	m_move_camera = true;
}

void Game::HandleBackMainMenu(StringHash eventType, VariantMap &eventData)
{
	m_main->ChangeGameGlobalUI(GLOBALUI_MAINMENU);
	//main_menu->Start();
}

void Game::HandleExitGame(StringHash eventType, VariantMap &eventData)
{
	m_engine->Exit();
}

Button *Game::CreateMenuButton(const String &label, const String &button_style, const String &label_style)
{
	Button *b = new Button(context_);
	b->SetStyle(button_style);
	m_window_menu->AddChild(b);
	CreateButtonLabel(b, label, label_style);

	return b;
}
}
