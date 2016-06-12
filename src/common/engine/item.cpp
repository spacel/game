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

#include <cassert>
#include "item.h"
#include "objectmanager.h"

namespace spacel {
namespace engine {

uint16_t ItemStack::AddItems(const uint16_t count)
{
	ItemDefPtr idef = ObjectMgr::instance()->GetItem(m_item_id);
	assert(idef);

	if (m_item_count == idef->stack_max) {
		return count;
	}

	if ((uint32_t)m_item_count + (uint32_t)count > idef->stack_max) {
		// Calculate the item overhead
		uint32_t diff = m_item_count + count - (uint16_t)idef->stack_max;
		m_item_count = idef->stack_max;
		return (uint16_t) diff;
	}

	m_item_count += count;
	return 0;
}

bool ItemStack::RemoveItems(const uint16_t count)
{
	if (m_item_count == 0) {
		return false;
	}

	if (count > m_item_count) {
		m_item_count = 0;
		return true;
	}

	m_item_count -= count;
}

void ItemStack::SetItemCount(const uint16_t count)
{
	ItemDefPtr idef = ObjectMgr::instance()->GetItem(m_item_id);
	assert(idef && count <= idef->stack_max);
	m_item_count = count;
}
}
}
