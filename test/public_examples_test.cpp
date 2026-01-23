#include "compiler_test.hpp"
#include "public_examples.hpp"

class PublicExamplesTest : public CompilerTest {};

TEST_F(PublicExamplesTest, Ex1_Simple) {
    std::string source_code = static_cast<std::string>(PublicExamples::example1);
    std::vector<std::string> input_data = { "30", "12" };
    auto output = compile_and_run(source_code, input_data);
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "2");
    EXPECT_EQ(output[2], "6");
}

TEST_F(PublicExamplesTest, Ex1_Fibonacci_Medium) {
    std::string source_code = static_cast<std::string>(PublicExamples::example1);
    std::vector<std::string> input_data = { "21", "13" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "5");
    EXPECT_EQ(output[1], "8");
    EXPECT_EQ(output[2], "1");
}

TEST_F(PublicExamplesTest, Ex1_Fibonacci_Hard) {
    std::string source_code = static_cast<std::string>(PublicExamples::example1);
    std::vector<std::string> input_data = { "89", "55" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "34");
    EXPECT_EQ(output[1], "55");
    EXPECT_EQ(output[2], "1");
}

TEST_F(PublicExamplesTest, Ex1_Edge_Diff) {
    std::string source_code = static_cast<std::string>(PublicExamples::example1);
    std::vector<std::string> input_data = { "100", "1" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "99");
    EXPECT_EQ(output[2], "1");
}

TEST_F(PublicExamplesTest, Ex2) {
    std::string source_code = static_cast<std::string>(PublicExamples::example2);
    std::vector<std::string> input_data = { "0", "1" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "46368");
    EXPECT_EQ(output[1], "28657");
}

TEST_F(PublicExamplesTest, Ex3) {
    std::string source_code = static_cast<std::string>(PublicExamples::example3);
    std::vector<std::string> input_data = { "1" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "121393");
}

TEST_F(PublicExamplesTest, Ex4) {
    std::string source_code = static_cast<std::string>(PublicExamples::example4);
    std::vector<std::string> input_data = { "20", "9" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "167960");
}

TEST_F(PublicExamplesTest, Ex5) {
    std::string source_code = static_cast<std::string>(PublicExamples::example5);
    std::vector<std::string> input_data = { "1234567890", "1234567890987654321", "987654321" };
    
    auto output = compile_and_run(source_code, input_data);
    
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "674106858");
}

TEST_F(PublicExamplesTest, Ex6) {
    std::string source_code = static_cast<std::string>(PublicExamples::example6);
    std::vector<std::string> input_data = { "20" };

    auto output = compile_and_run(source_code, input_data);

    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "2432902008176640000");
    EXPECT_EQ(output[1], "6765");
}

TEST_F(PublicExamplesTest, Ex7_A) {
    std::string source_code = static_cast<std::string>(PublicExamples::example7);
    std::vector<std::string> input_data = { "0", "0", "0" };

    auto output = compile_and_run(source_code, input_data);

    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "31000");
    EXPECT_EQ(output[1], "40900");
    EXPECT_EQ(output[2], "2222010");
}

TEST_F(PublicExamplesTest, Ex7_B) {
    std::string source_code = static_cast<std::string>(PublicExamples::example7);
    std::vector<std::string> input_data = { "1", "0", "2" };

    auto output = compile_and_run(source_code, input_data);

    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "31001");
    EXPECT_EQ(output[1], "40900");
    EXPECT_EQ(output[2], "2222012");
}

TEST_F(PublicExamplesTest, Ex9) {
    std::string source_code = static_cast<std::string>(PublicExamples::example9);
    std::vector<std::string> input_data = { "20", "9" };

    auto output = compile_and_run(source_code, input_data);

    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "167960");
}
