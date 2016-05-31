#ifndef SPACELGAME_H
#define SPACELGAME_H

#include <Urho3D/Engine/Application.h>
#include <Urho3D/IO/Log.h>

#include "settings.h"

using namespace Urho3D;

namespace spacel {

class SpacelGame : public Application
{
	URHO3D_OBJECT(SpacelGame, Application);

public:
	SpacelGame(Context *context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();

protected:
	void HandleClosePressed(StringHash eventType,VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);

	SharedPtr<Engine>	engine_;
	SharedPtr<Log>		log_;

private:
	void MusicMenu(bool active);
	ClientSettings *m_config;
};
}
#endif // SPACELGAME_H
