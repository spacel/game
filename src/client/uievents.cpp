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

#include "uievents.h"
#include "spacelgame.h"
#include "client.h"

namespace spacel {

const UIEventHandler UIEventHandlerTable[UI_EVENT_MAX] = {
	&SpacelGame::HandleCharacterList,
};

const ClientUIEventHandler ClientUIEventHandlerTable[CLIENT_UI_EVENT_MAX] = {
	&Client::handleClientUiEvent_ChararacterAdd,
	&Client::handleClientUiEvent_ChararacterRemove,
};
}
