#include "compiler_test.hpp"

class ArrayTest : public CompilerTest {};

// ==================== Basic Array Operations ====================

TEST_F(ArrayTest, SimpleArrayAssignAndRead) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5]
        IN
            arr[0] := 10;
            arr[1] := 20;
            arr[2] := 30;
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
    EXPECT_EQ(output[2], "30");
}

TEST_F(ArrayTest, ArrayWithVariableIndex) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], i
        IN
            arr[0] := 100;
            arr[1] := 200;
            arr[2] := 300;
            i := 0;
            WRITE arr[i];
            i := 1;
            WRITE arr[i];
            i := 2;
            WRITE arr[i];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "100");
    EXPECT_EQ(output[1], "200");
    EXPECT_EQ(output[2], "300");
}

TEST_F(ArrayTest, ArrayAssignmentWithVariableIndex) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], i
        IN
            i := 0;
            arr[i] := 111;
            i := 1;
            arr[i] := 222;
            i := 2;
            arr[i] := 333;
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "111");
    EXPECT_EQ(output[1], "222");
    EXPECT_EQ(output[2], "333");
}

// ==================== Non-Zero Start Index ====================

TEST_F(ArrayTest, ArrayWithNonZeroStartIndex) {
    std::string source_code = R"(
        PROGRAM IS
            arr[5:10]
        IN
            arr[5] := 50;
            arr[7] := 70;
            arr[10] := 100;
            WRITE arr[5];
            WRITE arr[7];
            WRITE arr[10];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "50");
    EXPECT_EQ(output[1], "70");
    EXPECT_EQ(output[2], "100");
}

TEST_F(ArrayTest, ArrayWithLargeStartIndex) {
    std::string source_code = R"(
        PROGRAM IS
            arr[100:105]
        IN
            arr[100] := 1;
            arr[101] := 2;
            arr[102] := 3;
            arr[103] := 4;
            arr[104] := 5;
            arr[105] := 6;
            WRITE arr[100];
            WRITE arr[103];
            WRITE arr[105];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "4");
    EXPECT_EQ(output[2], "6");
}

TEST_F(ArrayTest, ArrayWithNonZeroStartAndVariableIndex) {
    std::string source_code = R"(
        PROGRAM IS
            arr[10:15], i
        IN
            arr[10] := 10;
            arr[11] := 11;
            arr[12] := 12;
            i := 10;
            WRITE arr[i];
            i := 11;
            WRITE arr[i];
            i := 12;
            WRITE arr[i];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "11");
    EXPECT_EQ(output[2], "12");
}

// ==================== Wide Index Ranges ====================

TEST_F(ArrayTest, ArrayWithWideRange) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:100]
        IN
            arr[0] := 1;
            arr[50] := 50;
            arr[100] := 100;
            WRITE arr[0];
            WRITE arr[50];
            WRITE arr[100];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "50");
    EXPECT_EQ(output[2], "100");
}

// ==================== Array Elements in Expressions ====================

TEST_F(ArrayTest, ArrayElementsInAddition) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], result
        IN
            arr[0] := 10;
            arr[1] := 20;
            arr[2] := 30;
            result := arr[0] + arr[1];
            WRITE result;
            result := arr[1] + arr[2];
            WRITE result;
            result := arr[0] + arr[2];
            WRITE result;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "30");
    EXPECT_EQ(output[1], "50");
    EXPECT_EQ(output[2], "40");
}

TEST_F(ArrayTest, ArrayElementsInSubtraction) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], result
        IN
            arr[0] := 100;
            arr[1] := 30;
            result := arr[0] - arr[1];
            WRITE result;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "70");
}

TEST_F(ArrayTest, ArrayElementsInMultiplication) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], result
        IN
            arr[0] := 7;
            arr[1] := 8;
            result := arr[0] * arr[1];
            WRITE result;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "56");
}

TEST_F(ArrayTest, ArrayElementsInDivision) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], result
        IN
            arr[0] := 100;
            arr[1] := 5;
            result := arr[0] / arr[1];
            WRITE result;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "20");
}

TEST_F(ArrayTest, ArrayElementsInModulo) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], result
        IN
            arr[0] := 17;
            arr[1] := 5;
            result := arr[0] % arr[1];
            WRITE result;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "2");
}

TEST_F(ArrayTest, ArrayElementWithConstantInExpression) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5]
        IN
            arr[0] := 10;
            arr[1] := arr[0] + 5;
            arr[2] := arr[1] * 2;
            WRITE arr[1];
            WRITE arr[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "15");
    EXPECT_EQ(output[1], "30");
}

TEST_F(ArrayTest, ArrayElementWithVariableInExpression) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], x
        IN
            x := 7;
            arr[0] := 10;
            arr[1] := arr[0] + x;
            WRITE arr[1];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "17");
}

// ==================== Array in Conditions ====================

TEST_F(ArrayTest, ArrayElementInIfCondition) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5]
        IN
            arr[0] := 10;
            arr[1] := 20;
            IF arr[0] < arr[1] THEN
                WRITE 1;
            ELSE
                WRITE 0;
            ENDIF
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
}

TEST_F(ArrayTest, ArrayElementInWhileCondition) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:5], i
        IN
            arr[0] := 0;
            i := 0;
            WHILE arr[0] < 5 DO
                arr[0] := arr[0] + 1;
                i := i + 1;
            ENDWHILE
            WRITE arr[0];
            WRITE i;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "5");
    EXPECT_EQ(output[1], "5");
}

// ==================== Arrays as Procedure Arguments ====================

TEST_F(ArrayTest, PassArrayToProcedure) {
    std::string source_code = R"(
        PROCEDURE printfirst(arr) IS
        IN
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
        END

        PROGRAM IS
            tab[0:5]
        IN
            tab[0] := 100;
            tab[1] := 200;
            tab[2] := 300;
            printfirst(tab);
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "100");
    EXPECT_EQ(output[1], "200");
    EXPECT_EQ(output[2], "300");
}

TEST_F(ArrayTest, ModifyArrayInProcedure) {
    std::string source_code = R"(
        PROCEDURE addfive(arr) IS
        IN
            arr[0] := arr[0] + 5;
            arr[1] := arr[1] + 5;
        END

        PROGRAM IS
            tab[0:5]
        IN
            tab[0] := 10;
            tab[1] := 20;
            WRITE tab[0];
            WRITE tab[1];
            addfive(tab);
            WRITE tab[0];
            WRITE tab[1];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
    EXPECT_EQ(output[2], "15");
    EXPECT_EQ(output[3], "25");
}

TEST_F(ArrayTest, PassArrayWithNonZeroStartToProcedure) {
    std::string source_code = R"(
        PROCEDURE readarray(arr) IS
        IN
            WRITE arr[10];
            WRITE arr[11];
            WRITE arr[12];
        END

        PROGRAM IS
            tab[10:15]
        IN
            tab[10] := 1000;
            tab[11] := 1100;
            tab[12] := 1200;
            readarray(tab);
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1000");
    EXPECT_EQ(output[1], "1100");
    EXPECT_EQ(output[2], "1200");
}

TEST_F(ArrayTest, NestedProcedureCallsWithArray) {
    std::string source_code = R"(
        PROCEDURE inner(arr) IS
        IN
            arr[0] := arr[0] + 1;
        END

        PROCEDURE outer(arr) IS
        IN
            inner(arr);
            inner(arr);
        END

        PROGRAM IS
            tab[0:5]
        IN
            tab[0] := 10;
            WRITE tab[0];
            outer(tab);
            WRITE tab[0];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "12");
}

TEST_F(ArrayTest, MultipleProcedureCallsWithSameArray) {
    std::string source_code = R"(
        PROCEDURE increment(arr) IS
        IN
            arr[0] := arr[0] + 1;
        END

        PROGRAM IS
            tab[0:5]
        IN
            tab[0] := 0;
            increment(tab);
            increment(tab);
            increment(tab);
            WRITE tab[0];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "3");
}

TEST_F(ArrayTest, ProcedureWithArrayAndScalarParams) {
    std::string source_code = R"(
        PROCEDURE setvalue(arr, val) IS
        IN
            arr[0] := val;
        END

        PROGRAM IS
            tab[0:5], x
        IN
            x := 42;
            setvalue(tab, x);
            WRITE tab[0];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "42");
}

// ==================== Local Arrays in Procedures ====================

TEST_F(ArrayTest, LocalArrayInProcedure) {
    std::string source_code = R"(
        PROCEDURE computesum(result) IS
            local[0:3]
        IN
            local[0] := 10;
            local[1] := 20;
            local[2] := 30;
            result := local[0] + local[1];
            result := result + local[2];
        END

        PROGRAM IS
            sum
        IN
            sum := 0;
            computesum(sum);
            WRITE sum;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "60");
}

TEST_F(ArrayTest, LocalArrayWithLoop) {
    std::string source_code = R"(
        PROCEDURE initarray(result) IS
            arr[0:4], i, sum
        IN
            i := 0;
            WHILE i < 5 DO
                arr[i] := i * 10;
                i := i + 1;
            ENDWHILE
            
            sum := 0;
            i := 0;
            WHILE i < 5 DO
                sum := sum + arr[i];
                i := i + 1;
            ENDWHILE
            result := sum;
        END

        PROGRAM IS
            total
        IN
            total := 0;
            initarray(total);
            WRITE total;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    // 0*10 + 1*10 + 2*10 + 3*10 + 4*10 = 0 + 10 + 20 + 30 + 40 = 100
    EXPECT_EQ(output[0], "100");
}

// ==================== Multiple Arrays ====================

TEST_F(ArrayTest, TwoArrays) {
    std::string source_code = R"(
        PROGRAM IS
            arr_a[0:3], arr_b[0:3]
        IN
            arr_a[0] := 1;
            arr_a[1] := 2;
            arr_b[0] := 10;
            arr_b[1] := 20;
            WRITE arr_a[0];
            WRITE arr_a[1];
            WRITE arr_b[0];
            WRITE arr_b[1];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "2");
    EXPECT_EQ(output[2], "10");
    EXPECT_EQ(output[3], "20");
}

TEST_F(ArrayTest, CopyBetweenArrays) {
    std::string source_code = R"(
        PROGRAM IS
            src[0:3], dst[0:3], i
        IN
            src[0] := 100;
            src[1] := 200;
            src[2] := 300;
            
            i := 0;
            WHILE i < 3 DO
                dst[i] := src[i];
                i := i + 1;
            ENDWHILE
            
            WRITE dst[0];
            WRITE dst[1];
            WRITE dst[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "100");
    EXPECT_EQ(output[1], "200");
    EXPECT_EQ(output[2], "300");
}

TEST_F(ArrayTest, ArraysWithDifferentRanges) {
    std::string source_code = R"(
        PROGRAM IS
            arr_a[0:5], arr_b[10:15], arr_c[100:102]
        IN
            arr_a[0] := 1;
            arr_b[10] := 2;
            arr_c[100] := 3;
            WRITE arr_a[0];
            WRITE arr_b[10];
            WRITE arr_c[100];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "2");
    EXPECT_EQ(output[2], "3");
}

// ==================== Array Loops ====================

TEST_F(ArrayTest, FillArrayInLoop) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:4], i
        IN
            i := 0;
            WHILE i < 5 DO
                arr[i] := i * i;
                i := i + 1;
            ENDWHILE
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
            WRITE arr[3];
            WRITE arr[4];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 5);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "4");
    EXPECT_EQ(output[3], "9");
    EXPECT_EQ(output[4], "16");
}

TEST_F(ArrayTest, SumArrayElements) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:4], sum, i
        IN
            arr[0] := 10;
            arr[1] := 20;
            arr[2] := 30;
            arr[3] := 40;
            arr[4] := 50;
            
            sum := 0;
            i := 0;
            WHILE i < 5 DO
                sum := sum + arr[i];
                i := i + 1;
            ENDWHILE
            WRITE sum;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "150");
}

TEST_F(ArrayTest, FindMaxInArray) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:4], max, i
        IN
            arr[0] := 30;
            arr[1] := 10;
            arr[2] := 50;
            arr[3] := 20;
            arr[4] := 40;
            
            max := arr[0];
            i := 1;
            WHILE i < 5 DO
                IF arr[i] > max THEN
                    max := arr[i];
                ENDIF
                i := i + 1;
            ENDWHILE
            WRITE max;
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "50");
}

// ==================== Using Array Value as Index ====================

TEST_F(ArrayTest, ArrayValueAsIndex) {
    std::string source_code = R"(
        PROGRAM IS
            data[0:5], indices[0:3], tmp
        IN
            data[1] := 100;
            data[3] := 300;
            data[5] := 500;
            
            indices[0] := 1;
            indices[1] := 3;
            indices[2] := 5;
            
            tmp := indices[0];
            WRITE data[tmp];
            tmp := indices[1];
            WRITE data[tmp];
            tmp := indices[2];
            WRITE data[tmp];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "100");
    EXPECT_EQ(output[1], "300");
    EXPECT_EQ(output[2], "500");
}

// ==================== Edge Cases ====================

TEST_F(ArrayTest, SingleElementArray) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:0]
        IN
            arr[0] := 999;
            WRITE arr[0];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "999");
}

TEST_F(ArrayTest, ArrayWithZeroValues) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:3]
        IN
            arr[0] := 0;
            arr[1] := 0;
            arr[2] := 0;
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "0");
    EXPECT_EQ(output[2], "0");
}

TEST_F(ArrayTest, ArrayWithLargeValues) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:2]
        IN
            arr[0] := 1000000;
            arr[1] := 9999999;
            WRITE arr[0];
            WRITE arr[1];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "1000000");
    EXPECT_EQ(output[1], "9999999");
}

// ==================== Complex Scenarios ====================

TEST_F(ArrayTest, BubbleSortLike) {
    std::string source_code = R"(
        PROGRAM IS
            arr[0:2], tmp, swapped
        IN
            arr[0] := 30;
            arr[1] := 10;
            arr[2] := 20;
            
            swapped := 1;
            WHILE swapped > 0 DO
                swapped := 0;
                IF arr[0] > arr[1] THEN
                    tmp := arr[0];
                    arr[0] := arr[1];
                    arr[1] := tmp;
                    swapped := 1;
                ENDIF
                IF arr[1] > arr[2] THEN
                    tmp := arr[1];
                    arr[1] := arr[2];
                    arr[2] := tmp;
                    swapped := 1;
                ENDIF
            ENDWHILE
            
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "10");
    EXPECT_EQ(output[1], "20");
    EXPECT_EQ(output[2], "30");
}

TEST_F(ArrayTest, FibonacciInArray) {
    std::string source_code = R"(
        PROGRAM IS
            fib[0:9], i, prev, preprev
        IN
            fib[0] := 0;
            fib[1] := 1;
            i := 2;
            WHILE i < 10 DO
                prev := i-1;
                preprev := i-2;
                fib[i] := fib[prev] + fib[preprev];
                i := i + 1;
            ENDWHILE
            WRITE fib[0];
            WRITE fib[1];
            WRITE fib[5];
            WRITE fib[9];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "5");
    EXPECT_EQ(output[3], "34");
}

TEST_F(ArrayTest, ArrayReversal) {
    std::string source_code = R"(
        PROCEDURE reverse(arr, len) IS
            i, j, tmp
        IN
            i := 0;
            j := len - 1;
            WHILE i < j DO
                tmp := arr[i];
                arr[i] := arr[j];
                arr[j] := tmp;
                i := i + 1;
                j := j - 1;
            ENDWHILE
        END

        PROGRAM IS
            arr[0:4], n
        IN
            arr[0] := 1;
            arr[1] := 2;
            arr[2] := 3;
            arr[3] := 4;
            arr[4] := 5;
            n := 5;
            
            reverse(arr, n);
            
            WRITE arr[0];
            WRITE arr[1];
            WRITE arr[2];
            WRITE arr[3];
            WRITE arr[4];
        END
    )";

    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 5);
    EXPECT_EQ(output[0], "5");
    EXPECT_EQ(output[1], "4");
    EXPECT_EQ(output[2], "3");
    EXPECT_EQ(output[3], "2");
    EXPECT_EQ(output[4], "1");
}
