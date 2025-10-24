#include <gtest/gtest.h>
#include <iostream>

// Listener personalizado para mostrar información sobre los tests
class TestProgressListener : public ::testing::EmptyTestEventListener {
	void OnTestProgramStart(const ::testing::UnitTest& unit_test) override {
		std::cout << "\n";
		std::cout << "========================================\n";
		std::cout << "PGRenderCore Test Suite\n";
		std::cout << "========================================\n";
		std::cout << "Total tests: " << unit_test.total_test_case_count() << "\n";
		std::cout << "========================================\n\n";
	}

	void OnTestStart(const ::testing::TestInfo& test_info) override {
		std::cout << "[ RUNNING  ] "
			<< test_info.test_suite_name() << "."
			<< test_info.name() << std::endl;
	}

	void OnTestEnd(const ::testing::TestInfo& test_info) override {
		if (test_info.result()->Passed()) {
			std::cout << "[ PASSED   ] ";
		}
		else {
			std::cout << "[ FAILED   ] ";
		}
		std::cout << test_info.test_suite_name() << "."
			<< test_info.name()
			<< " (" << test_info.result()->elapsed_time() << " ms)\n";
	}

	void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override {
		std::cout << "\n========================================\n";
		std::cout << "Test Results Summary\n";
		std::cout << "========================================\n";
		std::cout << "Total tests: " << unit_test.total_test_count() << "\n";
		std::cout << "Passed: " << unit_test.successful_test_count() << "\n";
		std::cout << "Failed: " << unit_test.failed_test_count() << "\n";
		std::cout << "Time: " << unit_test.elapsed_time() << " ms\n";
		std::cout << "========================================\n";
	}
};

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	// Añadir listener personalizado
	//::testing::TestEventListeners& listeners =
	//	::testing::UnitTest::GetInstance()->listeners();

	// Remover el listener por defecto (opcional, para formato más limpio)
	//delete listeners.Release(listeners.default_result_printer());

	// Añadir nuestro listener personalizado
	//listeners.Append(new TestProgressListener);

	auto result = RUN_ALL_TESTS();

	std::cout << "========================================\n";
	std::cout << "========================================\n";
#ifdef PGRENDER_USE_GLFW
	std::cout << "USING the GLWF backend\n";
#elif PGRENDER_USE_SDL3
	std::cout << "Using the SDL3 backend\n";
#elif
#error "No window backend defined"
#endif
	std::cout << "========================================\n";
	std::cout << "========================================\n";


	return result;
}
