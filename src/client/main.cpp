#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

using namespace Urho3D;

namespace spacel {

class App : public Application
{
public:
    SharedPtr<Text> text_;
    SharedPtr<Scene> scene_;
    SharedPtr<Node> boxNode_;
    SharedPtr<Node> cameraNode_;


    App(Context* context) :
            Application(context)
    {
    }

    virtual void Setup()
    {
        // Called before engine initialization. engineParameters_ member variable can be modified here
        engineParameters_["WindowTitle"] = "Spacel Game";
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1280;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=true;
        GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
    }

    virtual void Start()
    {
        // Called after engine initialization. Setup application & subscribe to events here
        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(App, HandleKeyDown));

        ResourceCache* cache=GetSubsystem<ResourceCache>();

        // Let's use the default style that comes with Urho3D.
        GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

        // Let's create some text to display.
        text_=new Text(context_);
        // Text will be updated later in the E_UPDATE handler. Keep readin'.
        text_->SetText("Spacel Game.");
        text_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),40);
        text_->SetHorizontalAlignment(HA_CENTER);
        text_->SetVerticalAlignment(VA_TOP);
        GetSubsystem<UI>()->GetRoot()->AddChild(text_);

        Window* window_menu = new Window(context_);
        window_menu->SetSize(600,600);
        window_menu->SetHorizontalAlignment(HA_CENTER);
        window_menu->SetVerticalAlignment(VA_CENTER);
        window_menu->SetStyle("Window");
        GetSubsystem<UI>()->GetRoot()->AddChild(window_menu);


        Button* singleplayer = new Button(context_);
        window_menu->AddChild(singleplayer);
        // Note, must be part of the UI system before SetSize calls!
        singleplayer->SetName("Jouer");
        singleplayer->SetStyle("Button");
        singleplayer->SetPosition(16,window_menu->GetHeight() / 5 + window_menu->GetPosition().y_);
        singleplayer->SetHorizontalAlignment(HA_CENTER);

        Text* textSinglePlayer = new Text(context_);
        textSinglePlayer->SetText("Jouer");
        textSinglePlayer->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);

        singleplayer->AddChild(textSinglePlayer);


        Button* multiplayer = new Button(context_);
        // Note, must be part of the UI system before SetSize calls!
        window_menu->AddChild(multiplayer);
        multiplayer->SetName("Multiplayer");
        multiplayer->SetStyle("Button");
        multiplayer->SetPosition(16,window_menu->GetHeight() / 5 * 2 + window_menu->GetPosition().y_);
        multiplayer->SetHorizontalAlignment(HA_CENTER);

        Text* textMultiplayer = new Text(context_);
        textMultiplayer->SetText("Multijoueur");
        textMultiplayer->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);

        multiplayer->AddChild(textMultiplayer);


        Button* settings = new Button(context_);
        // Note, must be part of the UI system before SetSize calls!
        window_menu->AddChild(settings);
        settings->SetName("Options");
        settings->SetStyle("Button");
        settings->SetPosition(16,(window_menu->GetHeight() / 5 * 3) + window_menu->GetPosition().y_);
        settings->SetHorizontalAlignment(HA_CENTER);

        Text* textSettings = new Text(context_);
        textSettings->SetText("Settings");
        textSettings->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);

        settings->AddChild(textSettings);

        Button* button=new Button(context_);
        // Note, must be part of the UI system before SetSize calls!
        window_menu->AddChild(button);
        button->SetName("Quitter");
        button->SetStyle("Button");
        button->SetPosition(16,window_menu->GetHeight() / 5 * 4 + window_menu->GetPosition().y_);
        button->SetHorizontalAlignment(HA_CENTER);
        SubscribeToEvent(button,E_RELEASED,URHO3D_HANDLER(App,HandleClosePressed));

        Text* textButton = new Text(context_);
        textButton->SetText("Quitter");
        textButton->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);

        button->AddChild(textButton);

        // Let's setup a scene to render.
        scene_=new Scene(context_);
        // Let the scene have an Octree component!
        scene_->CreateComponent<Octree>();
        // Let's add an additional scene component for fun.
        scene_->CreateComponent<DebugRenderer>();

        // Let's put some sky in there.
        // Again, if the engine can't find these resources you need to check
        // the "ResourcePrefixPath". These files come with Urho3D.
        Node* skyNode=scene_->CreateChild("Sky");
        skyNode->SetScale(500.0f); // The scale actually does not matter
        Skybox* skybox=skyNode->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

        // Let's put a box in there.
        boxNode_=scene_->CreateChild("Box");
        boxNode_->SetPosition(Vector3(0,2,15));
        boxNode_->SetScale(Vector3(3,3,3));
        StaticModel* boxObject=boxNode_->CreateComponent<StaticModel>();
        boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
        boxObject->SetCastShadows(true);

        // Create 400 boxes in a grid.
        for(int x=-30;x<30;x+=3)
           for(int z=0;z<60;z+=3)
           {
               Node* boxNode_=scene_->CreateChild("Box");
               boxNode_->SetPosition(Vector3(x,-3,z));
               boxNode_->SetScale(Vector3(2,2,2));
               StaticModel* boxObject=boxNode_->CreateComponent<StaticModel>();
               boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
               boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
               boxObject->SetCastShadows(true);
           }

        // We need a camera from which the viewport can render.
        cameraNode_=scene_->CreateChild("Camera");
        Camera* camera=cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(2000);

        Renderer* renderer=GetSubsystem<Renderer>();
        SharedPtr<Viewport> viewport(new Viewport(context_,scene_,cameraNode_->GetComponent<Camera>()));
        renderer->SetViewport(0,viewport);

    }

    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }

    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESC)
            engine_->Exit();
    }

    void HandleClosePressed(StringHash eventType,VariantMap& eventData)
    {
        engine_->Exit();
    }
};
}

URHO3D_DEFINE_APPLICATION_MAIN(spacel::App)
