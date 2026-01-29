#include "compiler_test.hpp"

class MulTest : public CompilerTest {};

TEST_F(MulTest, SimpleMul) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 6;
        b := 7;
        c := a * b;
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

TEST_F(MulTest, MulByZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 58;
        c := a * b;
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

TEST_F(MulTest, MultipleMuls) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 2;
        b := 3;
        c := 7;
        d := a * b;
        d := d * c;
        WRITE d;
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

TEST_F(MulTest, MulLargeNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 1234;
        b := 5678;
        c := a * b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "7006652");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(MulTest, MulByOne) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 1;
        c := a * b;
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

TEST_F(MulTest, MulZeroByZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 0;
        c := a * b;
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

TEST_F(MulTest, MulPowersOfTwo) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 2;
        b := a * a;
        c := b * b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "16");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(MulTest, MulInLoop) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, result
    IN
        a := 5;
        b := 1;
        result := 1;
        WHILE b <= a DO
            result := result * 2;
            b := b + 1;
        ENDWHILE
        WRITE result;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "32");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(MulTest, MulFactorial) {
  std::string source_code = R"(
    PROGRAM IS
    n, i, result
    IN
        n := 5;
        i := 1;
        result := 1;
        WHILE i <= n DO
            result := result * i;
            i := i + 1;
        ENDWHILE
        WRITE result;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "120");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}