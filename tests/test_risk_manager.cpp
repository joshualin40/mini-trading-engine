#include <gtest/gtest.h> 
#include "../risk-manager/risk_manager.h"
TEST(risk_manager_test, fake_risk_manager) {
    SimpleRiskManager simple(100, 40); 
    TradingSystem simpleSystem(simple); 

    ASSERT_FALSE(simpleSystem.submit(Order{1, "AAPL", Side::BUY, 39, 101})); 
    ASSERT_FALSE(simpleSystem.submit(Order{2, "AAPL", Side::BUY, 41, 99})); 
    ASSERT_TRUE(simpleSystem.submit(Order{3, "AAPL", Side::BUY, 39, 99})); 

    FakeRiskManager fake; 
    TradingSystem fakeSystem(fake); 

    ASSERT_FALSE(fakeSystem.submit(Order{1, "AAPL", Side::BUY, 101, 39})); 
    ASSERT_FALSE(fakeSystem.submit(Order{2, "AAPL", Side::BUY, 99, 41})); 
    ASSERT_FALSE(fakeSystem.submit(Order{3, "AAPL", Side::BUY, 99, 39})); 
}