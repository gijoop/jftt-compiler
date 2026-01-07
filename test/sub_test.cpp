#include "compiler_test.hpp"

class SubTest : public CompilerTest {};

TEST_F(SubTest, SimpleSub) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 50;
        b := 8;
        c := a - b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "42");
}

TEST_F(SubTest, SubToZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 58;
        b := 58;
        c := a - b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "0");
}

TEST_F(SubTest, MultipleSubs) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 100;
        b := 30;
        c := 20;
        d := a - b;
        d := d - c;
        WRITE d;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "50");
}

TEST_F(SubTest, SubLargerFromSmaller) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 30;
        b := 75;
        c := a - b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "0");
}

TEST_F(SubTest, SubZeroFromNumber) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 0;
        c := a - b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "42");
}

TEST_F(SubTest, SubLargeNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 999999;
        b := 123456;
        c := a - b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "876543");
}

TEST_F(SubTest, SubInLoop) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 10;
        b := 0;
        WHILE a > b DO
            a := a - 1;
        ENDWHILE
        WRITE a;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "0");
}

TEST_F(SubTest, SubDecrement) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 100;
        b := a - 1;
        WRITE b;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "99");
}

TEST_F(SubTest, ChainedSubtractions) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d, result
    IN
        a := 100;
        b := 10;
        c := 20;
        d := 30;
        result := a - b;
        result := result - c;
        result := result - d;
        WRITE result;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "40");
}