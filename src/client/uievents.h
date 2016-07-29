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
#include <common/engine/player.h>
#include "../common/threadsafe_utils.h"
#include "player.h"

namespace spacel {

class SpacelGame;
class Client;

/*
 * Client -> UI
 */
enum UIEventID {
	UI_EVENT_CHARACTER_LIST,
	UI_EVENT_MAX,
};

struct UIEvent {
	UIEvent(UIEventID _id): id(_id) {}
	virtual ~UIEvent() {}

	UIEventID id;
};
typedef std::shared_ptr<UIEvent> UIEventPtr;

struct UIEvent_CharacterList: public UIEvent {
	UIEvent_CharacterList(): UIEvent(UI_EVENT_CHARACTER_LIST) {}
	std::vector<CharacterList_Player> player_list;
};

struct UIEventHandler
{
	void (SpacelGame::*handler)(UIEventPtr event);
};

extern const UIEventHandler UIEventHandlerTable[UI_EVENT_MAX];


typedef SafeQueue<UIEventPtr> UIEventQueue;

/*
 * UI -> Client
 */
enum ClientUIEventID {
	CLIENT_UI_EVENT_CHARACTER_ADD,
	CLIENT_UI_EVENT_CHARACTER_REMOVE,
	CLIENT_UI_EVENT_MAX,
};

struct ClientUIEvent {
	ClientUIEvent(ClientUIEventID _id): id(_id) {}
	virtual ~ClientUIEvent() {}

	ClientUIEventID id;
};
typedef std::shared_ptr<ClientUIEvent> ClientUIEventPtr;

struct ClientUIEvent_CharacterAdd: public ClientUIEvent {
	ClientUIEvent_CharacterAdd(): ClientUIEvent(CLIENT_UI_EVENT_CHARACTER_ADD) {}
	Urho3D::String name;
	engine::PlayerRace race;
	engine::PlayerSex sex;
};
struct ClientUIEvent_CharacterRemove: public ClientUIEvent {
	ClientUIEvent_CharacterRemove(): ClientUIEvent(CLIENT_UI_EVENT_CHARACTER_REMOVE) {}
	uint64_t guid;
};

struct ClientUIEventHandler
{
	void (Client::*handler)(ClientUIEventPtr event);
};

extern const ClientUIEventHandler ClientUIEventHandlerTable[CLIENT_UI_EVENT_MAX];
typedef SafeQueue<ClientUIEventPtr> ClientUIEventQueue;


}
