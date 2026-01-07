#include "compiler_test.hpp"

class VarTest : public CompilerTest {};

TEST_F(VarTest, VarZero) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 0;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  EXPECT_EQ(output[0], "0");
}

TEST_F(VarTest, VarNonZero) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 67;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "67");
}

TEST_F(VarTest, VarMultipleWrites) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 65;
        WRITE x;
        x := 3855;
        WRITE x;
        x := 78384598;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "65");
  EXPECT_EQ(output[1], "3855");
  EXPECT_EQ(output[2], "78384598");
}

TEST_F(VarTest, MultipleVars) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 12;
        b := 34;
        c := 56;
        WRITE a;
        WRITE b;
        WRITE c;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "12");
  EXPECT_EQ(output[1], "34");
  EXPECT_EQ(output[2], "56");
}

TEST_F(VarTest, VarToVarAssignment) {
  std::string source_code = R"(
    PROGRAM IS
    x, y
    IN
        x := 1234;
        y := x;
        WRITE y;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "1234");
}

TEST_F(VarTest, VarChainedAssignment) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 100;
        b := a;
        c := b;
        d := c;
        WRITE d;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "100");
}

TEST_F(VarTest, VarSwap) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, temp
    IN
        a := 10;
        b := 20;
        temp := a;
        a := b;
        b := temp;
        WRITE a;
        WRITE b;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 2);
  EXPECT_EQ(output[0], "20");
  EXPECT_EQ(output[1], "10");
}

TEST_F(VarTest, VarReassignment) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 10;
        x := 20;
        x := 30;
        x := 40;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "40");
}

TEST_F(VarTest, VarLargeValue) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 999999999;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "999999999");
}

TEST_F(VarTest, ManyVariables) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d, e, f, g, h, i, j
    IN
        a := 1;
        b := 2;
        c := 3;
        d := 4;
        e := 5;
        f := 6;
        g := 7;
        h := 8;
        i := 9;
        j := 10;
        WRITE a;
        WRITE e;
        WRITE j;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "1");
  EXPECT_EQ(output[1], "5");
  EXPECT_EQ(output[2], "10");
}