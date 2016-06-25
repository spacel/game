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

#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>

#include "genericmenu.h"
#include "settings.h"

using namespace Urho3D;

namespace spacel {

GenericMenu::GenericMenu(Context *context, ClientSettings *config): UIElement(context),
		m_l10n(GetSubsystem<Localization>()),
		m_cache(GetSubsystem<ResourceCache>()),
		m_engine(GetSubsystem<Engine>()),
		m_config(config)
{
	m_scene = new Scene(context_);
}

/*
 * This function translate the label
 */
void GenericMenu::CreateButtonLabel(Button *b, const String &text,
		const String &style) const
{
	Text *t = new Text(context_);
	b->AddChild(t);
	t->SetStyle(style);
	t->SetText(m_l10n->Get(text));
}

void GenericMenu::CreateLineEditLabel(LineEdit *le, const String &text,
		const String &style) const
{
	Text *t = new Text(context_);
	le->GetParent()->AddChild(t);
	t->SetName(text);
	t->SetStyle(style);
	t->SetPosition(25, le->GetPosition().y_ + 5);
	t->SetText(m_l10n->Get(text));
}

void GenericMenu::TakeScreenshot(const bool play_sound)
{
	Image screenshot(context_);
	GetSubsystem<Graphics>()->TakeScreenShot(screenshot);
	// Here we save in the Data folder with date and time appended
	screenshot.SavePNG(GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "screenshots") + "Screenshot_" +
		Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
	if (play_sound) {
		PlaySound("sounds/screenshot.ogg");
	}
}

void GenericMenu::PlaySound(const String &sound_name)
{
	if (!GetSubsystem<FileSystem>()->FileExists(
				GetSubsystem<FileSystem>()->GetProgramDir() + "/Data/" + sound_name)) {
		URHO3D_LOGERRORF("Sound: %s doesn't exist", sound_name.CString());
		return;
	}
	SoundSource *soundSource = m_scene->CreateChild("Sound")->CreateComponent<SoundSource>();
	soundSource->SetGain(m_config->getFloat(FLOATSETTINGS_SOUND_UI_GAIN));
	soundSource->Play(m_cache->GetResource<Sound>(sound_name));
}

void GenericMenu::PlayMusic(const bool activate)
{
	// Check if the music player node/component already exist
	if (!m_scene->GetChild("Music") && activate) {
		Sound *music = m_cache->GetResource<Sound>("Music/Gnawa-Spirit.ogg");
		if (!music) {
			URHO3D_LOGERROR("Main menu music doesn't exist");
			return;
		}
		music->SetLooped(true);
		SoundSource *musicSource = m_scene->CreateChild("Music")->CreateComponent<SoundSource>();
		// Set the sound type to music so that master volume control works correctly
		musicSource->SetSoundType(SOUND_MUSIC);
		musicSource->Play(music);
		GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, m_config->getFloat(FLOATSETTINGS_SOUND_MUSIC_GAIN) / 100);
	} else {
		m_scene->RemoveChild(m_scene->GetChild("Music"));
	}
}
}
