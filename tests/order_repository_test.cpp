#include <gtest/gtest.h>
#include "../order_repository/order_repository.h"

TEST(order_repository_test, test_insert) {
    OrderRepository repo; 

    // insert valid order
    EXPECT_TRUE(repo.insert(Order{"APPL", 1, Side::BUY, 40, 3})); 
    
    // insert invalid orders
    EXPECT_FALSE(repo.insert(Order{"", 1, Side::BUY, 40, 3})); // empty symbol
    EXPECT_FALSE(repo.insert(Order("APPL", 0, Side::BUY, 40, 3))); // order_id == 0
    EXPECT_FALSE(repo.insert(Order("APPL", 1, Side::BUY, -1, 3))); // price < 0
    EXPECT_FALSE(repo.insert(Order("APPL", 1, Side::BUY, 40, -1))); // quantity < 0
    EXPECT_FALSE(repo.insert(Order("AMZN", 1, Side::BUY, 30, 3))); // non-unique order_id
}

TEST(order_repository_test, test_find) {
    OrderRepository repo; 

    Order order1{"APPL", 1, Side::BUY, 40, 3}; 
    Order order2{"APPL", 2, Side::BUY, 40, 3}; 
    repo.insert(order1); 
    repo.insert(order2); 

    // finding valid orders
    ASSERT_TRUE(repo.find(order1).has_value()); 
    ASSERT_EQ(*(repo.find(order1)), 0); 
    ASSERT_TRUE(repo.find(order2).has_value()); 
    ASSERT_EQ(*(repo.find(order2)), 1); 

    // finding invalid orders
    ASSERT_FALSE(repo.find(Order{"APPL", 3, Side::BUY, 40, 3}).has_value()); // invalid order_id
    ASSERT_FALSE(repo.find(Order{"APPL", 2, Side::BUY, 80, 90}).has_value()); // valid order_id but doesn't match
}

