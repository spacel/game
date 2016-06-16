#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>
#include <iostream>

#include "SettingsTests.h"
#include "TimeTests.h"

int main() {
	CppUnit::TextUi::TestRunner runner;

	std::cout << "Creating Test Suites:" << std::endl;
	runner.addTest(spacel::unittests::TimeUnitTest::suite());
	runner.addTest(spacel::unittests::SettingsTest::suite());
	std::cout << "Running the unit tests." << std::endl;
	return runner.run() ? 0 : 1;
}
