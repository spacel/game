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

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Input/InputEvents.h>

#include "ModalWindow.h"

using namespace Urho3D;

namespace spacel {
namespace engine {
namespace ui {

ModalWindow::ModalWindow(Context *context) :
	Urho3D::Window(context),
	m_l10n(GetSubsystem<Localization>())
{
	m_ok_button = nullptr;
	m_title_text = nullptr;
	m_message_text = nullptr;
}

ModalWindow::~ModalWindow()
{
	RemoveAllChildren();
	Remove();
}

void ModalWindow::InitComponents(const String &title, const String &message,
	bool show_close_button)
{
	assert(!m_ok_button); // ok button should not have been inited

	m_title_text = dynamic_cast<Text *>(GetChild("TitleText", true));
	assert(m_title_text);
	m_title_text->SetText(m_l10n->Get(title));

	m_message_text = dynamic_cast<Text *>(GetChild("MessageText", true));
	m_message_text->SetText(m_l10n->Get(message));

	m_ok_button = dynamic_cast<Button *>(GetChild("OkButton", true));
	assert(m_ok_button);

	Button *cancel_button = dynamic_cast<Button *>(GetChild("CancelButton", true));
	Text *cancelButtonText = dynamic_cast<Text *>(GetChild("CancelButtonText", true));
	assert(cancel_button && cancelButtonText);

	cancelButtonText->SetText(m_l10n->Get(cancelButtonText->GetText()));

	Button *close_button = dynamic_cast<Button *>(GetChild("CloseButton", true));
	assert(close_button);

	if (show_close_button) {
		SubscribeToEvent(close_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));
	}
	else {
		close_button->Remove();
	}

	SetModal(true);
	SubscribeToEvent(this, E_MODALCHANGED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));

	SubscribeToEvent(m_ok_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));
	SubscribeToEvent(cancel_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ModalWindow, HandleKeyDown));

}
void ModalWindow::RegisterObject(Context *context)
{
	context->RegisterFactory<ModalWindow>("UI");
	URHO3D_COPY_BASE_ATTRIBUTES(Window);
}

void ModalWindow::HandleMessageAcknowledged(StringHash, VariantMap &eventData)
{
	using namespace MessageACK;

	VariantMap &newEventData = GetEventDataMap();
	newEventData[P_OK] = eventData[Released::P_ELEMENT] == m_ok_button;
	SendEvent(E_MESSAGEACK, newEventData);

	this->ReleaseRef();
}

void ModalWindow::HandleKeyDown(StringHash, VariantMap &eventData)
{
	using namespace KeyDown;
	using namespace MessageACK;

	int key = eventData[P_KEY].GetInt();

	switch (key) {
		case KEY_ESCAPE: {
			VariantMap &newEventData = GetEventDataMap();
			newEventData[P_OK] = false;
			SendEvent(E_MESSAGEACK, newEventData);
			this->ReleaseRef();
			break;
		}
		case KEY_RETURN: {
			VariantMap &newEventData = GetEventDataMap();
			newEventData[P_OK] = true;
			SendEvent(E_MESSAGEACK, newEventData);
			this->ReleaseRef();
			break;
		}
		default: break;
	}
}

}
}
}
