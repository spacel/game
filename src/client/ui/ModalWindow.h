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

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/Localization.h>

namespace spacel {
namespace engine {
namespace ui {

class URHO3D_API ModalWindow: public Urho3D::Window
{
URHO3D_OBJECT(ModalWindow, Window);

public:
	ModalWindow(Urho3D::Context *context);
	virtual ~ModalWindow();
	static void RegisterObject(Urho3D::Context *context);
	void InitComponents(const Urho3D::String &title, const Urho3D::String &message,
							bool show_close_button = false);

private:
	void HandleMessageAcknowledged(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
	void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);

	Urho3D::Localization *m_l10n;
	Urho3D::SharedPtr<Urho3D::Button> m_ok_button;
	Urho3D::SharedPtr<Urho3D::Text> m_title_text;
	Urho3D::SharedPtr<Urho3D::Text> m_message_text;
};

}
}
}
