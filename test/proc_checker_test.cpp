#include "compiler_test.hpp"

class ProcCheckerTest : public CompilerTest {};

TEST_F(ProcCheckerTest, SimpleProcedureDeclaration) {
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
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "ProcedureChecker threw an SemanticError for valid procedure declaration: " << e.what();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for valid procedure declaration.";
  }
}

TEST_F(ProcCheckerTest, DoubleProcedureDeclaration) {
  std::string source_code = R"(
    PROCEDURE test(x) IS
    IN
        WRITE x;
    END

    PROCEDURE test(y) IS
    IN
        WRITE y;
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
    FAIL() << "ProcedureChecker did not throw an SemanticError for double procedure declaration.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for double procedure declaration.";
  }
}

TEST_F(ProcCheckerTest, CallUndeclaredProcedure) {
  std::string source_code = R"(
    PROGRAM IS
    a
    IN
        a := 42;
        undeclared(a);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    FAIL() << "ProcedureChecker did not throw an SemanticError for call to undeclared procedure.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for call to undeclared procedure.";
  }
}

TEST_F(ProcCheckerTest, RecursiveProcedureCall) {
  std::string source_code = R"(
    PROCEDURE factorial(n) IS
    IN
        factorial(n);
    END

    PROGRAM IS
    x
    IN
        x := 5;
        factorial(x);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    FAIL() << "ProcedureChecker did not throw an SemanticError for recursive procedure call.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for recursive procedure call.";
  }
}

TEST_F(ProcCheckerTest, IncorrectNumberOfArguments_TooFew) {
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
        test(a, b);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    FAIL() << "ProcedureChecker did not throw an SemanticError for too few arguments.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for too few arguments.";
  }
}

TEST_F(ProcCheckerTest, IncorrectNumberOfArguments_TooMany) {
  std::string source_code = R"(
    PROCEDURE test(x) IS
    IN
        WRITE x;
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
    FAIL() << "ProcedureChecker did not throw an SemanticError for too many arguments.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for too many arguments.";
  }
}

TEST_F(ProcCheckerTest, CorrectNumberOfArguments_Multiple) {
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
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "ProcedureChecker threw an SemanticError for procedure with correct number of arguments: " << e.what();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for procedure with correct number of arguments.";
  }
}

TEST_F(ProcCheckerTest, MultipleProceduresWithDifferentArgumentCounts) {
  std::string source_code = R"(
    PROCEDURE proc_a(x) IS
    IN
        WRITE x;
    END

    PROCEDURE proc_b(x, y) IS
    IN
        WRITE x;
        WRITE y;
    END

    PROCEDURE proc_c(x, y, z) IS
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
        proc_a(a);
        proc_b(a, b);
        proc_c(a, b, c);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "ProcedureChecker threw an SemanticError for multiple procedures with different argument counts: " << e.what();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for multiple procedures with different argument counts.";
  }
}

TEST_F(ProcCheckerTest, ProcedureCalledBeforeDeclaration) {
  std::string source_code = R"(
    PROCEDURE first(x) IS
    IN
        second(x);
    END

    PROCEDURE second(y) IS
    IN
        WRITE y;
    END

    PROGRAM IS
    a
    IN
        a := 42;
        first(a);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    FAIL() << "ProcedureChecker did not throw an SemanticError for procedure called before declaration.";
  } catch (const SemanticError& e) {
    SUCCEED();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for procedure called before declaration.";
  }
}

TEST_F(ProcCheckerTest, MultipleProceduresAllCalledCorrectly) {
  std::string source_code = R"(
    PROCEDURE proc_a(x) IS
    IN
        WRITE 1;
    END

    PROCEDURE proc_b(x) IS
    IN
        WRITE 2;
    END

    PROCEDURE proc_c(x, y) IS
    IN
        WRITE 3;
    END

    PROGRAM IS
    a, b
    IN
        a := 10;
        b := 20;
        proc_a(a);
        proc_b(a);
        proc_c(a, b);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "ProcedureChecker threw an SemanticError for multiple procedures called correctly: " << e.what();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for multiple procedures called correctly.";
  }
}

TEST_F(ProcCheckerTest, NestedProcedureCallsCorrectArguments) {
  std::string source_code = R"(
    PROCEDURE inner(x) IS
    IN
        WRITE x;
    END

    PROCEDURE outer(y) IS
    IN
        inner(y);
    END

    PROGRAM IS
    a
    IN
        a := 100;
        outer(a);
    END
  )";

  try {
    auto output = compile_and_run(source_code);
    SUCCEED();
  } catch (const SemanticError& e) {
    FAIL() << "ProcedureChecker threw an SemanticError for nested procedure calls with correct arguments: " << e.what();
  } catch (...) {
    FAIL() << "ProcedureChecker threw an unexpected exception type for nested procedure calls with correct arguments.";
  }
}
