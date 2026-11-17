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
    ASSERT_TRUE(repo.find(1).has_value()); 
    ASSERT_EQ(*(repo.find(1)), 0); 
    ASSERT_TRUE(repo.find(2).has_value()); 
    ASSERT_EQ(*(repo.find(2)), 1); 

    // finding invalid orders
    ASSERT_FALSE(repo.find(3).has_value()); // invalid order_id
}

TEST(order_repository_test, test_erase) {
    OrderRepository repo; 

    Order order1{"APPL", 1, Side::BUY, 40, 3}; 
    Order order2{"APPL", 2, Side::BUY, 40, 3}; 
    Order order3{"APPL", 3, Side::BUY, 40, 3}; 
    repo.insert(order1); 
    repo.insert(order2); 
    repo.insert(order3); 

    // 1 2 3
    // remove front
    repo.erase(1); 
    ASSERT_FALSE(repo.find(1).has_value()); 
    // add to back
    repo.insert(order1); 
    ASSERT_EQ(repo.find(1).value(), 2); 

    // 2 3 1
    // remove middle 
    repo.erase(3); 
    ASSERT_FALSE(repo.find(3).has_value()); 
    // add back
    repo.insert(order3); 
    // 2 1 3
    ASSERT_EQ(repo.find(3).value(), 2); 

    // 2 1 3
    // remove back 
    repo.erase(3); 
    ASSERT_FALSE(repo.find(3).has_value()); 
}