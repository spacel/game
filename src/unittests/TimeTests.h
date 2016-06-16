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

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "../common/time_utils.h"

namespace spacel {
namespace unittests {

class TimeUnitTest : public CppUnit::TestFixture {
private:
public:
	TimeUnitTest() {}
	virtual ~TimeUnitTest() {}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("Time");
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeUnitTest>("Test1 - Current Time.",
				&TimeUnitTest::test_currentTime));

		suiteOfTests->addTest(new CppUnit::TestCaller<TimeUnitTest>("Test2 - Current Time MS.",
				&TimeUnitTest::test_currentTimeMs));

		suiteOfTests->addTest(new CppUnit::TestCaller<TimeUnitTest>("Test3 - Timestamp to string.",
				&TimeUnitTest::test_timestampToString));

		return suiteOfTests;
	}

	/// Setup method
	void setUp() {}

	/// Teardown method
	void tearDown() {}

protected:
	void test_currentTime()
	{
		uint64_t current_time = get_current_time();
		CPPUNIT_ASSERT(current_time > 0);
	}

	void test_currentTimeMs()
	{
		uint64_t current_time_ms = get_current_time_ms();
		CPPUNIT_ASSERT(current_time_ms > 0);
	}

	void test_timestampToString()
	{
		std::string ts_str = timestamp_to_string(1500000, false);
		CPPUNIT_ASSERT(ts_str == "18/01/70 08:40");
	}
};

}
}

