#include "compiler_test.hpp"

class AddTest : public CompilerTest {};

TEST_F(AddTest, SimpleAdd) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 10;
        b := 32;
        c := a + b;
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

TEST_F(AddTest, AddZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 58;
        c := a + b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "58");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(AddTest, MultipleAdds) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 5;
        b := 15;
        c := 25;
        d := a + b;
        d := d + c;
        WRITE d;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "45");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(AddTest, AddLargeNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 123456;
        b := 654321;
        c := a + b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "777777");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(AddTest, AddZeroToZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 0;
        c := a + b;
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

TEST_F(AddTest, AddInLoop) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, sum
    IN
        a := 0;
        b := 5;
        sum := 0;
        WHILE a < b DO
            sum := sum + a;
            a := a + 1;
        ENDWHILE
        WRITE sum;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "10");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(AddTest, AddWithOne) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 99;
        b := a + 1;
        WRITE b;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "100");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}