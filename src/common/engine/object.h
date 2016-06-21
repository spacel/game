/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Vincent Glize <vincent.glize@live.fr>
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

#include <Urho3D/Math/Vector3.h>
#include <stdint.h>

namespace spacel {

namespace engine {

enum ObjectType
{
	OBJECT_TYPE_OBJECT,
	OBJECT_TYPE_UNIT,
	OBJECT_TYPE_PLAYER,
	OBJECT_TYPE_GAMEOBJECT,
};

class Object {
public:
	Object() {};
	~Object() {};
	virtual const ObjectType GetType() const { return OBJECT_TYPE_OBJECT; }
protected:
	uint64_t m_guid = 0;
	Urho3D::Vector3 m_position;
};

};

}
