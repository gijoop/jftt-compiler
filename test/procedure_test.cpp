#include "compiler_test.hpp"

class ProcedureTest : public CompilerTest {};

TEST_F(ProcedureTest, SimpleProcedureCall) {
  std::string source_code = R"(
    PROCEDURE test() IS
    IN
        WRITE 42;
    END

    PROGRAM IS
    IN
        test();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "42");
}

TEST_F(ProcedureTest, MultipleProcedureCalls) {
  std::string source_code = R"(
    PROCEDURE first() IS
    IN
        WRITE 10;
    END

    PROCEDURE second() IS
    IN
        WRITE 20;
    END

    PROCEDURE third() IS
    IN
        WRITE 30;
    END

    PROGRAM IS
    IN
        first();
        second();
        third();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "10");
  EXPECT_EQ(output[1], "20");
  EXPECT_EQ(output[2], "30");
}

TEST_F(ProcedureTest, ProcedureWithMultipleWrites) {
  std::string source_code = R"(
    PROCEDURE writeseq() IS
    IN
        WRITE 1;
        WRITE 2;
        WRITE 3;
    END

    PROGRAM IS
    IN
        writeseq();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "1");
  EXPECT_EQ(output[1], "2");
  EXPECT_EQ(output[2], "3");
}


TEST_F(ProcedureTest, ProcedureCalledInConditional) {
  std::string source_code = R"(
    PROCEDURE writeyes() IS
    IN
        WRITE 1;
    END

    PROCEDURE writeno() IS
    IN
        WRITE 0;
    END

    PROGRAM IS
    x
    IN
        x := 10;
        IF x > 5 THEN
            writeyes();
        ELSE
            writeno();
        ENDIF
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "1");
}

TEST_F(ProcedureTest, MixedProgramAndProcedure) {
  std::string source_code = R"(
    PROCEDURE middle() IS
    IN
        WRITE 20;
    END

    PROGRAM IS
    IN
        WRITE 10;
        middle();
        WRITE 30;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0], "10");
  EXPECT_EQ(output[1], "20");
  EXPECT_EQ(output[2], "30");
}

TEST_F(ProcedureTest, ProcedureWithArithmetic) {
  std::string source_code = R"(
    PROCEDURE calculate() IS
    a, b, c
    IN
        a := 10;
        b := 5;
        c := a + b;
        WRITE c;
    END

    PROGRAM IS
    IN
        calculate();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "15");
}


TEST_F(ProcedureTest, ProcedureWithConditional) {
  std::string source_code = R"(
    PROCEDURE checkval() IS
    val
    IN
        val := 15;
        IF val > 10 THEN
            WRITE 1;
        ELSE
            WRITE 0;
        ENDIF
    END

    PROGRAM IS
    IN
        checkval();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "1");
}

TEST_F(ProcedureTest, ProcedureWithLoop) {
  std::string source_code = R"(
    PROCEDURE countto5() IS
    i
    IN
        i := 0;
        WHILE i < 5 DO
            i := i + 1;
        ENDWHILE
        WRITE i;
    END

    PROGRAM IS
    IN
        countto5();
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 1);
  EXPECT_EQ(output[0], "5");
}

TEST_F(ProcedureTest, EmptyProcedure) {
  std::string source_code = R"(
    PROCEDURE donothing() IS
    IN
    END

    PROGRAM IS
    IN
        WRITE 1;
        donothing();
        WRITE 2;
    END
  )";

  auto output = compile_and_run(source_code);
  ASSERT_EQ(output.size(), 2);
  EXPECT_EQ(output[0], "1");
  EXPECT_EQ(output[1], "2");
}
