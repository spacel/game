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

#include "objectmanager.h"
#include <Urho3D/IO/Log.h>

namespace spacel {
namespace engine {

ObjectMgr::~ObjectMgr()
{
}

bool ObjectMgr::RegisterItem(ItemDefPtr def)
{
	if (m_itemdefs.find(def->id) != m_itemdefs.end()) {
		URHO3D_LOGWARNINGF("Unable to register item %d (%s), it was already registered",
			def->id, def->name.c_str());
		return false;
	};

	URHO3D_LOGDEBUGF("Registered item %d (%s)", def->id, def->name.c_str());
	m_itemdefs[def->id] = def;
	return true;
}

const ItemDefPtr ObjectMgr::GetItem(const uint32_t item_id) const
{
	auto idef_it = m_itemdefs.find(item_id);
	if (idef_it == m_itemdefs.end()) {
		return nullptr;
	}

	return idef_it->second;
}
}
}
