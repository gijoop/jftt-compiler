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

// ============================================================================
// ListA Tests - Binary conversion
// ============================================================================

TEST_F(PublicExamplesTest, ListA_Five) {
    std::string source_code = static_cast<std::string>(PublicExamples::listA);
    std::vector<std::string> input_data = { "5" };

    auto output = compile_and_run(source_code, input_data);

    // 5 = 101 in binary (LSB first: 1, 0, 1)
    ASSERT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], "1");
    EXPECT_EQ(output[1], "0");
    EXPECT_EQ(output[2], "1");
}

TEST_F(PublicExamplesTest, ListA_Eight) {
    std::string source_code = static_cast<std::string>(PublicExamples::listA);
    std::vector<std::string> input_data = { "8" };

    auto output = compile_and_run(source_code, input_data);

    // 8 = 1000 in binary (LSB first: 0, 0, 0, 1)
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "0");
    EXPECT_EQ(output[1], "0");
    EXPECT_EQ(output[2], "0");
    EXPECT_EQ(output[3], "1");
}

TEST_F(PublicExamplesTest, ListA_255) {
    std::string source_code = static_cast<std::string>(PublicExamples::listA);
    std::vector<std::string> input_data = { "255" };

    auto output = compile_and_run(source_code, input_data);

    // 255 = 11111111 in binary
    ASSERT_EQ(output.size(), 8);
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(output[i], "1");
    }
}

// ============================================================================
// ListB Tests - GCD
// ============================================================================

TEST_F(PublicExamplesTest, ListB_Simple) {
    std::string source_code = static_cast<std::string>(PublicExamples::listB);
    std::vector<std::string> input_data = { "12", "8", "15", "10" };

    auto output = compile_and_run(source_code, input_data);

    // gcd(12,8)=4, gcd(15,10)=5, gcd(4,5)=1
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
}

TEST_F(PublicExamplesTest, ListB_Fibonacci) {
    std::string source_code = static_cast<std::string>(PublicExamples::listB);
    std::vector<std::string> input_data = { "21", "13", "34", "55" };

    auto output = compile_and_run(source_code, input_data);

    // Consecutive Fibonacci numbers are coprime
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "1");
}

// ============================================================================
// ListC Tests - Sieve of Eratosthenes (primes up to 100)
// ============================================================================

TEST_F(PublicExamplesTest, ListC_Primes) {
    std::string source_code = static_cast<std::string>(PublicExamples::listC);
    std::vector<std::string> input_data = {};

    auto output = compile_and_run(source_code, input_data);

    // Primes up to 100 in descending order: 97, 89, 83, 79, 73, 71, 67, 61, 59, 53, 47, 43, 41, 37, 31, 29, 23, 19, 17, 13, 11, 7, 5, 3, 2
    ASSERT_EQ(output.size(), 25);
    EXPECT_EQ(output[0], "97");
    EXPECT_EQ(output[1], "89");
    EXPECT_EQ(output[output.size() - 1], "2");
}

// ============================================================================
// ListD Tests - Prime factorization
// ============================================================================

TEST_F(PublicExamplesTest, ListD_Example1) {
    std::string source_code = static_cast<std::string>(PublicExamples::listD);
    std::vector<std::string> input_data = { "1234567890" };

    auto output = compile_and_run(source_code, input_data);

    // 1234567890 = 2 * 3^2 * 5 * 3607 * 3803
    ASSERT_EQ(output.size(), 10);
    EXPECT_EQ(output[0], "2");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "3");
    EXPECT_EQ(output[3], "2");
    EXPECT_EQ(output[4], "5");
    EXPECT_EQ(output[5], "1");
    EXPECT_EQ(output[6], "3607");
    EXPECT_EQ(output[7], "1");
    EXPECT_EQ(output[8], "3803");
    EXPECT_EQ(output[9], "1");
}

TEST_F(PublicExamplesTest, ListD_Example2) {
    std::string source_code = static_cast<std::string>(PublicExamples::listD);
    std::vector<std::string> input_data = { "12345678901" };

    auto output = compile_and_run(source_code, input_data);

    // 12345678901 = 857 * 14405693
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "857");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "14405693");
    EXPECT_EQ(output[3], "1");
}

TEST_F(PublicExamplesTest, ListD_Example3) {
    std::string source_code = static_cast<std::string>(PublicExamples::listD);
    std::vector<std::string> input_data = { "12345678903" };

    auto output = compile_and_run(source_code, input_data);

    // 12345678903 = 3 * 4115226301
    ASSERT_EQ(output.size(), 4);
    EXPECT_EQ(output[0], "3");
    EXPECT_EQ(output[1], "1");
    EXPECT_EQ(output[2], "4115226301");
    EXPECT_EQ(output[3], "1");
}

TEST_F(PublicExamplesTest, ListD_Prime) {
    std::string source_code = static_cast<std::string>(PublicExamples::listD);
    std::vector<std::string> input_data = { "97" };

    auto output = compile_and_run(source_code, input_data);

    // 97 is prime
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "97");
    EXPECT_EQ(output[1], "1");
}

TEST_F(PublicExamplesTest, ListD_PowerOfTwo) {
    std::string source_code = static_cast<std::string>(PublicExamples::listD);
    std::vector<std::string> input_data = { "1024" };

    auto output = compile_and_run(source_code, input_data);

    // 1024 = 2^10
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], "2");
    EXPECT_EQ(output[1], "10");
}
