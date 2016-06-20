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

#include "ModalWindow.h"

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace spacel {
namespace engine {
namespace ui {

ModalWindow::ModalWindow(Context *context) :
	Window(context),
	m_l10n(GetSubsystem<Localization>())
{
	ResourceCache *cache = GetSubsystem<ResourceCache>();
	SetStyle("ModalWindow", cache->GetResource<XMLFile>("UI/ModalWindow.xml"));
	SetModal(true);
	SubscribeToEvent(this, E_MODALCHANGED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));


}

void ModalWindow::InitComponents(const String &title, const String &message)
{
	m_title_text = dynamic_cast<Text *>(GetChild("TitleText", true));
	assert(m_title_text);
	m_title_text->SetText(m_l10n->Get(title));
	m_message_text = dynamic_cast<Text *>(GetChild("MessageText", true));
	m_message_text->SetText(m_l10n->Get(message));

	m_ok_button = dynamic_cast<Button *>(GetChild("OkButton", true));
	if (m_ok_button) {
		SubscribeToEvent(m_ok_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));
	}

	Button *cancel_button = dynamic_cast<Button *>(GetChild("CancelButton", true));
	if (cancel_button) {
		Text *cancelButtonText = dynamic_cast<Text *>(GetChild("CancelButtonText", true));
		cancelButtonText->SetText(m_l10n->Get(cancelButtonText->GetText()));
		SubscribeToEvent(cancel_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));
	}

	Button *close_button = dynamic_cast<Button *>(GetChild("CloseButton", true));
	if (close_button) {
		SubscribeToEvent(close_button, E_RELEASED, URHO3D_HANDLER(ModalWindow, HandleMessageAcknowledged));
	}

}
void ModalWindow::RegisterObject(Context *context)
{
	context->RegisterFactory<ModalWindow>("UI");
}

void ModalWindow::SetTitle(const String &text)
{
	if (m_title_text)
		m_title_text->SetText(m_l10n->Get(text));
}

void ModalWindow::SetMessage(const String &text)
{
	if (m_message_text)
		m_message_text->SetText(m_l10n->Get(text));
}

void ModalWindow::HandleMessageAcknowledged(StringHash eventType, VariantMap &eventData)
{
	using namespace MessageACK;

	VariantMap &newEventData = GetEventDataMap();
	newEventData[P_OK] = eventData[Released::P_ELEMENT] == m_ok_button;
	SendEvent(E_MESSAGEACK, newEventData);

	this->ReleaseRef();
}

}
}
}
