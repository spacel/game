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
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

namespace spacel {

class Config
{
public:
	Config();

	inline void setBool(const uint32_t idx, const bool v)
	{
		assert(idx < m_bool_settings.size());
		m_bool_settings[idx] = v;
	}

	inline const bool getBool(const uint32_t idx) const
	{
		assert(idx < m_bool_settings.size());
		return m_bool_settings[idx];
	}

private:
	std::vector<bool> m_bool_settings;
};

}