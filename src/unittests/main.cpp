#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>
#include <iostream>
#include <common/engine/generators.h>

#include "SettingsTests.h"
#include "TimeTests.h"
#include "GeneratorsTests.h"
#include "UIEventTests.h"

spacel::engine::UniverseGenerator *spacel::engine::UniverseGenerator::s_univgen = nullptr;
uint64_t spacel::engine::UniverseGenerator::s_seed = 0;

int main() {
	CppUnit::TextUi::TestRunner runner;

	std::cout << "Creating Test Suites:" << std::endl;
	runner.addTest(spacel::unittests::TimeUnitTest::suite());
	runner.addTest(spacel::unittests::SettingsTest::suite());
	runner.addTest(spacel::unittests::GeneratorsUnitTest::suite());
	runner.addTest(spacel::unittests::UIEventUnitTest::suite());
	std::cout << "Running the unit tests." << std::endl;
	return runner.run() ? 0 : 1;
}
