#include "compiler_test.hpp"

class ForTest : public CompilerTest {};

// ==================== FOR TO (Ascending) Tests ====================

TEST_F(ForTest, SimpleForTo) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 1 TO 5 DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "15");  // 1+2+3+4+5 = 15
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForToSingleIteration) {
    std::string source_code = R"(
        PROGRAM IS
        result
        IN
            result := 0;
            FOR i FROM 5 TO 5 DO
                result := result + 1;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "1");  // Single iteration when start = end
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForToZeroIterations) {
    std::string source_code = R"(
        PROGRAM IS
        result
        IN
            result := 10;
            FOR i FROM 5 TO 3 DO
                result := result + 1;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "10");  // No iterations when start > end for TO
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForToFromZero) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 0 TO 4 DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "10");  // 0+1+2+3+4 = 10
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForToWithVariableBounds) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, sum
        IN
            a := 2;
            b := 6;
            sum := 0;
            FOR i FROM a TO b DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "20");  // 2+3+4+5+6 = 20
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForToWriteIterator) {
    std::string source_code = R"(
        PROGRAM IS
        IN
            FOR i FROM 1 TO 5 DO
                WRITE i;
            ENDFOR
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "1");
        EXPECT_EQ(output[1], "2");
        EXPECT_EQ(output[2], "3");
        EXPECT_EQ(output[3], "4");
        EXPECT_EQ(output[4], "5");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== FOR DOWNTO (Descending) Tests ====================

TEST_F(ForTest, SimpleForDownto) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 5 DOWNTO 1 DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "15");  // 5+4+3+2+1 = 15
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoSingleIteration) {
    std::string source_code = R"(
        PROGRAM IS
        result
        IN
            result := 0;
            FOR i FROM 5 DOWNTO 5 DO
                result := result + 1;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "1");  // Single iteration when start = end
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoZeroIterations) {
    std::string source_code = R"(
        PROGRAM IS
        result
        IN
            result := 10;
            FOR i FROM 3 DOWNTO 5 DO
                result := result + 1;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "10");  // No iterations when start < end for DOWNTO
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoToZero) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 4 DOWNTO 0 DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "10");  // 4+3+2+1+0 = 10
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoWithVariableBounds) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, sum
        IN
            a := 6;
            b := 2;
            sum := 0;
            FOR i FROM a DOWNTO b DO
                sum := sum + i;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "20");  // 6+5+4+3+2 = 20
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoWriteIterator) {
    std::string source_code = R"(
        PROGRAM IS
        IN
            FOR i FROM 5 DOWNTO 1 DO
                WRITE i;
            ENDFOR
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "5");
        EXPECT_EQ(output[1], "4");
        EXPECT_EQ(output[2], "3");
        EXPECT_EQ(output[3], "2");
        EXPECT_EQ(output[4], "1");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Nested FOR Loops ====================

TEST_F(ForTest, NestedForLoops) {
    std::string source_code = R"(
        PROGRAM IS
        count
        IN
            count := 0;
            FOR i FROM 1 TO 3 DO
                FOR j FROM 1 TO 4 DO
                    count := count + 1;
                ENDFOR
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "12");  // 3 * 4 = 12
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, NestedForToAndDownto) {
    std::string source_code = R"(
        PROGRAM IS
        sum, temp
        IN
            sum := 0;
            FOR i FROM 1 TO 3 DO
                FOR j FROM 3 DOWNTO 1 DO
                    temp := i + j;
                    sum := sum + temp;
                ENDFOR
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        // i=1: (1+3)+(1+2)+(1+1) = 4+3+2 = 9
        // i=2: (2+3)+(2+2)+(2+1) = 5+4+3 = 12
        // i=3: (3+3)+(3+2)+(3+1) = 6+5+4 = 15
        // Total: 9+12+15 = 36
        EXPECT_EQ(output[0], "36");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, DeeplyNestedForLoops) {
    std::string source_code = R"(
        PROGRAM IS
        count
        IN
            count := 0;
            FOR i FROM 1 TO 2 DO
                FOR j FROM 1 TO 3 DO
                    FOR k FROM 1 TO 4 DO
                        count := count + 1;
                    ENDFOR
                ENDFOR
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "24");  // 2 * 3 * 4 = 24
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== FOR Loop with Array Access ====================

TEST_F(ForTest, ForLoopArraySum) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:9], sum
        IN
            arr[0] := 1;
            arr[1] := 2;
            arr[2] := 3;
            arr[3] := 4;
            arr[4] := 5;
            sum := 0;
            FOR i FROM 0 TO 4 DO
                sum := sum + arr[i];
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "15");  // 1+2+3+4+5 = 15
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForLoopArrayInitialize) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:4], temp
        IN
            FOR i FROM 0 TO 4 DO
                temp := i + 1;
                arr[i] := temp;
            ENDFOR
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
            WRITE arr[3];
            WRITE arr[4];
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "1");
        EXPECT_EQ(output[1], "2");
        EXPECT_EQ(output[2], "3");
        EXPECT_EQ(output[3], "4");
        EXPECT_EQ(output[4], "5");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForLoopArrayReverse) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:4]
        IN
            arr[0] := 10;
            arr[1] := 20;
            arr[2] := 30;
            arr[3] := 40;
            arr[4] := 50;
            FOR i FROM 4 DOWNTO 0 DO
                WRITE arr[i];
            ENDFOR
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "50");
        EXPECT_EQ(output[1], "40");
        EXPECT_EQ(output[2], "30");
        EXPECT_EQ(output[3], "20");
        EXPECT_EQ(output[4], "10");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== FOR Loop with Expressions in Bounds ====================

TEST_F(ForTest, ForWithExpressionStart) {
    std::string source_code = R"(
        PROGRAM IS
        a, sum, start
        IN
            a := 3;
            start := a + 2;
            sum := 0;
            FOR i FROM start TO 10 DO
                sum := sum + 1;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "6");  // i from 5 to 10 = 6 iterations
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForWithExpressionEnd) {
    std::string source_code = R"(
        PROGRAM IS
        a, sum, end_val
        IN
            a := 3;
            end_val := a * 2;
            sum := 0;
            FOR i FROM 1 TO end_val DO
                sum := sum + 1;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "6");  // i from 1 to 6 = 6 iterations
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== FOR Loop with Other Control Structures ====================

TEST_F(ForTest, ForWithIfInside) {
    std::string source_code = R"(
        PROGRAM IS
        sum, rem
        IN
            sum := 0;
            FOR i FROM 1 TO 10 DO
                rem := i % 2;
                IF rem = 0 THEN
                    sum := sum + i;
                ENDIF
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "30");  // 2+4+6+8+10 = 30
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForWithIfElseInside) {
    std::string source_code = R"(
        PROGRAM IS
        even_sum, odd_sum, rem
        IN
            even_sum := 0;
            odd_sum := 0;
            FOR i FROM 1 TO 10 DO
                rem := i % 2;
                IF rem = 0 THEN
                    even_sum := even_sum + i;
                ELSE
                    odd_sum := odd_sum + i;
                ENDIF
            ENDFOR
            WRITE even_sum;
            WRITE odd_sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 2);
        EXPECT_EQ(output[0], "30");  // 2+4+6+8+10 = 30
        EXPECT_EQ(output[1], "25");  // 1+3+5+7+9 = 25
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForInsideWhile) {
    std::string source_code = R"(
        PROGRAM IS
        outer, total
        IN
            outer := 0;
            total := 0;
            WHILE outer < 3 DO
                FOR i FROM 1 TO 4 DO
                    total := total + 1;
                ENDFOR
                outer := outer + 1;
            ENDWHILE
            WRITE total;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "12");  // 3 * 4 = 12
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForInsideRepeat) {
    std::string source_code = R"(
        PROGRAM IS
        outer, total
        IN
            outer := 0;
            total := 0;
            REPEAT
                FOR i FROM 1 TO 5 DO
                    total := total + i;
                ENDFOR
                outer := outer + 1;
            UNTIL outer = 2;
            WRITE total;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "30");  // 2 * 15 = 30
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Complex Calculations with FOR ====================

TEST_F(ForTest, Factorial) {
    std::string source_code = R"(
        PROGRAM IS
        n, result
        IN
            n := 5;
            result := 1;
            FOR i FROM 1 TO n DO
                result := result * i;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "120");  // 5! = 120
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, SumOfSquares) {
    std::string source_code = R"(
        PROGRAM IS
        n, sum, sq
        IN
            n := 5;
            sum := 0;
            FOR i FROM 1 TO n DO
                sq := i * i;
                sum := sum + sq;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "55");  // 1+4+9+16+25 = 55
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, PowerCalculation) {
    std::string source_code = R"(
        PROGRAM IS
        base, exp, result
        IN
            base := 2;
            exp := 10;
            result := 1;
            FOR i FROM 1 TO exp DO
                result := result * base;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "1024");  // 2^10 = 1024
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, TriangularNumber) {
    std::string source_code = R"(
        PROGRAM IS
        n, result
        IN
            n := 10;
            result := 0;
            FOR i FROM 1 TO n DO
                result := result + i;
            ENDFOR
            WRITE result;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "55");  // Sum 1 to 10 = 55
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, Fibonacci) {
    std::string source_code = R"(
        PROGRAM IS
        n, a, b, c
        IN
            n := 10;
            a := 0;
            b := 1;
            FOR i FROM 1 TO n DO
                c := a + b;
                a := b;
                b := c;
            ENDFOR
            WRITE a;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "55");  // 10th Fibonacci = 55
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Edge Cases ====================

TEST_F(ForTest, ForWithLargeRange) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 1 TO 100 DO
                sum := sum + 1;
            ENDFOR
            WRITE sum;
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

TEST_F(ForTest, ForDowntoWithLargeRange) {
    std::string source_code = R"(
        PROGRAM IS
        sum
        IN
            sum := 0;
            FOR i FROM 100 DOWNTO 1 DO
                sum := sum + 1;
            ENDFOR
            WRITE sum;
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

TEST_F(ForTest, ForFromZeroToZero) {
    std::string source_code = R"(
        PROGRAM IS
        count
        IN
            count := 0;
            FOR i FROM 0 TO 0 DO
                count := count + 1;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "1");  // Single iteration at 0
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ConsecutiveForLoops) {
    std::string source_code = R"(
        PROGRAM IS
        sum_a, sum_b
        IN
            sum_a := 0;
            sum_b := 0;
            FOR i FROM 1 TO 5 DO
                sum_a := sum_a + i;
            ENDFOR
            FOR j FROM 6 TO 10 DO
                sum_b := sum_b + j;
            ENDFOR
            WRITE sum_a;
            WRITE sum_b;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 2);
        EXPECT_EQ(output[0], "15");  // 1+2+3+4+5 = 15
        EXPECT_EQ(output[1], "40");  // 6+7+8+9+10 = 40
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForLoopWithZeroBoundVariable) {
    std::string source_code = R"(
        PROGRAM IS
        start, end_val, count
        IN
            start := 0;
            end_val := 0;
            count := 0;
            FOR i FROM start TO end_val DO
                count := count + 1;
            ENDFOR
            WRITE count;
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

// ==================== FOR with Procedures ====================

TEST_F(ForTest, ForLoopCallsProcedure) {
    std::string source_code = R"(
        PROCEDURE double(n) IS
        IN
            n := n * 2;
        END
    
        PROGRAM IS
        val, sum
        IN
            sum := 0;
            FOR i FROM 1 TO 5 DO
                val := i;
                double(val);
                sum := sum + val;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "30");  // 2+4+6+8+10 = 30
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForLoopInProcedure) {
    std::string source_code = R"(
        PROCEDURE sum_range(start, end_val, result) IS
        IN
            result := 0;
            FOR i FROM start TO end_val DO
                result := result + i;
            ENDFOR
        END
    
        PROGRAM IS
        res, x, y
        IN
            res := 0;
            x := 1;
            y := 10;
            sum_range(x, y, res);
            WRITE res;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "55");  // 1+2+...+10 = 55
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForLoopWithArrayProcedure) {
    std::string source_code = R"(
        PROCEDURE init_array(T arr, size) IS
        IN
            FOR i FROM 0 TO size DO
                arr[i] := i * 10;
            ENDFOR
        END
    
        PROGRAM IS
        my_arr[0:9], sum, arr_size
        IN
            arr_size := 4;
            init_array(my_arr, arr_size);
            sum := 0;
            FOR i FROM 0 TO arr_size DO
                sum := sum + my_arr[i];
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "100");  // 0+10+20+30+40 = 100
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Mathematical Algorithms ====================

TEST_F(ForTest, IsPrimeCheck) {
    std::string source_code = R"(
        PROGRAM IS
        n, is_prime, limit, rem
        IN
            n := 17;
            is_prime := 1;
            limit := n - 1;
            FOR i FROM 2 TO limit DO
                rem := n % i;
                IF rem = 0 THEN
                    is_prime := 0;
                ENDIF
            ENDFOR
            WRITE is_prime;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "1");  // 17 is prime
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, GCDUsingLoop) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, gcd, rem_a, rem_b
        IN
            a := 48;
            b := 18;
            gcd := 1;
            FOR i FROM 1 TO b DO
                rem_a := a % i;
                IF rem_a = 0 THEN
                    rem_b := b % i;
                    IF rem_b = 0 THEN
                        gcd := i;
                    ENDIF
                ENDIF
            ENDFOR
            WRITE gcd;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "6");  // GCD(48, 18) = 6
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, CountDivisors) {
    std::string source_code = R"(
        PROGRAM IS
        n, count, rem
        IN
            n := 12;
            count := 0;
            FOR i FROM 1 TO n DO
                rem := n % i;
                IF rem = 0 THEN
                    count := count + 1;
                ENDIF
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "6");  // Divisors of 12: 1,2,3,4,6,12
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, SumDigits) {
    std::string source_code = R"(
        PROGRAM IS
        n, sum, digit
        IN
            n := 12345;
            sum := 0;
            FOR i FROM 1 TO 5 DO
                digit := n % 10;
                sum := sum + digit;
                n := n / 10;
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "15");  // 1+2+3+4+5 = 15
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Bubble Sort ====================

TEST_F(ForTest, BubbleSortSmall) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:4], temp, n, limit, next
        IN
            arr[0] := 5;
            arr[1] := 2;
            arr[2] := 4;
            arr[3] := 1;
            arr[4] := 3;
            n := 4;
            limit := n - 1;
            
            FOR i FROM 0 TO limit DO
                FOR j FROM 0 TO limit DO
                    next := j + 1;
                    IF arr[j] > arr[next] THEN
                        temp := arr[j];
                        arr[j] := arr[next];
                        arr[next] := temp;
                    ENDIF
                ENDFOR
            ENDFOR
            
            FOR i FROM 0 TO n DO
                WRITE arr[i];
            ENDFOR
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "1");
        EXPECT_EQ(output[1], "2");
        EXPECT_EQ(output[2], "3");
        EXPECT_EQ(output[3], "4");
        EXPECT_EQ(output[4], "5");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Array Min/Max ====================

TEST_F(ForTest, FindMinimum) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:4], min_val
        IN
            arr[0] := 5;
            arr[1] := 2;
            arr[2] := 8;
            arr[3] := 1;
            arr[4] := 9;
            
            min_val := arr[0];
            FOR i FROM 1 TO 4 DO
                IF arr[i] < min_val THEN
                    min_val := arr[i];
                ENDIF
            ENDFOR
            WRITE min_val;
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

TEST_F(ForTest, FindMaximum) {
    std::string source_code = R"(
        PROGRAM IS
        arr[0:4], max_val
        IN
            arr[0] := 5;
            arr[1] := 2;
            arr[2] := 8;
            arr[3] := 1;
            arr[4] := 9;
            
            max_val := arr[0];
            FOR i FROM 1 TO 4 DO
                IF arr[i] > max_val THEN
                    max_val := arr[i];
                ENDIF
            ENDFOR
            WRITE max_val;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "9");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

// ==================== Matrix-like Operations ====================

TEST_F(ForTest, MatrixSum) {
    std::string source_code = R"(
        PROGRAM IS
        matrix[0:8], sum, rows, cols, idx, val
        IN
            rows := 3;
            cols := 3;
            
            FOR i FROM 0 TO 2 DO
                FOR j FROM 0 TO 2 DO
                    idx := i * 3;
                    idx := idx + j;
                    val := i + j;
                    matrix[idx] := val;
                ENDFOR
            ENDFOR
            
            sum := 0;
            FOR i FROM 0 TO 8 DO
                sum := sum + matrix[i];
            ENDFOR
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "18");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForBoundsFixedAtStart_EndVarChanges) {
    std::string source_code = R"(
        PROGRAM IS
        end_val, count
        IN
            end_val := 5;
            count := 0;
            FOR i FROM 1 TO end_val DO
                count := count + 1;
                end_val := 100;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "5");  // Should iterate 5 times, not 100
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForBoundsFixedAtStart_StartVarChanges) {
    std::string source_code = R"(
        PROGRAM IS
        start_val, count
        IN
            start_val := 1;
            count := 0;
            FOR i FROM start_val TO 5 DO
                count := count + 1;
                start_val := 10;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "5");  // Should iterate 5 times (1 to 5)
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForBoundsFixedAtStart_BothVarsChange) {
    std::string source_code = R"(
        PROGRAM IS
        start_val, end_val, count
        IN
            start_val := 3;
            end_val := 7;
            count := 0;
            FOR i FROM start_val TO end_val DO
                count := count + 1;
                start_val := 1;
                end_val := 20;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "5");  // Should iterate 5 times (3 to 7)
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForDowntoBoundsFixedAtStart) {
    std::string source_code = R"(
        PROGRAM IS
        start_val, end_val, count
        IN
            start_val := 10;
            end_val := 6;
            count := 0;
            FOR i FROM start_val DOWNTO end_val DO
                count := count + 1;
                start_val := 100;
                end_val := 1;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "5");  // Should iterate 5 times (10 down to 6)
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(ForTest, ForBoundsFixedAtStart_NestedLoops) {
    std::string source_code = R"(
        PROGRAM IS
        outer_end, inner_end, count
        IN
            outer_end := 3;
            inner_end := 3;
            count := 0;
            FOR i FROM 1 TO outer_end DO
                FOR j FROM 1 TO inner_end DO
                    count := count + 1;
                    inner_end := 10;
                ENDFOR
                outer_end := 10;
            ENDFOR
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "23");  // outer(3) * inner(3,10,10) = 3+10+10 = 23
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}
