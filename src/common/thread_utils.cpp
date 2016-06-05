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

Thread::Thread()
{
	retval = NULL;
	requeststop = false;
	running = false;
	started = false;
}

Thread::~Thread()
{
	kill();
}

void Thread::wait() {
	if (started) {
		if (m_thread) {
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
		started = false;
	}
}

const bool Thread::Start()
{
	if (running) {
		return false;
	}

	requeststop = false;

	continuemutex.lock();
	m_thread = new std::thread(TheThread, this);
	if (!m_thread) {
		continuemutex.unlock();
		return false;
	}

	/* Wait until 'running' is set */

	while (!running) {
#ifdef _WIN32
		Sleep(1);
#else
		struct timespec req,rem;
		req.tv_sec = 0;
		req.tv_nsec = 1000000;
		nanosleep(&req,&rem);
#endif
	}
	started = true;

	continuemutex.unlock();

	continuemutex2.lock();
	continuemutex2.unlock();
	return true;
}

int Thread::kill()
{
	if (!running)
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
	running = false;

	return 0;
}

void * Thread::TheThread(void *data)
{
	Thread *thread = (Thread *)data;

	thread->continuemutex2.lock();
	thread->running = true;

	thread->continuemutex.lock();
	thread->continuemutex.unlock();

	thread->run();

	thread->running = false;
	return NULL;
}

void Thread::ThreadStarted()
{
	continuemutex2.unlock();
}


}
