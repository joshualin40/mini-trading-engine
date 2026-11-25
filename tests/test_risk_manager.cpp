#include <gtest/gtest.h> 
#include "../risk-manager/risk_manager.h"
TEST(risk_manager_test, maxNotional) {
    SimpleRiskManager simple(100, 40); 
    TradingSystem simpleSystem(simple); 

    // Reject overflowing Notional
    ASSERT_FALSE(simpleSystem.submit(Order{1, "AAPL", Side::BUY, 50, 100})); 
    ASSERT_FALSE(simpleSystem.submit(Order{2, "AAPL", Side::BUY, 41, 100})); 
}
TEST(risk_manager_test, maxQuantity) {
    SimpleRiskManager simple(100, 40); 
    TradingSystem simpleSystem(simple); 

    // Reject overflowing maxQuantity 
    ASSERT_FALSE(simpleSystem.submit(Order{1, "AAPL", Side::BUY, 39, 101})); 
    ASSERT_FALSE(simpleSystem.submit(Order{2, "AAPL", Side::BUY, 39, 105})); 
    ASSERT_FALSE(simpleSystem.submit(Order{3, "AAPL", Side::BUY, 39, 106}));
} 

TEST(risk_manager_test, repeated_calls) {
    SimpleRiskManager simple(100, 40); 
    TradingSystem simpleSystem(simple); 

    Order invalid{1, "AAPL", Side::BUY, 39, 101};
    ASSERT_FALSE(simpleSystem.submit(invalid)); 
    ASSERT_FALSE(simpleSystem.submit(invalid)); 

    Order valid{2, "AAPL", Side::BUY, 30, 80};
    ASSERT_TRUE(simpleSystem.submit(valid)); 
    ASSERT_TRUE(simpleSystem.submit(valid)); 
}

TEST(risk_manager_test, rejects_infinite_arguments) {
    EXPECT_THROW(SimpleRiskManager(std::numeric_limits<Quantity>::infinity(), 40), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(100,std::numeric_limits<Price>::infinity()), std::invalid_argument);
}

TEST(risk_manager_test, reject_floating_point_arguments) {
    // SimpleRiskManager simple(100.5, 40); 
}

TEST(risk_manager_test, reject_zero_arguments) {
   EXPECT_THROW(SimpleRiskManager(100,0), std::invalid_argument);
   EXPECT_THROW(SimpleRiskManager(0,100), std::invalid_argument);
   EXPECT_THROW(SimpleRiskManager(0,0), std::invalid_argument); 
}

TEST(risk_manager_test, negative_arguments) {
    EXPECT_THROW(SimpleRiskManager(-1,3), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(3,-1), std::invalid_argument); 
    EXPECT_THROW(SimpleRiskManager(-1,-1), std::invalid_argument); 
}

TEST(risk_manager_test, fake_max_quantity) {
    FakeRiskManager fake; 
    TradingSystem fakeSystem(fake); 

    ASSERT_FALSE(fakeSystem.submit(Order{1, "AAPL", Side::BUY, 101, 39})); 
    ASSERT_FALSE(fakeSystem.submit(Order{2, "AAPL", Side::BUY, 99, 41})); 
    ASSERT_FALSE(fakeSystem.submit(Order{3, "AAPL", Side::BUY, 99, 39})); 
}

TEST(risk_manager_test, base_manager) {
    EXPECT_THROW(SimpleRiskManager(-1, 40), std::invalid_argument);
}

TEST(risk_manager_test, risk_manager_ownership) {
    SimpleRiskManager* p = new SimpleRiskManager(100,400); 
    TradingSystem simpleSystem(p); 
}
