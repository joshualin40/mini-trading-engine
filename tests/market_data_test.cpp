#include <gtest/gtest.h>
#include "../market_data/market_data.h"
#include <print>

TEST(market_data_test, test_subscribe) {
    MarketData md; 
    std::vector<int> called(2, 0); 
    auto myFunc = [&](const Quote&) {
        std::print("Got Quote"); 
         called[0]++;
    };
    auto myFunc2 = [&](const Quote&) {
        std::print("Got Quote 2");
        called[1]++;
    };
    
    ASSERT_EQ(md.subscribe(myFunc), 0); 
    ASSERT_EQ(md.subscribe(myFunc2), 1); 

    md.publish(Quote("APPL", 2, 3, 0));
    EXPECT_EQ(called[0], 1);
    EXPECT_EQ(called[1], 1); 

    EXPECT_THROW(md.subscribe(nullptr), std::invalid_argument);
    md.publish(Quote("APPL", 4, 5, 1));

    EXPECT_EQ(called[0], 2);
    EXPECT_EQ(called[1], 2); 
}