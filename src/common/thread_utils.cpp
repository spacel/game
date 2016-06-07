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

#include "thread_utils.h"

namespace spacel {

Thread::~Thread()
{
	kill();
}

void Thread::wait() {
	if (m_is_started) {
		if (m_thread) {
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
		m_is_started = false;
	}
}

const bool Thread::Start()
{
	if (m_is_running) {
		return false;
	}

	m_stop_requested = false;

	m_thread = new std::thread(TheThread, this);
	if (!m_thread) {
		return false;
	}

	/* Wait until 'running' is set */

	while (!m_is_running) {
#ifdef _WIN32
		Sleep(1);
#else
		struct timespec req,rem;
		req.tv_sec = 0;
		req.tv_nsec = 1000000;
		nanosleep(&req,&rem);
#endif
	}
	m_is_started = true;
	return true;
}

int Thread::kill()
{
	if (!m_is_running)
	{
		wait();
		return 1;
	}

	if (m_thread) {
#ifdef _WIN32
		TerminateThread(m_thread->native_handle(), 0);
		CloseHandle(m_thread->native_handle());
#else
		// We need to pthread_kill instead on Android since NDKv5's pthread
		// implementation is incomplete.
#ifdef __ANDROID__
		pthread_kill(m_thread->native_handle(), SIGKILL);
#else
		pthread_cancel(m_thread->native_handle());
#endif
#endif
	}

	wait();
	m_is_running = false;

	return 0;
}

void * Thread::TheThread(void *data)
{
	Thread *thread = (Thread *)data;

	thread->m_is_running = true;
	thread->run();
	thread->m_is_running = false;
	return NULL;
}


}
