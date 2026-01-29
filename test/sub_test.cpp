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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "42");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "50");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "42");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "876543");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "99");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "40");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(SubTest, SubLargerFromSmallerMultipleTimes) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 10;
        b := 100;
        c := a - b;
        d := c - 50;
        WRITE c;
        WRITE d;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(SubTest, SubOneFromZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 0;
        b := a - 1;
        WRITE b;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(SubTest, SubInLoopWithProperTermination) {
  std::string source_code = R"(
    PROGRAM IS
    a, count
    IN
        a := 5;
        count := 0;
        WHILE a > 0 DO
            a := a - 1;
            count := count + 1;
        ENDWHILE
        WRITE count;
        WRITE a;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "5");
    EXPECT_EQ(output[1], "0");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

