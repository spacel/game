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

#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "../client/settings.h"

namespace spacel {
namespace unittests {

class SettingsTest : public CppUnit::TestFixture {
private:
public:
	SettingsTest() {}
	virtual ~SettingsTest() {}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("Settings");
		suiteOfTests->addTest(new CppUnit::TestCaller<SettingsTest>("Test1 - Bool settings.",
		        &SettingsTest::test_setting_bool));

		suiteOfTests->addTest(new CppUnit::TestCaller<SettingsTest>("Test2 - U32 settings.",
		        &SettingsTest::test_setting_u32));

		suiteOfTests->addTest(new CppUnit::TestCaller<SettingsTest>("Test3 - Float settings.",
		        &SettingsTest::test_setting_float));

		suiteOfTests->addTest(new CppUnit::TestCaller<SettingsTest>("Test4 - Save & Load.",
		        &SettingsTest::test_setting_save_load));

		return suiteOfTests;
	}

	/// Setup method
	void setUp() {}

	/// Teardown method
	void tearDown() {}

protected:
	void test_setting_bool()
	{
		ClientSettings settings;
		bool init_v = true;
		settings.setBool(BSETTING_ENABLE_MUSIC, init_v);
		bool v = settings.getBool(BSETTING_ENABLE_MUSIC);
		CPPUNIT_ASSERT(v == init_v);
	}

	void test_setting_u32()
	{
		ClientSettings settings;
		uint32_t init_v = 800;
		settings.setU32(U32SETTING_WINDOW_HEIGHT, init_v);
		uint32_t v = settings.getU32(U32SETTING_WINDOW_HEIGHT);
		CPPUNIT_ASSERT(v == init_v);
	}

	void test_setting_float()
	{
		ClientSettings settings;
		float init_v = 41.587f;
		settings.setFloat(FLOATSETTINGS_SOUND_AMBIENT_GAIN, init_v);
		float v = settings.getFloat(FLOATSETTINGS_SOUND_AMBIENT_GAIN);
		CPPUNIT_ASSERT(v == init_v);
	}

	void test_setting_save_load()
	{
		ClientSettings settings;
		float init_v = 41.48587f;
		settings.setFloat(FLOATSETTINGS_SOUND_UI_GAIN, init_v);
		settings.save("unittests_settings.json");

		ClientSettings settings_load;
		settings_load.load("unittests_settings.json");
		float v = settings_load.getFloat(FLOATSETTINGS_SOUND_UI_GAIN);
		CPPUNIT_ASSERT(v == init_v);
	}

};

}
}


