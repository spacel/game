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
#include "inventory.h"

namespace spacel {
namespace engine {
Inventory::Inventory(uint16_t size)
{
	assert(size > 0);
	m_size = size;
}

bool Inventory::AddItemIntoFirstAvailableSlot(std::shared_ptr<ItemStack> stack)
{
	for (uint16_t i = 0; i < m_size; i++) {
		if (m_items.find(i) == m_items.end()) {
			m_items[i] = stack;
			return true;
		}
	}

	return false;
}

ItemStackPtr Inventory::GetItem(const uint16_t slot_id)
{
	assert(slot_id < m_size);
	if (m_items.find(slot_id) != m_items.end()) {
		return m_items[slot_id];
	}

	return nullptr;
}

bool Inventory::AddItem(const uint16_t slot_id, ItemStackPtr stack)
{
	assert(slot_id < m_size);
	auto current_stack = m_items.find(slot_id);
	if (current_stack == m_items.end()) {
		m_items[slot_id] = stack;
		return true;
	}

	// Check if we are adding items on a similar stack
	if (stack->GetItemID() != (*current_stack->second).GetItemID()) {
		return false;
	}

	// Add items from stack to the Inventory Stack and set the new stack amount to stack
	stack->SetItemCount((*current_stack->second).AddItems(stack->GetItemCount()));
	return true;
}
}
}
