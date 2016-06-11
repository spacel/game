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

bool ItemStack::AddItems(uint32_t count)
{
	// @TODO check max stacking ItemDef and stack only the maximum
	// @TODO change return to -1 (fail) or not stacked count (0 if okay)
	if (m_item_count == UINT32_MAX) {
		return false;
	}

	if ((uint64_t)m_item_count + (uint64_t)count > UINT32_MAX) {
		m_item_count = UINT32_MAX;
		return true;
	}

	m_item_count += count;
	return true;
}

bool ItemStack::RemoveItems(uint32_t count)
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
}
}
