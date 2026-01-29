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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "67");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "65");
    EXPECT_EQ(output[1], "3855");
    EXPECT_EQ(output[2], "78384598");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "12");
    EXPECT_EQ(output[1], "34");
    EXPECT_EQ(output[2], "56");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1234");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "20");
    EXPECT_EQ(output[1], "10");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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

TEST_F(VarTest, VarLargeValue) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 999999999;
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "999999999");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
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
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "5");
    EXPECT_EQ(output[2], "10");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(VarTest, UnderscoreIdentifier) {
  std::string source_code = R"(
    PROGRAM IS
    _x
    IN
        _x := 42;
        WRITE _x;
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

TEST_F(VarTest, MultipleUnderscoresIdentifier) {
  std::string source_code = R"(
    PROGRAM IS
    ___
    IN
        ___ := 77;
        WRITE ___;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "77");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(VarTest, MixedUnderscoreLetterIdentifier) {
  std::string source_code = R"(
    PROGRAM IS
    _my_variable_, another_var
    IN
        _my_variable_ := 100;
        another_var := 200;
        WRITE _my_variable_;
        WRITE another_var;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "100");
    EXPECT_EQ(output[1], "200");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(VarTest, LongIdentifier) {
  std::string source_code = R"(
    PROGRAM IS
    this_is_a_very_long_variable_name
    IN
        this_is_a_very_long_variable_name := 123;
        WRITE this_is_a_very_long_variable_name;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "123");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(VarTest, CaseSensitiveKeywords) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 42;
        WRITE x;
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

TEST_F(VarTest, IdentifierCaseSensitive) {
  std::string source_code = R"(
    PROGRAM IS
    abc, xyz
    IN
        abc := 10;
        xyz := 20;
        WRITE abc;
        WRITE xyz;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}
