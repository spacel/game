/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
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

#include <cmath>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "loadingscreen.h"

using namespace Urho3D;

namespace spacel {

LoadingScreen::LoadingScreen(Context *context, ClientSettings *config, engine::Server *server) :
	GenericMenu(context, config),
	m_server(server)
{
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/LoadingScreenStyle.xml"));
	m_loading_bar = new Slider(context_);
	m_loading_text = new Text(context_);
	m_loading_percent_text = new Text(context_);
	m_tips_text = new Text(context_);
}

void LoadingScreen::Start()
{
	ShowBackground();
	LoadingBar();
	ShowTips();

	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(LoadingScreen, HandleUpdate));
}

void LoadingScreen::ShowBackground()
{
	Texture2D *background_texture = m_cache->GetResource<Texture2D>("Textures/background_loading.png");
	if (!background_texture) {
		URHO3D_LOGERROR("Background loading texture not loaded");
		return;
	}
	m_loading_background = m_ui_elem->CreateChild<Sprite>();
	m_loading_background->SetTexture(background_texture);
	m_loading_background->SetSize(m_ui_elem->GetSize().x_, m_ui_elem->GetSize().y_ - 50);
	m_loading_background->SetPriority(-100);
}

void LoadingScreen::LoadingBar()
{
	m_ui_elem->AddChild(m_loading_bar);
	m_loading_bar->SetStyle("Slider");
	m_loading_bar->SetSize(m_ui_elem->GetSize().x_ -50 , 20);
	m_loading_bar->AddChild(m_loading_percent_text);
	m_loading_bar->SetEditable(false);
	m_loading_bar->SetFocus(false);
	m_loading_bar->SetEnabled(false);
	m_loading_bar->SetRange(100);

	m_loading_percent_text->SetStyle("Text");
	m_loading_percent_text->SetAlignment(HA_CENTER, VA_CENTER);
	m_loading_percent_text->SetText("0 %");

	m_ui_elem->AddChild(m_loading_text);
	m_loading_text->SetStyle("Text");
	m_loading_text->SetAlignment(HA_CENTER, VA_CENTER);
	m_loading_text->SetPosition(0, 25);
	m_loading_text->SetText("Chargement du cache");
}

void LoadingScreen::ShowTips()
{
	m_ui_elem->AddChild(m_tips_text);
	m_tips_text->SetStyle("Text");
	m_tips_text->SetPosition(0, m_ui_elem->GetSize().y_ - 45);
	m_tips_text->SetHorizontalAlignment(HA_CENTER);
	m_tips_text->SetText("Il pleut aujourd'hui\n Mais il fera surement beau demain!!");
}

void LoadingScreen::SetLoadingPercentText(const float &range)
{
	int new_range = static_cast<int>(range);
	// @TODO: range = 100 percent = 0 - range = 1 = 50% - Range = 0 percent 100%
	if (new_range < 0) {
		new_range = 0;
	 } else if (range > 100) {
		new_range = 100;
	}
	m_loading_percent_text->SetText(ToString("%d", new_range) + " %");
}

void LoadingScreen::HandleUpdate(StringHash, VariantMap &eventData)
{
	m_loading_background->SetSize(m_ui_elem->GetSize().x_,
		m_ui_elem->GetSize().y_ - 50);

	// @TODO : For test
	//m_loading_bar->SetRange(m_loading_bar->GetRange() - 0.05);
	SetLoadingPercentText(m_loading_bar->GetRange());

	// @TODO: WIP
	switch (server_loading_step = m_server->getLoadingStep()) {
	case engine::SERVERLOADINGSTEP_NOT_STARTED:

		break;
	case engine::SERVERLOADINGSTEP_BEGIN_START:

		break;
	case engine::SERVERLOADINGSTEP_STARTED:

		break;
	case engine::SERVERLOADINGSTEP_DB_INITED:

		break;
	case engine::SERVERLOADINGSTEP_FAILED:

		break;
	default:
		break;
	}

}
}
