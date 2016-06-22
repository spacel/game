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

#include <queue>
#include <mutex>

namespace spacel {

template <typename T>
class SafeQueue
{
public:
	SafeQueue() {}
	~SafeQueue() {}

	const bool empty()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	void push_back(T t)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push_back(t);
	}

	T pop_front()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty()) {
			return T();
		}

		T r = m_queue.front();
		m_queue.pop_front();
		return r;
	}

	size_t size()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.size();
	}

private:
	std::mutex m_mutex;
	std::deque<T> m_queue;
};
}
