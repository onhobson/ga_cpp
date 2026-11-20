// #include "ga/utility/random.hpp"

// #include <gtest/gtest.h>

// void verifyGetInteger(ga::Random& rng)
// {
//     const int value = rng.get(0, 10);
//     EXPECT_GE(value, 0);
//     EXPECT_LE(value, 10);
// }

// void verifyGetFloating(ga::Random& rng)
// {
//     const double value = rng.get(0.0, 1.0);
//     EXPECT_GE(value, 0.0);
//     EXPECT_LT(value, 1.0);
// }

// TEST(EngineTest, MtEngine)
// {
//     ga::Random rng { std::mt19937 { 12345 } };
//     verifyGetInteger(rng);
//     verifyGetFloating(rng);
// }

// TEST(EngineTest, MinstdRandEngine)
// {
//     ga::Random rng { std::minstd_rand { 12345 } };
//     verifyGetInteger(rng);
//     verifyGetFloating(rng);
// }

// class BarebonesEngine
// {
// public:
//     using result_type = unsigned int;

//     static constexpr result_type min() { return 0; }
//     static constexpr result_type max() { return 255; }

//     result_type operator()()
//     {
//         const result_type result = m_value;
//         m_value = (m_value == max()) ? min() : m_value + 1;
//         return result;
//     }

// private:
//     result_type m_value { min() };
// };

// TEST(EngineTest, CustomURBGEngine)
// {
//     ga::Random rng { BarebonesEngine {} };
//     verifyGetInteger(rng);
//     verifyGetFloating(rng);
// }

// #pragma endregion Different_Engines
