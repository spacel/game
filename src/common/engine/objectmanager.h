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

#include <unordered_map>
#include <cstdint>
#include "item.h"

namespace spacel {
namespace engine {
class ObjectMgr
{
public:
	ObjectMgr() {}
	~ObjectMgr();

	inline static ObjectMgr *instance()
	{
		if (!ObjectMgr::s_objmgr) {
			ObjectMgr::s_objmgr = new ObjectMgr();
		}

		return ObjectMgr::s_objmgr;
	}

	bool RegisterItem(ItemDefPtr def);
	const ItemDefPtr GetItem(const uint32_t item_id) const;

	const uint32_t GetRegisteredItemsCount() const { return m_itemdefs.size(); }
private:
	static ObjectMgr *s_objmgr;
	std::unordered_map<uint32_t, ItemDefPtr> m_itemdefs;
};
}
}
