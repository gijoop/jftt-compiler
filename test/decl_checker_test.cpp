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