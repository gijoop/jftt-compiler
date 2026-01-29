#include "compiler_test.hpp"

class WriteTest : public CompilerTest {};

TEST_F(WriteTest, SimpleWrite1) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 0;
    END
  )";
  
  auto output = compile_and_run(source_code);
  EXPECT_EQ(output[0], "0");
}

TEST_F(WriteTest, SimpleWrite2) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 42;
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

TEST_F(WriteTest, SimpleWrite3) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 7;
        WRITE 15;
        WRITE 23;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "7");
    EXPECT_EQ(output[1], "15");
    EXPECT_EQ(output[2], "23");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, WriteLargeNumber) {
  std::string source_code = R"(
    PROGRAM IS
    IN
        WRITE 123456789;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "123456789");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, WriteVariable) {
  std::string source_code = R"(
    PROGRAM IS
    x
    IN
        x := 999;
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "999");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, WriteManyValues) {
  std::string source_code = R"(
    PROGRAM IS
    i
    IN
        i := 0;
        WHILE i < 10 DO
            WRITE i;
            i := i + 1;
        ENDWHILE
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 10);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[9], "9");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, CommentAtEndOfLine) {
  std::string source_code = R"(
    PROGRAM IS
    x # this declares x
    IN
        x := 42; # set x to 42
        WRITE x; # output x
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

TEST_F(WriteTest, CommentOnOwnLine) {
  std::string source_code = R"(
    # This is a test program
    PROGRAM IS
    # Variable declarations
    x
    # Begin main block
    IN
        # Assignment
        x := 100;
        # Output
        WRITE x;
    # End program
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

TEST_F(WriteTest, CommentWithSpecialCharacters) {
  std::string source_code = R"(
    PROGRAM IS
    x # comment with := and + - * / % symbols
    IN
        x := 77; # more special chars: [0:10] IF THEN ELSE
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "77");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, EmptyComments) {
  std::string source_code = R"(
    PROGRAM IS #
    x #
    IN #
        x := 55; #
        WRITE x; #
    END #
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

TEST_F(WriteTest, CommentsInProcedure) {
  std::string source_code = R"(
    # Procedure definition
    PROCEDURE test(x) IS # x is a parameter
    IN
        # Write the value
        WRITE x;
    END
  
    # Main program
    PROGRAM IS
    a # our variable
    IN
        a := 123;
        test(a); # call the procedure
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "123");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}

TEST_F(WriteTest, MultipleHashesInComment) {
  std::string source_code = R"(
    PROGRAM IS
    x # this is ## a comment with # multiple hashes ###
    IN
        x := 88;
        WRITE x;
    END
  )";
  
  try {
    auto output = compile_and_run(source_code);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "88");
  } catch (const SemanticError& e) {
    FAIL() << "Semantic error: " << e.what();
  } catch (const std::exception& e) {
    FAIL() << "Error: " << e.what();
  }
}
