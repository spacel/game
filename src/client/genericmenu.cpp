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

#include "genericmenu.h"

#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace spacel {

GenericMenu::GenericMenu(Context *context): UIElement(context),
		m_l10n(GetSubsystem<Localization>()),
		m_engine(GetSubsystem<Engine>())
{
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
}
