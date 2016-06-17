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

#include "../common/engine/generators.h"

namespace spacel {
namespace unittests {

class GeneratorsUnitTest : public CppUnit::TestFixture {
private:
public:
	GeneratorsUnitTest() {}
	virtual ~GeneratorsUnitTest() {}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("Generators");
		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test1 - Generate Seed.",
				&GeneratorsUnitTest::test_generate_seed));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test2 - Generate World Name.",
				&GeneratorsUnitTest::test_generate_worldname));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test3 - Generate Solar System Double.",
				&GeneratorsUnitTest::test_generate_solarsystemdouble));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test4 - Generate Solar System Type.",
				&GeneratorsUnitTest::test_generate_solarsystemtype));
		return suiteOfTests;
	}

	/// Setup method
	void setUp() {}

	/// Teardown method
	void tearDown() {}

protected:
	void test_generate_seed()
	{
		uint64_t seed = engine::UniverseGenerator::generate_seed();
		CPPUNIT_ASSERT(seed > 0);
	}

	void test_generate_worldname()
	{
		engine::UniverseGenerator::SetSeed(engine::UniverseGenerator::generate_seed());
		std::string name = engine::UniverseGenerator::instance()->generate_world_name();
		CPPUNIT_ASSERT(name.length() > 3);
	}

	void test_generate_solarsystemdouble()
	{
		engine::UniverseGenerator::SetSeed(44887799);
		double result = engine::UniverseGenerator::instance()->generate_solarsystem_double(5448855);
		CPPUNIT_ASSERT(result == 6075898096.2088031769);
	}

	void test_generate_solarsystemtype()
	{
		engine::UniverseGenerator::SetSeed(487597);
		uint8_t result = engine::UniverseGenerator::instance()->generate_solarsystem_type(12487904);
		CPPUNIT_ASSERT(result == 3);
	}
};

}
}

