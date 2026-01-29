#include "compiler_test.hpp"

class WhileTest : public CompilerTest {};

TEST_F(WhileTest, SimpleWhileLoop) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 5;
            WHILE a < b DO
                a := a + 1;
            ENDWHILE
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

TEST_F(WhileTest, WhileLoopZeroIterations) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 5;
            WHILE a < b DO
                a := a + 1;
            ENDWHILE
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

TEST_F(WhileTest, WhileLoopMultipleIterations) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 0;
            b := 3;
            c := 0;
            WHILE a < b DO
                c := c + 2;
                a := a + 1;
            ENDWHILE
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

TEST_F(WhileTest, NestedWhileLoops) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c
        IN
            a := 0;
            b := 2;
            c := 0;
            WHILE a < b DO
                a := a + 1;
                WHILE c < 3 DO
                    c := c + 1;
                ENDWHILE
            ENDWHILE
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

TEST_F(WhileTest, WhileInternalWrite) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 3;
            WHILE a < b DO
                a := a + 1;
                WRITE a;
            ENDWHILE
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

TEST_F(WhileTest, WhileInternalFalseConditionWrite) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 5;
            b := 3;
            WHILE a < b DO
                a := a + 1;
                WRITE a;
            ENDWHILE
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 0);
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(WhileTest, WhileWithGreaterThan) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 5;
            WHILE a > b DO
                a := a - 1;
            ENDWHILE
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

TEST_F(WhileTest, WhileWithGreaterOrEqual) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 10;
            b := 5;
            WHILE a >= b DO
                a := a - 1;
            ENDWHILE
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

TEST_F(WhileTest, WhileWithNotEqual) {
    std::string source_code = R"(
        PROGRAM IS
        a, b
        IN
            a := 0;
            b := 5;
            WHILE a != b DO
                a := a + 1;
            ENDWHILE
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

TEST_F(WhileTest, WhileCountdown) {
    std::string source_code = R"(
        PROGRAM IS
        a
        IN
            a := 5;
            WHILE a > 0 DO
                WRITE a;
                a := a - 1;
            ENDWHILE
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 5);
        EXPECT_EQ(output[0], "5");
        EXPECT_EQ(output[4], "1");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(WhileTest, WhileAccumulation) {
    std::string source_code = R"(
        PROGRAM IS
        i, sum
        IN
            i := 1;
            sum := 0;
            WHILE i <= 100 DO
                sum := sum + i;
                i := i + 1;
            ENDWHILE
            WRITE sum;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "5050");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(WhileTest, DeeplyNestedWhile) {
    std::string source_code = R"(
        PROGRAM IS
        i, j, k, count
        IN
            i := 0;
            count := 0;
            WHILE i < 2 DO
                j := 0;
                WHILE j < 2 DO
                    k := 0;
                    WHILE k < 2 DO
                        count := count + 1;
                        k := k + 1;
                    ENDWHILE
                    j := j + 1;
                ENDWHILE
                i := i + 1;
            ENDWHILE
            WRITE count;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 1);
        EXPECT_EQ(output[0], "8");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(WhileTest, WhileWithComplexCondition) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, diff
        IN
            a := 1;
            b := 100;
            WHILE a < b DO
                diff := b - a;
                a := a + 1;
                b := b - 1;
            ENDWHILE
            WRITE a;
            WRITE b;
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 2);
        EXPECT_EQ(output[0], "51");
        EXPECT_EQ(output[1], "50");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}

TEST_F(WhileTest, WhileFibonacci) {
    std::string source_code = R"(
        PROGRAM IS
        a, b, c, count
        IN
            a := 1;
            b := 1;
            count := 0;
            WHILE count < 10 DO
                c := a + b;
                a := b;
                b := c;
                count := count + 1;
                WRITE c;
            ENDWHILE
        END
    )";
    
    try {
        auto output = compile_and_run(source_code);
        ASSERT_EQ(output.size(), 10);
        EXPECT_EQ(output[0], "2");
        EXPECT_EQ(output[1], "3");
        EXPECT_EQ(output[2], "5");
        EXPECT_EQ(output[3], "8");
        EXPECT_EQ(output[4], "13");
        EXPECT_EQ(output[5], "21");
        EXPECT_EQ(output[6], "34");
        EXPECT_EQ(output[7], "55");
        EXPECT_EQ(output[8], "89");
        EXPECT_EQ(output[9], "144");
    } catch (const SemanticError& e) {
        FAIL() << "Semantic error: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Error: " << e.what();
    }
}