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

#pragma once

#include <memory>
#include "../common/threadsafe_utils.h"

namespace spacel {

class SpacelGame;

enum UIEventID {
	UI_EVENT_CHARACTER_LIST,
	UI_EVENT_MAX,
};

struct UIEvent {
	UIEvent(UIEventID _id, void *_data): id(_id), data(_data) {}
	UIEventID id;
	void *data;
};

struct UIEventHandler
{
	void (SpacelGame::*handler)(UIEventID event_id, void *data);
};

extern const UIEventHandler UIEventHandlerTable[UI_EVENT_MAX];

typedef std::shared_ptr<UIEvent> UIEventPtr;
typedef SafeQueue<UIEventPtr> UIEventQueue;
}
