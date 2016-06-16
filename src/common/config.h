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

#include <vector>
#include <cstdint>
#include <cassert>
#include <string>
#include <Urho3D/Container/Str.h>

namespace spacel {

template <typename T>
struct SettingDefault {
	std::string name;
	T default_value;
};

class Config
{
public:
	Config() {}
	Config(uint32_t b_size, uint32_t u32_size, uint32_t f_size);
	virtual ~Config() {}

	bool load(const Urho3D::String &f);
	bool save(const Urho3D::String &f);

	inline void registerBool(const uint32_t idx, const bool v, const std::string &name)
	{
		setBool(idx, v);
		m_bool_names[idx] = name;
	}

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

	inline void registerU32(const uint32_t idx, const uint32_t v, const std::string &name)
	{
		setU32(idx, v);
		m_u32_names[idx] = name;
	}

	inline void setU32(const uint32_t idx, const uint32_t v)
	{
		assert(idx < m_u32_settings.size());
		m_u32_settings[idx] = v;
	}

	inline const uint32_t getU32(const uint32_t idx) const
	{
		assert(idx < m_u32_settings.size());
		return m_u32_settings[idx];
	}

	inline void registerFloat(const uint32_t idx, const float v, const std::string &name)
	{
		setFloat(idx, v);
		m_float_names[idx] = name;
	}

	inline void setFloat(const uint32_t idx, const float v)
	{
		assert(idx < m_float_settings.size());
		m_float_settings[idx] = v;
	}

	inline const float getFloat(const uint32_t idx) const
	{
		assert(idx < m_float_settings.size());
		return m_float_settings[idx];
	}

private:
	virtual void init() = 0;

	std::vector<bool> m_bool_settings;
	std::vector<std::string> m_bool_names;
	std::vector<uint32_t> m_u32_settings;
	std::vector<std::string> m_u32_names;
	std::vector<float> m_float_settings;
	std::vector<std::string> m_float_names;
};

}
