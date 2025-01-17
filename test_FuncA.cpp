#include <gtest/gtest.h>
#include "FuncA.h"

class FuncATest : public ::testing::Test {
protected:
    FuncA func;
};

// Test case 1: Small `n` values, e.g., `n = 1`
TEST_F(FuncATest, SmallN) {
    EXPECT_NEAR(func.calculate(1.0, 1), 1.0, 1e-6);
    EXPECT_NEAR(func.calculate(1.0, 2), 0.5, 1e-6);
}

// Test case 2: Negative x
TEST_F(FuncATest, NegativeX) {
    EXPECT_NEAR(func.calculate(-1.0, 3), -1.833333, 1e-6); // -1 - 0.5 - 0.333...
}

// Test case 3: Edge case `n = 0`
TEST_F(FuncATest, EdgeCaseZeroTerms) {
    EXPECT_DOUBLE_EQ(func.calculate(1.0, 0), 0.0);
}

// Test case 4: Edge case `x = 0`
TEST_F(FuncATest, EdgeCaseZeroX) {
    EXPECT_DOUBLE_EQ(func.calculate(0.0, 5), 0.0);
}

