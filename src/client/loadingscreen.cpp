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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <stdlib.h>
#include <common/macro_utils.h>

#include "loadingscreen.h"

using namespace Urho3D;

namespace spacel {

LoadingScreen::LoadingScreen(Context *context, ClientSettings *config, engine::Server *server) :
	GenericMenu(context, config),
	m_server(server)
{
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/LoadingScreenStyle.xml"));
	m_progress_bar = new engine::ui::ProgressBar(context_);
	m_loading_text = new Text(context_);
	m_tips_text = new Text(context_);
}

void LoadingScreen::Start()
{
	ShowBackground();
	ShowProgressBar();
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

void LoadingScreen::ShowProgressBar()
{
	m_ui_elem->AddChild(m_progress_bar);
	m_progress_bar->SetStyle("Slider");
	m_progress_bar->SetSize(m_ui_elem->GetSize().x_ -50 , 20);
	m_progress_bar->SetRange(100);
	m_progress_bar->SetAlignment(HA_CENTER, VA_BOTTOM);
	m_progress_bar->SetPosition(0, -30);

	m_ui_elem->AddChild(m_loading_text);
	m_loading_text->SetStyle("Text");
	m_loading_text->SetAlignment(HA_CENTER, VA_BOTTOM);
	m_loading_text->SetPosition(0, -10);
	m_loading_text->SetText(m_l10n->Get("Loading..."));
}

static const char *tip_list[] = {
		"You can change the settings somewhere.",
		"Don't forget to do a pause after some hours of gaming.",
};

void LoadingScreen::ShowTips()
{
	m_ui_elem->AddChild(m_tips_text);
	m_tips_text->SetStyle("Text");
	m_tips_text->SetPosition(0, -70);
	m_tips_text->SetAlignment(HA_CENTER, VA_BOTTOM);
	m_tips_text->SetText(tip_list[rand() % (ARRLEN(tip_list) - 1)]);
}

static const char *loading_texts[engine::SERVERLOADINGSTEP_COUNT] = {
	"Not started.",
	"Loading...",
	"Game DB backend inited.",
	"Started!",
	"Failed!"
};

void LoadingScreen::HandleUpdate(StringHash, VariantMap &eventData)
{
	m_loading_background->SetSize(m_ui_elem->GetSize().x_,
		m_ui_elem->GetSize().y_ - 50);

	m_progress_bar->SetSize(m_ui_elem->GetSize().x_ -50 , 20);

	const auto loading_step = m_server->getLoadingStep();
	if (m_last_loading_step != loading_step) {
		switch (loading_step) {
			case engine::SERVERLOADINGSTEP_NOT_STARTED:
				m_progress_bar->SetValue(0);
				m_loading_text->SetText(
						m_l10n->Get(loading_texts[m_server->getLoadingStep()]));
				break;
			case engine::SERVERLOADINGSTEP_BEGIN_START:
				m_progress_bar->SetValue(5);
				m_loading_text->SetText(
						m_l10n->Get(loading_texts[m_server->getLoadingStep()]));
				break;
			case engine::SERVERLOADINGSTEP_DB_INITED:
				m_progress_bar->SetValue(10);
				m_loading_text->SetText(
						m_l10n->Get(loading_texts[m_server->getLoadingStep()]));
				break;
			case engine::SERVERLOADINGSTEP_STARTED:
				m_progress_bar->SetValue(100);
				m_loading_text->SetText(
						m_l10n->Get(loading_texts[m_server->getLoadingStep()]));
				break;
			case engine::SERVERLOADINGSTEP_FAILED:
				m_progress_bar->SetValue(0);
				m_loading_text->SetText(
						m_l10n->Get(loading_texts[m_server->getLoadingStep()]));
				break;
			default:
				assert(false);
		}
		m_last_loading_step = loading_step;
	}

}
}
