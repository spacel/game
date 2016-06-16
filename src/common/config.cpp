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

#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <json/json.h>
#include <fstream>
#include <iostream>
#include "config.h"

using namespace Urho3D;

namespace spacel
{

Config::Config(uint32_t b_size, uint32_t u32_size, uint32_t f_size)
{
	m_bool_settings.resize(b_size);
	m_u32_settings.resize(u32_size);
	m_float_settings.resize(f_size);

	m_bool_names.resize(b_size);
	m_u32_names.resize(u32_size);
	m_float_names.resize(f_size);
}

bool Config::load(const String &f)
{
	try {
		std::ifstream cfg_file(f.CString(), std::ifstream::binary);
		if (!cfg_file.good()) {
			return false;
		}
		Json::Value root;
		cfg_file >> root;
		cfg_file.close();

		for (const auto &cfg_name: root.getMemberNames()) {
			for (uint32_t i = 0; i < m_bool_names.size(); ++i) {
				if (m_bool_names[i].compare(cfg_name) == 0) {
					Json::Value val = root[cfg_name];
					if (val.isBool()) {
						m_bool_settings[i] = val.asBool();
					}
					continue;
				}
			}

			for (uint32_t i = 0; i < m_u32_names.size(); ++i) {
				if (m_u32_names[i].compare(cfg_name) == 0) {
					Json::Value val = root[cfg_name];
					if (val.isInt()) {
						m_u32_settings[i] = (uint32_t) val.asInt();
					}
					continue;
				}
			}

			for (uint32_t i = 0; i < m_float_names.size(); ++i) {
				if (m_float_names[i].compare(cfg_name) == 0) {
					Json::Value val = root[cfg_name];
					if (val.isDouble()) {
						m_float_settings[i] = val.asFloat();
					}
					continue;
				}
			}
		}
		return true;
	}
	catch (std::exception &e) {
		return false;
	}
}

bool Config::save(const String &f)
{
	Json::Value root;

	for (uint32_t i = 0; i < m_bool_settings.size(); ++i) {
		root[m_bool_names[i]] = (bool) m_bool_settings[i];
	}

	for (uint32_t i = 0; i < m_u32_settings.size(); ++i) {
		root[m_u32_names[i]] = m_u32_settings[i];
	}

	for (uint32_t i = 0; i < m_float_settings.size(); ++i) {
		root[m_float_names[i]] = m_float_settings[i];
	}

	try {
		std::ofstream cfg_file(f.CString(), std::ofstream::binary);
		if (!cfg_file.good()) {
			return false;
		}

		cfg_file << root;
		cfg_file.close();
		return true;
	}
	catch (std::exception &e) {
		return false;
	}
}


}
