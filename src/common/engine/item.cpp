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

#include "item.h"

namespace spacel {
namespace engine {

uint16_t ItemStack::AddItems(const uint16_t count)
{
	// @TODO: check we don't exceed ItemDef stack size
	if (m_item_count == UINT16_MAX) {
		return count;
	}

	if ((uint32_t)m_item_count + (uint32_t)count > UINT16_MAX) {
		// Calculate the item overhead
		uint32_t diff = m_item_count + count - (uint16_t)UINT16_MAX;
		m_item_count = UINT16_MAX;
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
	// @TODO: check we don't exceed ItemDef stack size
	m_item_count = count;
}
}
}
