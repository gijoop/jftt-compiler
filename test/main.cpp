#include "gtest/gtest.h"
#include "test_printer.hpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new CustomPrinter);

    return RUN_ALL_TESTS();
}