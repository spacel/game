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
#include "../common/engine/space.h"

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
				&GeneratorsUnitTest::test_generate_solarsystemradius));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test4 - Generate Solar System Type.",
				&GeneratorsUnitTest::test_generate_solarsystemtype));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test5 - Generate Solar System Planet Number.",
				&GeneratorsUnitTest::test_generate_solarsystem_planetnumber));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test6 - Generate Solar System Planet Type.",
				&GeneratorsUnitTest::test_generate_planettype));

		suiteOfTests->addTest(new CppUnit::TestCaller<GeneratorsUnitTest>("Test7 - Generate Solar System Planet Distance.",
				&GeneratorsUnitTest::test_generate_planetdistance));
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
		std::string name = engine::UnivGen->generate_world_name();
		CPPUNIT_ASSERT(name.length() > 3);
	}

	void test_generate_solarsystemradius()
	{
		engine::UniverseGenerator::SetSeed(44887799);
		double result = engine::UnivGen->generate_solarsystem_radius(
			5448855);
		CPPUNIT_ASSERT(result == 7707902007844.146484);
	}

	void test_generate_solarsystemtype()
	{
		engine::UniverseGenerator::SetSeed(487597);
		uint8_t result = engine::UnivGen->generate_solarsystem_type(12487904);
		CPPUNIT_ASSERT(result == 3);
	}

	void test_generate_solarsystem_planetnumber()
	{
		engine::UniverseGenerator::SetSeed(4487899);
		engine::SolarSystem solar_system;
		solar_system.id = 697;
		solar_system.type = engine::SOLAR_TYPE_BIG_BLUE;
		uint8_t planet_number = engine::UnivGen->
			generate_solarsystem_planetnumber(&solar_system);
		CPPUNIT_ASSERT(planet_number == 2);
	}

	void test_generate_planettype()
	{
		engine::UniverseGenerator::SetSeed(3698598);
		uint8_t planet_type = engine::UnivGen->generate_planet_type(8891178656);
		CPPUNIT_ASSERT(planet_type == 4);
	}

	void test_generate_planetdistance()
	{
		engine::UniverseGenerator::SetSeed(5578824136);
		engine::SolarSystem solar_system;
		solar_system.radius = 1000 * 1000.0f * 1000.0f;
		double planet_distance = engine::UnivGen->
			generate_planet_distance(110599, engine::PLANET_TYPE_EARTH, &solar_system);
		// We should use stringstream as we don't have the correct on-screen output
		// for this test, due to precision
		std::stringstream ss;
		ss << planet_distance;
		CPPUNIT_ASSERT(ss.str() == "7.05024e+08");
	}
};

}
}

