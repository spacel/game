/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
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
#include <Urho3D/IO/Log.h>

#include "settings.h"

using namespace Urho3D;

namespace spacel {

class SpacelGame : public Application
{
	URHO3D_OBJECT(SpacelGame, Application);

public:
	SpacelGame(Context *context): Application(context) {}
	virtual void Setup();
	virtual void Start();
	virtual void Stop();

protected:
	void HandleClosePressed(StringHash eventType,VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);

	SharedPtr<Engine>	engine_;
	SharedPtr<Log>		log_;

private:
	void MusicMenu(const bool active);
	ClientSettings *m_config;
};

}
