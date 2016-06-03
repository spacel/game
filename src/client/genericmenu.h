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

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/Log.h>

namespace spacel {

class GenericMenu: public Urho3D::UIElement {
public:
	GenericMenu(Urho3D::Context *context);
protected:
	void CreateButtonLabel(Urho3D::Button *b, const Urho3D::String &text,
			const Urho3D::String &style = "TextButton") const;
	void CreateLineEditLabel(Urho3D::LineEdit *le, const Urho3D::String &text,
			const Urho3D::String &style = "LineEditLabel") const;
	void TakeScreenshot();
	Urho3D::Localization *m_l10n;
	Urho3D::SharedPtr<Urho3D::Engine> m_engine;
	Urho3D::SharedPtr<Urho3D::Log> m_log;
};
}
