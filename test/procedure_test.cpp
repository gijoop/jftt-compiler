#include "compiler_test.hpp"

class ProcedureTest : public CompilerTest {};

TEST_F(ProcedureTest, SimpleProcedureCall) {
  std::string source_code = R"(
    PROCEDURE test(dummy) IS
    IN
        WRITE 42;
    END
  
    PROGRAM IS
    x
    IN
        x:=0;
        test(x);
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

TEST_F(ProcedureTest, MultipleProcedureCalls) {
  std::string source_code = R"(
    PROCEDURE first(dummy) IS
    IN
        WRITE 10;
    END
  
    PROCEDURE second(dummy) IS
    IN
        WRITE 20;
    END
  
    PROCEDURE third(dummy) IS
    IN
        WRITE 30;
    END
  
    PROGRAM IS
    x
    IN
        x := 0;
        first(x);
        second(x);
        third(x);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
    EXPECT_EQ(output[2], "30");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ProcedureWithMultipleWrites) {
  std::string source_code = R"(
    PROCEDURE writeseq(dummy) IS
    IN
        WRITE 1;
        WRITE 2;
        WRITE 3;
    END
  
    PROGRAM IS
    x
    IN
        x := 0;
        writeseq(x);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "2");
    EXPECT_EQ(output[2], "3");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}


TEST_F(ProcedureTest, ProcedureCalledInConditional) {
  std::string source_code = R"(
    PROCEDURE writeyes(dummy) IS
    IN
        WRITE 1;
    END
  
    PROCEDURE writeno(dummy) IS
    IN
        WRITE 0;
    END
  
    PROGRAM IS
    x
    IN
        x := 10;
        IF x > 5 THEN
            writeyes(x);
        ELSE
            writeno(x);
        ENDIF
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

TEST_F(ProcedureTest, MixedProgramAndProcedure) {
  std::string source_code = R"(
    PROCEDURE middle(dummy) IS
    IN
        WRITE 20;
    END
  
    PROGRAM IS
    x
    IN
        x := 0;
        WRITE 10;
        middle(x);
        WRITE 30;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
    EXPECT_EQ(output[2], "30");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ProcedureWithArithmetic) {
  std::string source_code = R"(
    PROCEDURE calculate(dummy) IS
    a, b, c
    IN
        a := 10;
        b := 5;
        c := a + b;
        WRITE c;
    END
  
    PROGRAM IS
    x
    IN
        x := 0;
        calculate(x);
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "15");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}


TEST_F(ProcedureTest, ProcedureWithConditional) {
  std::string source_code = R"(
    PROCEDURE checkval(dummy) IS
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
    x
    IN
        x := 0;
        checkval(x);
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

TEST_F(ProcedureTest, ProcedureWithLoop) {
  std::string source_code = R"(
    PROCEDURE counttofive(dummy) IS
    i
    IN
        i := 0;
        WHILE i < 5 DO
            i := i + 1;
        ENDWHILE
        WRITE i;
    END
  
    PROGRAM IS
    x
    IN
        x := 0;
        counttofive(x);
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

// ============================================================================
// PARAMETER PASSING TESTS (Pass-by-Reference)
// ============================================================================

TEST_F(ProcedureTest, SingleParameterModification) {
  std::string source_code = R"(
    PROCEDURE setfive(x) IS
    IN
        x := 5;
    END
  
    PROGRAM IS
    val
    IN
        val := 10;
        WRITE val;
        setfive(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "10");  // Before call
    EXPECT_EQ(output[1], "5");   // After call - modified by reference
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ParameterAddition) {
  std::string source_code = R"(
    PROCEDURE addten(x) IS
    IN
        x := x + 10;
    END
  
    PROGRAM IS
    val
    IN
        val := 5;
        addten(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "15");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, MultipleParameterSwap) {
  std::string source_code = R"(
    PROCEDURE swap(a, b) IS
    temp
    IN
        temp := a;
        a := b;
        b := temp;
    END
  
    PROGRAM IS
    x, y
    IN
        x := 10;
        y := 20;
        WRITE x;
        WRITE y;
        swap(x, y);
        WRITE x;
        WRITE y;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "10");  // x before
    EXPECT_EQ(output[1], "20");  // y before
    EXPECT_EQ(output[2], "20");  // x after swap
    EXPECT_EQ(output[3], "10");  // y after swap
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ParameterArithmetic) {
  std::string source_code = R"(
    PROCEDURE double(x) IS
    IN
        x := x * 2;
    END
  
    PROGRAM IS
    val
    IN
        val := 7;
        double(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "14");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, NestedProcedureCalls) {
  std::string source_code = R"(
    PROCEDURE addtwo(x) IS
    IN
        x := x + 2;
    END
  
    PROCEDURE addfour(x) IS
    IN
        addtwo(x);
        addtwo(x);
    END
  
    PROGRAM IS
    val
    IN
        val := 10;
        addfour(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "14");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ProcedureCallsAnotherWithParameter) {
  std::string source_code = R"(
    PROCEDURE addone(x) IS
    IN
        x := x + 1;
    END
  
    PROCEDURE addten(y) IS
    IN
        y := y + 10;
    END
  
    PROCEDURE combined(z) IS
    IN
        addone(z);
        addten(z);
    END
  
    PROGRAM IS
    val
    IN
        val := 5;
        combined(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "16");  // 5 + 1 + 10
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ParameterInCondition) {
  std::string source_code = R"(
    PROCEDURE setbycondition(x) IS
    IN
        IF x > 10 THEN
            x := 100;
        ELSE
            x := 1;
        ENDIF
    END
  
    PROGRAM IS
    a, b
    IN
        a := 5;
        b := 15;
        setbycondition(a);
        setbycondition(b);
        WRITE a;
        WRITE b;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "1");    // a was 5, set to 1
    EXPECT_EQ(output[1], "100");  // b was 15, set to 100
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ParameterInLoop) {
  std::string source_code = R"(
    PROCEDURE multiplybyloop(result, factor, count) IS
    i
    IN
        result := 0;
        i := 0;
        WHILE i < count DO
            result := result + factor;
            i := i + 1;
        ENDWHILE
    END
  
    PROGRAM IS
    res, x, y
    IN  
        x := 7;
        y := 4;
        multiplybyloop(res, x, y);
        WRITE res;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "28");  // 7 * 4
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ThreeParameterComputation) {
  std::string source_code = R"(
    PROCEDURE compute(a, b, c) IS
    IN
        c := a + b;
        a := c * 2;
        b := c - 5;
    END
  
    PROGRAM IS
    x, y, z
    IN
        x := 10;
        y := 20;
        z := 0;
        compute(x, y, z);
        WRITE x;
        WRITE y;
        WRITE z;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "60");  // (10+20)*2 = 60
    EXPECT_EQ(output[1], "25");  // 30-5 = 25
    EXPECT_EQ(output[2], "30");  // 10+20 = 30
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, ParameterPassingChain) {
  std::string source_code = R"(
    PROCEDURE third(z) IS
    IN
        z := z + 100;
    END
  
    PROCEDURE second(y) IS
    IN
        y := y + 10;
        third(y);
    END
  
    PROCEDURE first(x) IS
    IN
        x := x + 1;
        second(x);
    END
  
    PROGRAM IS
    val
    IN
        val := 5;
        first(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "116");  // 5 + 1 + 10 + 100
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, MultipleCallsSameParameter) {
  std::string source_code = R"(
    PROCEDURE increment(n) IS
    IN
        n := n + 1;
    END
  
    PROGRAM IS
    counter
    IN
        counter := 0;
        increment(counter);
        increment(counter);
        increment(counter);
        WRITE counter;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "3");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, IParamCanBeRead) {
  std::string source_code = R"(
    PROCEDURE readval(I x) IS
    IN
        WRITE x;
    END
  
    PROGRAM IS
    val
    IN
        val := 42;
        readval(val);
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

TEST_F(ProcedureTest, IParamInExpression) {
  std::string source_code = R"(
    PROCEDURE compute(I x, result) IS
    IN
        result := x + 10;
    END
  
    PROGRAM IS
    val, res
    IN
        val := 5;
        res := 0;
        compute(val, res);
        WRITE res;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "15");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, IParamPassedToAnotherIParam) {
  std::string source_code = R"(
    PROCEDURE inner(I x) IS
    IN
        WRITE x;
    END
  
    PROCEDURE outer(I y) IS
    IN
        inner(y);
    END
  
    PROGRAM IS
    val
    IN
        val := 99;
        outer(val);
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

// O prefix tests - output parameter (undefined value until assigned)

TEST_F(ProcedureTest, OParamCanBeWritten) {
  std::string source_code = R"(
    PROCEDURE setval(O x) IS
    IN
        x := 100;
    END
  
    PROGRAM IS
    val
    IN
        val := 0;
        setval(val);
        WRITE val;
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

TEST_F(ProcedureTest, OParamWriteThenRead) {
  std::string source_code = R"(
    PROCEDURE compute(O x) IS
    IN
        x := 50;
        x := x + 25;
    END
  
    PROGRAM IS
    val
    IN
        val := 0;
        compute(val);
        WRITE val;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "75");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, MixedIOParams) {
  std::string source_code = R"(
    PROCEDURE transform(I input, O output) IS
    IN
        output := input * 2;
    END
  
    PROGRAM IS
    a, b
    IN
        a := 21;
        b := 0;
        transform(a, b);
        WRITE b;
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

TEST_F(ProcedureTest, MultipleOParams) {
  std::string source_code = R"(
    PROCEDURE minmax(I a, I b, O min_val, O max_val) IS
    IN
        IF a < b THEN
            min_val := a;
            max_val := b;
        ELSE
            min_val := b;
            max_val := a;
        ENDIF
    END
  
    PROGRAM IS
    x, y, min_out, max_out
    IN
        x := 50;
        y := 30;
        min_out := 0;
        max_out := 0;
        minmax(x, y, min_out, max_out);
        WRITE min_out;
        WRITE max_out;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "30");
    EXPECT_EQ(output[1], "50");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

// T prefix tests for array parameters

TEST_F(ProcedureTest, TArrayParamRead) {
  std::string source_code = R"(
    PROCEDURE sumarray(T arr, I len, O sum) IS
    IN
        sum := 0;
        FOR i FROM 0 TO len DO
            sum := sum + arr[i];
        ENDFOR
    END
  
    PROGRAM IS
    data[0:4], total, len
    IN
        data[0] := 1;
        data[1] := 2;
        data[2] := 3;
        data[3] := 4;
        data[4] := 5;
        total := 0;
        len := 4;
        sumarray(data, len, total);
        WRITE total;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "15");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(ProcedureTest, TArrayParamWrite) {
  std::string source_code = R"(
    PROCEDURE fillarray(T arr, I len, I val) IS
    IN
        FOR i FROM 0 TO len DO
            arr[i] := val;
        ENDFOR
    END
  
    PROGRAM IS
    data[0:2], len, val
    IN
        data[0] := 0;
        data[1] := 0;
        data[2] := 0;
        len := 2;
        val := 7;
        fillarray(data, len, val);
        WRITE data[0];
        WRITE data[1];
        WRITE data[2];
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "7");
    EXPECT_EQ(output[1], "7");
    EXPECT_EQ(output[2], "7");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}
