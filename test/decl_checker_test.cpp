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
