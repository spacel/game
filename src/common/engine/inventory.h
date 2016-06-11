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

#include <cstdint>
#include <memory>
#include <unordered_map>
#include "item.h"

namespace spacel {
namespace engine {

class Inventory
{
public:
	Inventory(uint16_t size);
	~Inventory() {}

	bool AddItemIntoFirstAvailableSlot(ItemStackPtr stack);
	ItemStackPtr GetItem(uint16_t slot_id);
	bool AddItem(uint16_t slot_id, ItemStackPtr stack);
private:
	uint16_t m_size = 0;
	std::unordered_map<uint16_t, ItemStackPtr> m_items;
};
}
}
