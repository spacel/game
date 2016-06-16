#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>
#include <iostream>

#include "TimeTests.h"

int main() {
	CppUnit::TextUi::TestRunner runner;

	std::cout << "Creating Test Suites:" << std::endl;
	runner.addTest(spacel::unittests::TimeUnitTest::suite());
	std::cout << "Running the unit tests." << std::endl;
	runner.run();
	return 0;
}
