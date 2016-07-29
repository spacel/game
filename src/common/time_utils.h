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

#include <chrono>
#include <ctime>
#include <string>

namespace spacel {

inline static uint64_t get_current_time()
{
	return (uint64_t) std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

inline static uint64_t get_current_time_ms()
{
	return (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

inline static std::string timestamp_to_string(const uint64_t &timestamp, bool localtime = true)
{
	std::chrono::system_clock::time_point date = std::chrono::system_clock::from_time_t(timestamp);
	std::time_t t = std::chrono::system_clock::to_time_t(date);
	char mbstr[50];
	if (localtime) {
		std::strftime(mbstr, sizeof(mbstr), "%d/%m/%y %H:%M", std::localtime(&t));
	}
	else {
		std::strftime(mbstr, sizeof(mbstr), "%d/%m/%y %H:%M", std::gmtime(&t));
	}
	return std::string(mbstr);
}
}
