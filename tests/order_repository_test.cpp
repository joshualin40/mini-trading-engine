#include <gtest/gtest.h>
#include "../order_repository/order_repository.h"
#include <limits>
#include <vector>

TEST(order_repository_test, test_insert) {
    OrderRepository repo; 

    // insert valid order
    EXPECT_TRUE(repo.insert(Order{1, "APPL", Side::BUY, 40, 3})); 
    
    // insert invalid orders
    EXPECT_FALSE(repo.insert(Order{1, "", Side::BUY, 40, 3})); // empty symbol
    EXPECT_FALSE(repo.insert(Order(0, "APPL", Side::BUY, 40, 3))); // order_id == 0
    EXPECT_FALSE(repo.insert(Order(1, "APPL", Side::BUY, -1, 3))); // price < 0
    EXPECT_FALSE(repo.insert(Order(1, "APPL", Side::BUY, 40, -1))); // quantity < 0
    EXPECT_FALSE(repo.insert(Order(1, "AMZN", Side::BUY, 30, 3))); // non-unique order_id
}

TEST(order_repository_test, test_find) {
    OrderRepository repo; 

    Order order1{1, "APPL", Side::BUY, 40, 3}; 
    Order order2{2, "APPL", Side::BUY, 40, 3}; 
    repo.insert(order1); 
    repo.insert(order2); 

    // finding valid orders
    ASSERT_TRUE(repo.find(1).has_value()); 
    ASSERT_EQ(*(repo.find(1).value()), order1); 
    ASSERT_TRUE(repo.find(2).has_value()); 
    ASSERT_EQ(*(repo.find(2).value()), order2); 

    // finding invalid orders
    ASSERT_FALSE(repo.find(3).has_value()); // invalid order_id
}

// TEST(order_repository_test, test_erase) {
//     OrderRepository repo; 

//     Order order1{1, "APPL", Side::BUY, 40, 3}; 
//     Order order2{2, "APPL", Side::BUY, 40, 3}; 
//     Order order3{3, "APPL", Side::BUY, 40, 3}; 
//     repo.insert(order1); 
//     repo.insert(order2); 
//     repo.insert(order3); 

//     // 1 2 3
//     // remove front
//     repo.erase(1); 
//     ASSERT_FALSE(repo.find(1).has_value()); 
//     // add to back
//     repo.insert(order1); 
//     ASSERT_EQ(repo.find(1).value(), 2); 

//     // 2 3 1
//     // remove middle 
//     repo.erase(3); 
//     ASSERT_FALSE(repo.find(3).has_value()); 
//     // add back
//     repo.insert(order3); 
//     // 2 1 3
//     ASSERT_EQ(repo.find(3).value(), 2); 

//     // 2 1 3
//     // remove back 
//     repo.erase(3); 
//     ASSERT_FALSE(repo.find(3).has_value()); 
// }

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

namespace {
// Collects the order ids from all() in storage order.
std::vector<OrderId> ids_of(const OrderRepository &repo) {
    std::vector<OrderId> ids;
    for (const auto &order : repo.all())
        ids.push_back(order.order_id);
    return ids;
}
} // namespace

TEST(order_repository_edge_test, empty_repository) {
    OrderRepository repo;

    EXPECT_TRUE(repo.all().empty());
    EXPECT_FALSE(repo.find(1).has_value());
    EXPECT_FALSE(repo.find(0).has_value());
    EXPECT_FALSE(repo.erase(1));
    EXPECT_FALSE(repo.erase(0));
    EXPECT_TRUE(repo.all().empty());
}

TEST(order_repository_edge_test, max_order_id_is_valid) {
    OrderRepository repo;
    constexpr OrderId max_id = std::numeric_limits<OrderId>::max();

    EXPECT_TRUE(repo.insert(Order{max_id, "APPL", Side::BUY, 40, 3}));
    ASSERT_TRUE(repo.find(max_id).has_value());
    EXPECT_EQ(repo.find(max_id).value()->order_id, max_id);
    EXPECT_TRUE(repo.erase(max_id));
    EXPECT_FALSE(repo.find(max_id).has_value());
}

TEST(order_repository_edge_test, boundary_price_and_quantity) {
    OrderRepository repo;
    constexpr uint64_t max_val = std::numeric_limits<uint64_t>::max();

    EXPECT_TRUE(repo.insert(Order{1, "APPL", Side::BUY, max_val, 3}));
    EXPECT_TRUE(repo.insert(Order{2, "APPL", Side::BUY, 40, max_val}));

    EXPECT_EQ(repo.find(1).value()->price, max_val);
    EXPECT_EQ(repo.find(2).value()->quantity, max_val);
}

TEST(order_repository_edge_test, side_is_preserved) {
    OrderRepository repo;

    repo.insert(Order{1, "APPL", Side::BUY, 40, 3});
    repo.insert(Order{2, "APPL", Side::SELL, 40, 3});

    EXPECT_EQ(repo.find(1).value()->side, Side::BUY);
    EXPECT_EQ(repo.find(2).value()->side, Side::SELL);
}

TEST(order_repository_edge_test, rejected_duplicate_does_not_overwrite_original) {
    OrderRepository repo;
    Order original{1, "APPL", Side::BUY, 40, 3};

    ASSERT_TRUE(repo.insert(original));
    EXPECT_FALSE(repo.insert(Order{1, "AMZN", Side::BUY, 99, 7}));

    ASSERT_TRUE(repo.find(1).has_value());
    EXPECT_EQ(repo.find(1).value()->symbol, "APPL");
    EXPECT_EQ(repo.find(1).value()->price, 40u);
    EXPECT_EQ(repo.find(1).value()->quantity, 3u);
    EXPECT_EQ(repo.all().size(), 1u);
}

TEST(order_repository_edge_test, rejected_inserts_leave_repository_unchanged) {
    OrderRepository repo;

    EXPECT_FALSE(repo.insert(Order{0, "APPL", Side::BUY, 40, 3})); // id 0
    EXPECT_FALSE(repo.insert(Order{5, "", Side::BUY, 40, 3}));     // empty symbol

    EXPECT_TRUE(repo.all().empty());
    EXPECT_FALSE(repo.find(0).has_value());
    EXPECT_FALSE(repo.find(5).has_value());

    // id 5 was never stored, so it can still be used
    EXPECT_TRUE(repo.insert(Order{5, "APPL", Side::BUY, 40, 3}));
}

TEST(order_repository_edge_test, all_preserves_insertion_order) {
    OrderRepository repo;

    repo.insert(Order{3, "APPL", Side::BUY, 40, 3});
    repo.insert(Order{1, "APPL", Side::BUY, 40, 3});
    repo.insert(Order{2, "APPL", Side::BUY, 40, 3});

    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{3, 1, 2}));
}

TEST(order_repository_edge_test, all_returns_independent_copy) {
    OrderRepository repo;
    repo.insert(Order{1, "APPL", Side::BUY, 40, 3});

    auto snapshot = repo.all();
    snapshot.front().quantity = 999;
    snapshot.clear();

    ASSERT_EQ(repo.all().size(), 1u);
    EXPECT_EQ(repo.find(1).value()->quantity, 3u);
}

TEST(order_repository_edge_test, erase_same_id_twice) {
    OrderRepository repo;
    repo.insert(Order{1, "APPL", Side::BUY, 40, 3});

    EXPECT_TRUE(repo.erase(1));
    EXPECT_FALSE(repo.erase(1));
    EXPECT_TRUE(repo.all().empty());
}

TEST(order_repository_edge_test, erase_only_element_then_reinsert) {
    OrderRepository repo;

    ASSERT_TRUE(repo.insert(Order{1, "APPL", Side::BUY, 40, 3}));
    ASSERT_TRUE(repo.erase(1));
    EXPECT_TRUE(repo.all().empty());

    // the id is free again and the new data is what gets stored
    EXPECT_TRUE(repo.insert(Order{1, "AMZN", Side::SELL, 50, 4}));
    ASSERT_TRUE(repo.find(1).has_value());
    EXPECT_EQ(repo.find(1).value()->symbol, "AMZN");
    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{1}));
}

TEST(order_repository_edge_test, erase_front_middle_back) {
    OrderRepository repo;
    for (OrderId id = 1; id <= 5; ++id)
        repo.insert(Order{id, "APPL", Side::BUY, 40, 3});

    EXPECT_TRUE(repo.erase(1)); // front
    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{2, 3, 4, 5}));

    EXPECT_TRUE(repo.erase(3)); // middle
    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{2, 4, 5}));

    EXPECT_TRUE(repo.erase(5)); // back
    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{2, 4}));

    // remaining orders are still reachable through the lookup
    EXPECT_EQ(repo.find(2).value()->order_id, 2u);
    EXPECT_EQ(repo.find(4).value()->order_id, 4u);
}

TEST(order_repository_edge_test, reinserted_order_goes_to_back) {
    OrderRepository repo;
    for (OrderId id = 1; id <= 3; ++id)
        repo.insert(Order{id, "APPL", Side::BUY, 40, 3});

    repo.erase(1);
    repo.insert(Order{1, "APPL", Side::BUY, 40, 3});

    EXPECT_EQ(ids_of(repo), (std::vector<OrderId>{2, 3, 1}));
}

TEST(order_repository_edge_test, find_iterator_survives_other_erases) {
    OrderRepository repo;
    for (OrderId id = 1; id <= 3; ++id)
        repo.insert(Order{id, "APPL", Side::BUY, 40, 3});

    auto it = repo.find(2).value();
    repo.erase(1);
    repo.erase(3);
    repo.insert(Order{4, "APPL", Side::BUY, 40, 3});

    // std::list iterators stay valid when other elements are removed or added
    EXPECT_EQ(it->order_id, 2u);
    EXPECT_EQ(repo.find(2).value(), it);
}

TEST(order_repository_edge_test, many_inserts_and_erases_stay_consistent) {
    OrderRepository repo;
    constexpr OrderId n = 1000;

    for (OrderId id = 1; id <= n; ++id)
        ASSERT_TRUE(repo.insert(Order{id, "APPL", Side::BUY, id, id}));

    for (OrderId id = 2; id <= n; id += 2)
        ASSERT_TRUE(repo.erase(id));

    std::vector<OrderId> expected;
    for (OrderId id = 1; id <= n; id += 2)
        expected.push_back(id);
    EXPECT_EQ(ids_of(repo), expected);

    for (OrderId id = 1; id <= n; ++id) {
        auto found = repo.find(id);
        if (id % 2 == 0) {
            EXPECT_FALSE(found.has_value()) << "id " << id;
        } else {
            ASSERT_TRUE(found.has_value()) << "id " << id;
            EXPECT_EQ(found.value()->price, id);
        }
    }
}
