#include "compiler_test.hpp"

class DivTest : public CompilerTest {};

TEST_F(DivTest, SimpleDiv) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 6;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "7");
}

TEST_F(DivTest, DivByOne) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 100;
        b := 1;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "100");
}

TEST_F(DivTest, DivZeroByNumber) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 10;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "0");
}

TEST_F(DivTest, DivSameNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 42;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "1");
}

TEST_F(DivTest, DivWithTruncation) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 10;
        b := 3;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "3");
}

TEST_F(DivTest, MultipleDivs) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 100;
        b := 5;
        c := 2;
        d := a / b;
        d := d / c;
        WRITE d;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "10");
}

TEST_F(DivTest, DivLargeNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 999999;
        b := 999;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "1001");
}

TEST_F(DivTest, DivSmallerByLarger) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 5;
        b := 10;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "0");
}

TEST_F(DivTest, DivPowersOfTwo) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 64;
        b := 2;
        c := a / b;
        c := c / b;
        c := c / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "8");
}

TEST_F(DivTest, DivInLoop) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 1000;
        b := 0;
        WHILE a > 0 DO
            a := a / 2;
            b := b + 1;
        ENDWHILE
        WRITE b;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "10");
}

TEST_F(DivTest, DivAndMul) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 20;
        b := 4;
        c := a / b;
        d := c * b;
        WRITE d;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "20");
}

TEST_F(DivTest, DivWithRemainderCheck) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, quotient, product, remainder
    IN
        a := 23;
        b := 5;
        quotient := a / b;
        product := quotient * b;
        remainder := a - product;
        WRITE quotient;
        WRITE remainder;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 2);
  EXPECT_EQ(output[0], "4");
  EXPECT_EQ(output[1], "3");
}

TEST_F(DivTest, DivFactorialDivision) {
  std::string source_code = R"(
    PROGRAM IS
    n, fact, i
    IN
        n := 5;
        fact := 1;
        i := 1;
        WHILE i <= n DO
            fact := fact * i;
            i := i + 1;
        ENDWHILE
        fact := fact / 5;
        fact := fact / 4;
        WRITE fact;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "6");
}

TEST_F(DivTest, DivInNestedLoops) {
  std::string source_code = R"(
    PROGRAM IS
    i, j, result
    IN
        i := 100;
        result := 0;
        WHILE i > 1 DO
            j := i;
            WHILE j > 1 DO
                j := j / 2;
            ENDWHILE
            i := i / 2;
            result := result + 1;
        ENDWHILE
        WRITE result;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "6");
}

TEST_F(DivTest, DivWithLargeQuotient) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 1000000;
        b := 10;
        c := a / b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "100000");
}
