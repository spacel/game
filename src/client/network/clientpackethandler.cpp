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

#include "clientpackethandler.h"

namespace spacel {
namespace engine {
namespace network {

const static CMsgHandler null_command_handler = {"MSG_NULL", SESSION_STATE_NONE, &Client::handlePacket_Null};

const CMsgHandler cmsgHandlerTable[MSG_MAX] = {
	null_command_handler,
	{"SMSG_HELLO", SESSION_STATE_NOT_CONNECTED, &Client::handlePacket_Hello},
	null_command_handler,
	{"SMSG_CHAT", SESSION_STATE_AUTHED, &Client::handlePacket_Chat},
	null_command_handler,
	{"SMSG_AUTH", SESSION_STATE_CONNECTED, &Client::handlePacket_Auth},
	{"SMSG_CHARACTER_LIST", SESSION_STATE_AUTHED, &Client::handlePacket_CharacterList},
	null_command_handler,
	{"SMSG_CHARACTER_CREATE", SESSION_STATE_AUTHED, &Client::handlePacket_CharacterCreate},
	null_command_handler,
	{"SMSG_CHARACTER_REMOVE", SESSION_STATE_AUTHED, &Client::handlePacket_CharacterRemove},
	null_command_handler,
	{"SMSG_KICK", SESSION_STATE_AUTHED, &Client::handlePacket_Kick},
	{"SMSG_GALAXY_SYSTEMS", SESSION_STATE_AUTHED, &Client::handlePacket_GalaxySystems},
};
}
}
}
