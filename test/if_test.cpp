#include "compiler_test.hpp"

class IfTest : public CompilerTest {};

TEST_F(IfTest, IfWithoutElse) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 10;
            b := 20;
            IF a < b THEN
                c := 42;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "42");
}

TEST_F(IfTest, IfWithElse) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 30;
            b := 20;
            IF a < b THEN
                c := 42;
            ELSE
                c := 84;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "84");
}

TEST_F(IfTest, NestedIfElseA) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 15;
            b := 25;
            IF a < b THEN
                IF a = 15 THEN
                    c := 100;
                ELSE
                    c := 200;
                ENDIF
            ELSE
                c := 300;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "100");
}

TEST_F(IfTest, NestedIfElseB) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 35;
            b := 25;
            IF a < b THEN
                IF a = 15 THEN
                    c := 100;
                ELSE
                    c := 200;
                ENDIF
            ELSE
                c := 300;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "300");
}

TEST_F(IfTest, IfWithEqualityCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 42;
            b := 42;
            IF a = b THEN
                c := 1;
            ELSE
                c := 0;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
}

TEST_F(IfTest, IfWithNotEqualCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 10;
            b := 20;
            IF a != b THEN
                c := 1;
            ELSE
                c := 0;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
}

TEST_F(IfTest, IfWithGreaterOrEqualCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 50;
            b := 50;
            IF a >= b THEN
                c := 100;
            ELSE
                c := 0;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "100");
}

TEST_F(IfTest, IfWithLessOrEqualCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 30;
            b := 30;
            IF a <= b THEN
                c := 200;
            ELSE
                c := 0;
            ENDIF
            WRITE c;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "200");
}

TEST_F(IfTest, IfWithZeroComparison) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            IF a = 0 THEN
                b := 777;
            ELSE
                b := 888;
            ENDIF
            WRITE b;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "777");
}

TEST_F(IfTest, DeeplyNestedIf) {
    std::string source_code = R"(
        PROGRAM IS
        a, result
        IN
            a := 3;
            IF a > 0 THEN
                IF a > 1 THEN
                    IF a > 2 THEN
                        IF a > 3 THEN
                            result := 4;
                        ELSE
                            result := 3;
                        ENDIF
                    ELSE
                        result := 2;
                    ENDIF
                ELSE
                    result := 1;
                ENDIF
            ELSE
                result := 0;
            ENDIF
            WRITE result;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "3");
}

TEST_F(IfTest, IfInLoop) {
    std::string source_code = R"(
        PROGRAM IS
        i, count
        IN
            i := 0;
            count := 0;
            WHILE i < 10 DO
                IF i >= 5 THEN
                    count := count + 1;
                ENDIF
                i := i + 1;
            ENDWHILE
            WRITE count;
        END
    )";
    
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "5");
}

