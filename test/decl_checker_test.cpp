#include "compiler_test.hpp"

class DeclCheckerTest : public CompilerTest {};

TEST_F(DeclCheckerTest, VarDeclared) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 0;
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
  } catch (const SemanticError& e) {
    FAIL() << "DeclarationChecker threw an SemanticError for declared variable: " << e.what();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for declared variable.";
  }
}

TEST_F(DeclCheckerTest, VarUndeclared) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        x := 0;
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for undeclared variable.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for undeclared variable.";
  }
}

TEST_F(DeclCheckerTest, OneOfMultipleVarsUndeclared) {
  std::string source_code = R"(
    PROGRAM IS
    a, b
    IN
        a := 10;
        c := 20;
        WRITE a;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for one undeclared variable among multiple.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for one undeclared variable among multiple.";
  }
}

TEST_F(DeclCheckerTest, OneVarDoubleDeclared) {
  std::string source_code = R"(
    PROGRAM IS
    x, y, x
    IN
        x := 5;
        y := 10;
        WRITE x;
        WRITE y;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for double declared variable.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for double declared variable.";
  }
}

TEST_F(DeclCheckerTest, OneVarTripleDeclared) {
  std::string source_code = R"(
    PROGRAM IS
    a, b, a, c, a
    IN
        a := 1;
        b := 2;
        c := 3;
        WRITE a;
        WRITE b;
        WRITE c;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for triple declared variable.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for triple declared variable.";
  }
}

TEST_F(DeclCheckerTest, CallArgumentDeclared) {
  std::string source_code = R"(
    PROCEDURE test(x) IS
    IN
        WRITE x;
    END
  
    PROGRAM IS
    a
    IN
        a := 42;
        test(a);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
  } catch (const SemanticError& e) {
    FAIL() << "DeclarationChecker threw an SemanticError for declared call argument: " << e.what();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for declared call argument.";
  }
}

TEST_F(DeclCheckerTest, CallArgumentUndeclared) {
  std::string source_code = R"(
    PROCEDURE test(x) IS
    IN
        WRITE x;
    END
  
    PROGRAM IS
    IN
        test(undeclared);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for undeclared call argument.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for undeclared call argument.";
  }
}

TEST_F(DeclCheckerTest, CallMultipleArgumentsSomeDeclared) {
  std::string source_code = R"(
    PROCEDURE test(x, y, z) IS
    IN
        WRITE x;
    END
  
    PROGRAM IS
    a, b
    IN
        a := 1;
        b := 2;
        test(a, b, undeclared);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for partially undeclared call arguments.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for partially undeclared call arguments.";
  }
}

TEST_F(DeclCheckerTest, CallMultipleArgumentsAllDeclared) {
  std::string source_code = R"(
    PROCEDURE test(x, y, z) IS
    IN
        WRITE x;
        WRITE y;
        WRITE z;
    END
  
    PROGRAM IS
    a, b, c
    IN
        a := 1;
        b := 2;
        c := 3;
        test(a, b, c);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
  } catch (const SemanticError& e) {
    FAIL() << "DeclarationChecker threw an SemanticError for all declared call arguments: " << e.what();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for all declared call arguments.";
  }
}

TEST_F(DeclCheckerTest, CallArgumentDeclaredInProcedure) {
  std::string source_code = R"(
    PROCEDURE test(y) IS
    IN
        WRITE y;
    END
  
    PROCEDURE outer(x) IS
    IN
        test(x);
    END
  
    PROGRAM IS
    a
    IN
        a := 10;
        outer(a);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
  } catch (const SemanticError& e) {
    FAIL() << "DeclarationChecker threw an SemanticError for call argument declared in procedure: " << e.what();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for call argument declared in procedure.";
  }
}

TEST_F(DeclCheckerTest, CallArgumentUndeclaredInProcedure) {
  std::string source_code = R"(
    PROCEDURE test(y) IS
    IN
        WRITE y;
    END
  
    PROCEDURE outer(x) IS
    IN
        test(undeclared);
    END
  
    PROGRAM IS
    a
    IN
        a := 10;
        outer(a);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "DeclarationChecker did not throw an SemanticError for undeclared call argument in procedure.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for undeclared call argument in procedure.";
  }
}

TEST_F(DeclCheckerTest, CallArgumentLocalVariableDeclared) {
  std::string source_code = R"(
  
    PROCEDURE test(y) IS
    IN
        WRITE y;
    END
  
    PROCEDURE outer(x) IS
    local
    IN
        local := 5;
        test(local);
    END
  
    PROGRAM IS
    a
    IN
        a := 10;
        outer(a);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
  } catch (const SemanticError& e) {
    FAIL() << "DeclarationChecker threw an SemanticError for call argument using local variable: " << e.what();
  } catch (...) {
    FAIL() << "DeclarationChecker threw an unexpected exception type for call argument using local variable.";
  }
}

TEST_F(DeclCheckerTest, ArrayDeclarationValidRange) {
  std::string source_code = R"(
    PROGRAM IS
    arr[10:30]
    IN
        arr[10] := 1;
        arr[30] := 2;
        WRITE arr[10];
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "Threw error for valid array declaration: " << e.what();
  } catch (...) {
    FAIL() << "Threw unexpected exception for valid array declaration.";
  }
}

TEST_F(DeclCheckerTest, ArrayDeclarationSameIndex) {
  std::string source_code = R"(
    PROGRAM IS
    arr[5:5]
    IN
        arr[5] := 42;
        WRITE arr[5];
    END
  )";
  
  try {
    try {
      auto output = compile_and_run(source_code);
      ASSERT_EQ(output.size(), 1);
      EXPECT_EQ(output[0], "42");
    } catch (const SemanticError& e) {
      FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
      FAIL() << "Error: " << e.what();
    }
  } catch (const SemanticError& e) {
    FAIL() << "Threw error for valid single-element array: " << e.what();
  } catch (...) {
    FAIL() << "Threw unexpected exception for valid single-element array.";
  }
}

TEST_F(DeclCheckerTest, ArrayDeclarationInvalidRange) {
  std::string source_code = R"(
    PROGRAM IS
    arr[30:10]
    IN
        arr[10] := 1;
        WRITE arr[10];
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for array with first index > second (30:10).";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for invalid array range.";
  }
}

TEST_F(DeclCheckerTest, ArrayDeclarationInvalidRangeLarge) {
  std::string source_code = R"(
    PROGRAM IS
    arr[100:50]
    IN
        WRITE 1;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for array with first index > second (100:50).";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for invalid array range.";
  }
}

TEST_F(DeclCheckerTest, ArrayDeclarationInvalidRangeInProcedure) {
  std::string source_code = R"(
    PROCEDURE test(x) IS
    arr[20:10]
    IN
        WRITE x;
    END
  
    PROGRAM IS
    a
    IN
        a := 1;
        test(a);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for procedure local array with invalid range.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for procedure local array with invalid range.";
  }
}

TEST_F(DeclCheckerTest, ForIteratorCannotBeModified) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 1 TO 10 DO
            i := 5;
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for modifying FOR iterator inside loop.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for modifying FOR iterator.";
  }
}

TEST_F(DeclCheckerTest, ForIteratorCannotBeModifiedWithExpression) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 1 TO 10 DO
            i := i + 1;
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for modifying FOR iterator with expression.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for modifying FOR iterator.";
  }
}

TEST_F(DeclCheckerTest, ForDowntoIteratorCannotBeModified) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 10 DOWNTO 1 DO
            i := 5;
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for modifying DOWNTO iterator inside loop.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for modifying DOWNTO iterator.";
  }
}

TEST_F(DeclCheckerTest, ForIteratorCannotBeReadInto) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 1 TO 10 DO
            READ i;
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code, {"5"});
    FAIL() << "Did not throw error for READ into FOR iterator.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for READ into FOR iterator.";
  }
}

TEST_F(DeclCheckerTest, NestedForIteratorCannotBeModified) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 1 TO 5 DO
            FOR j FROM 1 TO 5 DO
                j := 10;
            ENDFOR
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for modifying nested FOR iterator.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for modifying nested FOR iterator.";
  }
}

TEST_F(DeclCheckerTest, OuterForIteratorCannotBeModifiedInInnerLoop) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        FOR i FROM 1 TO 5 DO
            FOR j FROM 1 TO 5 DO
                i := 10;
            ENDFOR
        ENDFOR
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    FAIL() << "Did not throw error for modifying outer FOR iterator in inner loop.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type for modifying outer FOR iterator.";
  }
}

TEST_F(DeclCheckerTest, ForIteratorIsLocal) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 100;
        FOR i FROM 1 TO 5 DO
            WRITE i;
        ENDFOR
        WRITE x;
    END
  )";
  
  try {
    try {
      auto output = compile_and_run(source_code);
      ASSERT_EQ(output.size(), 6);
      EXPECT_EQ(output[0], "1");
      EXPECT_EQ(output[1], "2");
      EXPECT_EQ(output[2], "3");
      EXPECT_EQ(output[3], "4");
      EXPECT_EQ(output[4], "5");
      EXPECT_EQ(output[5], "100");
    } catch (const SemanticError& e) {
      FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
      FAIL() << "Error: " << e.what();
    }
  } catch (const SemanticError& e) {
    FAIL() << "Threw error for valid FOR loop with local iterator: " << e.what();
  } catch (...) {
    FAIL() << "Threw unexpected exception for valid FOR loop.";
  }
}

TEST_F(DeclCheckerTest, ForIteratorShadowsVariable) {
  std::string source_code = R"(
    PROGRAM IS
    i
    IN
        i := 100;
        FOR i FROM 1 TO 3 DO
            WRITE i;
        ENDFOR
        WRITE i;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_GE(output.size(), 4);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "2");
    EXPECT_EQ(output[2], "3");
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "Threw unexpected exception type.";
  }
}

