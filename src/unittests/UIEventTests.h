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
#include <client/uievents.h>

#include "../common/time_utils.h"

namespace spacel {
namespace unittests {

class UIEventUnitTest : public CppUnit::TestFixture {
private:
public:
	UIEventUnitTest() {}
	virtual ~UIEventUnitTest() {}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("UIEvent");
		suiteOfTests->addTest(new CppUnit::TestCaller<UIEventUnitTest>("Test1 - UIEvent_CharacterAdd_ID.",
				&UIEventUnitTest::test_UIEventCharacterAdd_ID));

		suiteOfTests->addTest(new CppUnit::TestCaller<UIEventUnitTest>("Test2 - test_UIEventCharacterAdd_Var.",
				&UIEventUnitTest::test_UIEventCharacterAdd_Var));

		suiteOfTests->addTest(new CppUnit::TestCaller<UIEventUnitTest>("Test3 - test_UIEventCharacterAdd_SharedPtr.",
				&UIEventUnitTest::test_UIEventCharacterAdd_SharedPtr));

		return suiteOfTests;
	}

	/// Setup method
	void setUp() {}

	/// Teardown method
	void tearDown() {}

protected:
	void test_UIEventCharacterAdd_ID()
	{
		ClientUIEvent_CharacterAdd event;
		event.race = engine::PLAYER_RACE_HUMAN;
		CPPUNIT_ASSERT(event.id == CLIENT_UI_EVENT_CHARACTER_ADD);
	}

	void test_UIEventCharacterAdd_Var()
	{
		ClientUIEvent_CharacterAdd event;
		event.race = engine::PLAYER_RACE_HUMAN;
		CPPUNIT_ASSERT(event.race == engine::PLAYER_RACE_HUMAN);
	}

	void test_UIEventCharacterAdd_SharedPtr()
	{
		ClientUIEventPtr event_ptr(new ClientUIEvent_CharacterRemove());
		ClientUIEvent_CharacterRemove *event = dynamic_cast<ClientUIEvent_CharacterRemove *>(event_ptr.get());
		CPPUNIT_ASSERT(event);
	}

	void test_UIEvent_Queue() {
		const uint64_t test_value = 7;

		ClientUIEventQueue event_queue;
		ClientUIEventPtr event_ptr(new ClientUIEvent_CharacterRemove());
		ClientUIEvent_CharacterRemove *event = dynamic_cast<ClientUIEvent_CharacterRemove *>(event_ptr.get());
		event->guid = test_value;
		event_queue.push_back(event_ptr);

		CPPUNIT_ASSERT(!event_queue.empty());
		ClientUIEventPtr event_ptr_res = event_queue.pop_front();

		ClientUIEvent_CharacterRemove *event_res =
				dynamic_cast<ClientUIEvent_CharacterRemove *>(event_ptr_res.get());
		CPPUNIT_ASSERT(event_res);
		CPPUNIT_ASSERT(event_res->guid == test_value);
	}

};

}
}

