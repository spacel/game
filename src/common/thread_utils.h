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

#include <thread>
#include <atomic>
#include <mutex>
#include "macro_utils.h"

namespace spacel {

class Thread
{
public:
	Thread(): m_is_started(false), m_is_running(false), m_stop_requested(false) {}
	virtual ~Thread();
	const bool Start();
	inline virtual void stop() { m_stop_requested = true; }
	int kill();
	virtual void *run() = 0;
	inline bool IsRunning() const { return m_is_running; }
	inline bool StopRequested() const { return m_stop_requested; }

	/*
	 * Wait for thread to finish
	 * Note: this does not stop a thread you have to do this on your own
	 * WARNING: never ever call this on a thread not started or already killed!
	 */
	void wait();
	inline void stop_and_wait() { stop(); wait(); }

private:

	static void *TheThread(void *data);

	std::thread *m_thread = nullptr;

	/*
	 * reading and writing bool values is atomic on all relevant architectures
	 * ( x86 + arm ). No need to waste time for locking here.
	 * once C++11 is supported we can tell compiler to handle cpu caches correct
	 * too. This should cause additional improvement (and silence thread
	 * concurrency check tools.
	 */
	std::atomic_bool m_is_started;
	/*
	 * reading and writing bool values is atomic on all relevant architectures
	 * ( x86 + arm ). No need to waste time for locking here.
	 * once C++11 is supported we can tell compiler to handle cpu caches correct
	 * too. This should cause additional improvement (and silence thread
	 * concurrency check tools.
	 */
	std::atomic_bool m_is_running;
	std::atomic_bool m_stop_requested;

	DISABLE_CLASS_COPY(Thread);
};

}
