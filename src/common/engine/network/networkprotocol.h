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

#include <stddef.h>
#include <stdint.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <vector>

namespace spacel {
namespace engine {

class Server;

namespace network {

class NetworkPacket: public Urho3D::Deserializer, public Urho3D::Serializer
{
public:
	NetworkPacket(const uint16_t o);

	const uint16_t GetOpcode();

	/// Read bytes from the memory area. Return number of bytes actually read.
	virtual unsigned Read(void *dest, unsigned size);
	/// Set position from the beginning of the memory area.
	virtual unsigned Seek(unsigned position);
	/// Write bytes to the memory area.
	virtual unsigned Write(const void *data, unsigned size);

private:
	uint32_t m_session_id = 0;

	/// Pointer to the memory area.
	std::vector<unsigned char> buffer_;
};

enum PacketOpcode
{
	CMSG_HELLO,
	SMSG_HELLO,
	CMSG_CHAT,
	SMSG_CHAT,
	CMSG_AUTH,
	SMSG_AUTH,
	SMSG_CHARACTER_LIST,
	CMSG_CHARACTER_CREATE,
	SMSG_CHARACTER_CREATE,
	CMSG_CHARACTER_REMOVE,
	SMSG_CHARACTER_REMOVE,
	CMSG_CHARACTER_CONNECT,
	SMSG_KICK,
	SMSG_GALAXY_SYSTEMS,
	MSG_MAX,
};

enum SessionState
{
	SESSION_STATE_NONE,
	SESSION_STATE_NOT_CONNECTED,
	SESSION_STATE_CONNECTED,
	SESSION_STATE_AUTHED,
};

struct SMsgHandler
{
	const char *name;
	SessionState state;
	void (Server::*handler)(NetworkPacket *packet);
};

extern const SMsgHandler smsgHandlerTable[MSG_MAX];
}
}
}


