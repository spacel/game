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

#include "networkprotocol.h"
#include "../server.h"

namespace spacel {
namespace engine {
namespace network {

const static SMsgHandler null_command_handler = {"MSG_NULL", SESSION_STATE_NONE, &Server::handlePacket_Null};

const SMsgHandler smsgHandlerTable[MSG_MAX] = {
	{"CMSG_HELLO", SESSION_STATE_NOT_CONNECTED, &Server::handlePacket_Hello},
	null_command_handler,
	{"CMSG_CHAT", SESSION_STATE_AUTHED, &Server::handlePacket_Chat},
	null_command_handler,
	{"CMSG_AUTH", SESSION_STATE_CONNECTED, &Server::handlePacket_Auth},
	null_command_handler,
	null_command_handler,
	{"CMSG_CHARACTER_CREATE", SESSION_STATE_CONNECTED, &Server::handlePacket_CharacterCreate},
	null_command_handler,
	{"CMSG_CHARACTER_REMOVE", SESSION_STATE_CONNECTED, &Server::handlePacket_CharacterRemove},
	null_command_handler,
	{"CMSG_CHARACTER_CONNECT", SESSION_STATE_CONNECTED, &Server::handlePacket_CharacterConnect},
	null_command_handler,
	null_command_handler,
};
}
}
}
