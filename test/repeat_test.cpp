#include "compiler_test.hpp"

class RepeatTest : public CompilerTest {};

TEST_F(RepeatTest, SimpleRepeatLoop) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 5;
            REPEAT
                a := a + 1;
            UNTIL a = b;
            WRITE a;
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

TEST_F(RepeatTest, RepeatOneIteration) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 10;
            REPEAT
                a := a + 1;
            UNTIL a > b;
            WRITE a;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "11");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatMultipleIterations) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 0;
            b := 3;
            c := 0;
            REPEAT
                c := c + 2;
                a := a + 1;
            UNTIL a = b;
            WRITE c;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "6");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatWithGreaterThanCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 10;
            REPEAT
                a := a + 2;
            UNTIL a >= b;
            WRITE a;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "10");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, NestedRepeatLoops) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 0;
            b := 2;
            c := 0;
            REPEAT
                a := a + 1;
                c := 0;
                REPEAT
                    c := c + 1;
                UNTIL c = 3;
            UNTIL a = b;
            WRITE c;
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

TEST_F(RepeatTest, RepeatInternalWrite) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 3;
            REPEAT
                a := a + 1;
                WRITE a;
            UNTIL a = b;
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

TEST_F(RepeatTest, RepeatWithComplexCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 1;
            b := 2;
            c := 0;
            REPEAT
                c := a + b;
                a := b;
                b := c;
            UNTIL c >= 50;
            WRITE c;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "55");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatWithLessThanCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 5;
            REPEAT
                a := a - 1;
            UNTIL a < b;
            WRITE a;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "4");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatWithNotEqualCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 5;
            REPEAT
                a := a + 1;
            UNTIL a != b;
            WRITE a;
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

TEST_F(RepeatTest, RepeatImmediateConditionTrue) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 100;
            b := 0;
            REPEAT
                b := b + 1;
            UNTIL a > 50;
            WRITE b;
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

TEST_F(RepeatTest, RepeatModifyingBothVariablesInCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 1;
            b := 10;
            c := 0;
            REPEAT
                a := a + 1;
                b := b - 1;
                c := c + 1;
            UNTIL a >= b;
            WRITE c;
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

TEST_F(RepeatTest, RepeatWithZeroInCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a
        IN
            a := 5;
            REPEAT
                a := a - 1;
            UNTIL a = 0;
            WRITE a;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "0");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatCountdownLoop) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 0;
            REPEAT
                WRITE a;
                a := a - 1;
            UNTIL a = b;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 10);
        EXPECT_EQ(output[0], "10");
        EXPECT_EQ(output[9], "1");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, NestedRepeatWithWriteInInner) {
    std::string source_code = R"(
        PROGRAM IS
        i, j
        IN
            i := 0;
            REPEAT
                i := i + 1;
                j := 0;
                REPEAT
                    j := j + 1;
                    WRITE j;
                UNTIL j = 2;
            UNTIL i = 2;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 4);
        EXPECT_EQ(output[0], "1");
        EXPECT_EQ(output[1], "2");
        EXPECT_EQ(output[2], "1");
        EXPECT_EQ(output[3], "2");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatWithLargeIterationCount) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 100;
            REPEAT
                a := a + 1;
            UNTIL a = b;
            WRITE a;
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

TEST_F(RepeatTest, RepeatExecutesAtLeastOnce_ConditionTrueInitially) {
    std::string source_code = R"(
        PROGRAM IS
        a, count
        IN
            a := 10;
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL a = 10;
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

TEST_F(RepeatTest, RepeatExecutesAtLeastOnce_ConditionAlwaysTrue) {
    std::string source_code = R"(
        PROGRAM IS
        count
        IN
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL 1 = 1;
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

TEST_F(RepeatTest, RepeatVsWhileDifference) {
    std::string source_code = R"(
        PROGRAM IS
        while_count, repeat_count
        IN
            while_count := 0;
            WHILE 1 = 0 DO
                while_count := while_count + 1;
            ENDWHILE
            
            repeat_count := 0;
            REPEAT
                repeat_count := repeat_count + 1;
            UNTIL 1 = 1;
            
            WRITE while_count;
            WRITE repeat_count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 2);
        EXPECT_EQ(output[0], "0");
        EXPECT_EQ(output[1], "1");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(RepeatTest, RepeatWithAllConditionTypes) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, count
        IN
            a := 5;
            b := 5;
            count := 0;
            
            REPEAT
                count := count + 1;
            UNTIL a = b;
            WRITE count;
            
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL a != 10;
            WRITE count;
            
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL a < 10;
            WRITE count;
            
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL a <= b;
            WRITE count;
            
            count := 0;
            REPEAT
                count := count + 1;
            UNTIL a >= b;
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "1");
        EXPECT_EQ(output[1], "1");
        EXPECT_EQ(output[2], "1");
        EXPECT_EQ(output[3], "1");
        EXPECT_EQ(output[4], "1");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}
