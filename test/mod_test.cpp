#include "compiler_test.hpp"

class ModTest : public CompilerTest {};

TEST_F(ModTest, SimpleMod) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 10;
        b := 3;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModByOne) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 100;
        b := 1;
        c := a % b;
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

TEST_F(ModTest, ModZeroByNumber) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 10;
        c := a % b;
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

TEST_F(ModTest, ModExactDivision) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 7;
        c := a % b;
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

TEST_F(ModTest, ModSmallerByLarger) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 5;
        b := 10;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "5");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModSameNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 42;
        b := 42;
        c := a % b;
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

TEST_F(ModTest, ModByTwo) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 17;
        b := 2;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModEvenCheck) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 18;
        b := 2;
        c := a % b;
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

TEST_F(ModTest, ModLargeNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 123456;
        b := 100;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "56");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}


TEST_F(ModTest, ModMultipleOperations) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c, d
    IN
        a := 25;
        b := 7;
        c := a % b;
        d := b % c;
        WRITE c;
        WRITE d;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "4");
    EXPECT_EQ(output[1], "3");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModByTen) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 12345;
        b := 10;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "5");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModExtractDigits) {
  std::string source_code = R"(
    PROGRAM IS
    num, digit_a, digit_b, digit_c
    IN
        num := 456;
        digit_a := num % 10;
        num := num / 10;
        digit_b := num % 10;
        num := num / 10;
        digit_c := num % 10;
        WRITE digit_a;
        WRITE digit_b;
        WRITE digit_c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "6");
    EXPECT_EQ(output[1], "5");
    EXPECT_EQ(output[2], "4");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModGCD) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, temp
    IN
        a := 48;
        b := 18;
        WHILE b != 0 DO
            temp := b;
            b := a % b;
            a := temp;
        ENDWHILE
        WRITE a;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "6");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModPowerOfTwo) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 100;
        b := 8;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "4");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModConsecutiveNumbers) {
  std::string source_code = R"(
    PROGRAM IS
    i, wr
    IN
        i := 0;
        WHILE i < 5 DO
            wr := i % 3;
            WRITE wr;
            i := i + 1;
        ENDWHILE
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 5);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "2");
    EXPECT_EQ(output[3], "0");
    EXPECT_EQ(output[4], "1");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ModTest, ModByZeroLiteral) {
  std::string source_code = R"(
    PROGRAM IS
    a, c
    IN
        a := 42;
        c := a % 0;
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

TEST_F(ModTest, ModByZeroVariable) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 100;
        b := 0;
        c := a % b;
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

TEST_F(ModTest, ModZeroByZero) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 0;
        b := 0;
        c := a % b;
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

TEST_F(ModTest, ModByZeroLargeNumber) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, c
    IN
        a := 123456789;
        b := 0;
        c := a % b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "0");  // Modulo by zero should return 0
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}
