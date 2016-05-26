#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>

using namespace Urho3D;

namespace spacel {

class App : public Application
{
public:
	App(Context *context) :
			Application(context) {
	}

	virtual void Setup() {
// Called before engine initialization. engineParameters_ member variable can be modified here
	}

	virtual void Start() {
// Called after engine initialization. Setup application & subscribe to events here
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(spacel::App, HandleKeyDown));
	}

	virtual void Stop() {
// Perform optional cleanup after main loop has terminated
	}

	void HandleKeyDown(StringHash eventType, VariantMap &eventData) {
		using namespace KeyDown;
// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
		int key = eventData[P_KEY].GetInt();
		if (key == KEY_ESC)
			engine_->Exit();
	}
};
}

URHO3D_DEFINE_APPLICATION_MAIN(spacel::App)
