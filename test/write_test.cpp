#include "compiler_test.hpp"

class WriteTest : public CompilerTest {};

TEST_F(WriteTest, SimpleWrite1) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 0;
    END
  )";

  auto output = compile_and_run(source_code);
  EXPECT_EQ(output[0], "0");
}

TEST_F(WriteTest, SimpleWrite2) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 42;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "42");
}

TEST_F(WriteTest, SimpleWrite3) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 7;
        WRITE 15;
        WRITE 23;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "7");
  EXPECT_EQ(output[1], "15");
  EXPECT_EQ(output[2], "23");
}

TEST_F(WriteTest, WriteLargeNumber) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 123456789;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "123456789");
}

TEST_F(WriteTest, WriteVariable) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 999;
        WRITE x;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "999");
}

TEST_F(WriteTest, WriteManyValues) {
  std::string source_code = R"(
    PROGRAM IS
    i
    IN
        i := 0;
        WHILE i < 10 DO
            WRITE i;
            i := i + 1;
        ENDWHILE
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 10);
  EXPECT_EQ(output[0], "0");
  EXPECT_EQ(output[9], "9");
}
