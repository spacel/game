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

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>

using namespace Urho3D;

namespace spacel {
namespace engine {
namespace ui {

class ModalWindow : public Window
{
URHO3D_OBJECT(ModalWindow, Window);

public:
	ModalWindow(Context *context, const String &title = String::EMPTY, const String &message = String::EMPTY);
	virtual ~ModalWindow();
	static void RegisterObject(Context *context);
	void SetTitle(const String &text);
	void SetMessage(const String &text);

private:
	void HandleMessageAcknowledged(StringHash eventType, VariantMap &eventData);

	Localization *m_l10n;
	Button *m_ok_button;
	Text *m_title_text;
	Text *m_message_text;

};

}
}
}
