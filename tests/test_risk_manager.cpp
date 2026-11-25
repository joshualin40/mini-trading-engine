#include <gtest/gtest.h>
#include <limits>
#include "../risk-manager/risk_manager.h"

namespace {
constexpr Quantity kMaxQty = 100;
constexpr Price kMaxPrice = 40;

Order makeOrder(OrderId id = 1, const std::string& sym = "AAPL",
                Side side = Side::BUY, Price price = 10, Quantity qty = 10) {
    return Order{id, sym, side, price, qty};
}
} // namespace

// ---------------------------------------------------------------------------
// SimpleRiskManager: basic approval
// ---------------------------------------------------------------------------

TEST(SimpleRiskManagerTest, ApprovesOrderWellWithinLimits) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 10, 10)));
}

TEST(SimpleRiskManagerTest, ApprovesSellOrdersSameAsBuy) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::SELL, 10, 10)));
    EXPECT_FALSE(rm.approve(makeOrder(2, "AAPL", Side::SELL, 10, kMaxQty + 1)));
    EXPECT_FALSE(rm.approve(makeOrder(3, "AAPL", Side::SELL, kMaxPrice + 1, 10)));
}

// ---------------------------------------------------------------------------
// SimpleRiskManager: exact limits (boundaries are inclusive)
// ---------------------------------------------------------------------------

TEST(SimpleRiskManagerTest, ApprovesQuantityExactlyAtLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 10, kMaxQty)));
}

TEST(SimpleRiskManagerTest, RejectsQuantityOneOverLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 10, kMaxQty + 1)));
}

TEST(SimpleRiskManagerTest, ApprovesPriceExactlyAtLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, kMaxPrice, 10)));
}

TEST(SimpleRiskManagerTest, RejectsPriceOneOverLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "AAPL", Side::BUY, kMaxPrice + 1, 10)));
}

TEST(SimpleRiskManagerTest, ApprovesBothValuesExactlyAtLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, kMaxPrice, kMaxQty)));
}

TEST(SimpleRiskManagerTest, RejectsBothValuesOverLimit) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "AAPL", Side::BUY, kMaxPrice + 1, kMaxQty + 1)));
}

// ---------------------------------------------------------------------------
// SimpleRiskManager: malformed orders
// ---------------------------------------------------------------------------

TEST(SimpleRiskManagerTest, RejectsEmptySymbol) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "", Side::BUY, 10, 10)));
}

TEST(SimpleRiskManagerTest, RejectsZeroOrderId) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(0, "AAPL", Side::BUY, 10, 10)));
}

TEST(SimpleRiskManagerTest, ApprovesMaxOrderId) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(std::numeric_limits<OrderId>::max(), "AAPL", Side::BUY, 10, 10)));
}

TEST(SimpleRiskManagerTest, WhitespaceSymbolIsCurrentlyAccepted) {
    // Only "" is rejected; a whitespace-only symbol passes validation.
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, " ", Side::BUY, 10, 10)));
}

TEST(SimpleRiskManagerTest, ZeroQuantityIsCurrentlyAccepted) {
    // validate_order only rejects quantity < 0, so a 0-qty order passes.
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 10, 0)));
}

TEST(SimpleRiskManagerTest, ZeroPriceIsCurrentlyAccepted) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 0, 10)));
}

TEST(SimpleRiskManagerTest, RejectsNegativePrice) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "AAPL", Side::BUY, -1, 10)));
    EXPECT_FALSE(rm.approve(makeOrder(2, "AAPL", Side::BUY, std::numeric_limits<Price>::min(), 10)));
}

TEST(SimpleRiskManagerTest, RejectsNegativeQuantity) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 10, -1)));
    EXPECT_FALSE(rm.approve(makeOrder(2, "AAPL", Side::BUY, 10, std::numeric_limits<Quantity>::min())));
}

TEST(SimpleRiskManagerTest, MalformedOrderRejectedEvenWhenWithinLimits) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_FALSE(rm.approve(makeOrder(0, "", Side::BUY, 1, 1)));
}

// ---------------------------------------------------------------------------
// SimpleRiskManager: unusual limit configurations
// ---------------------------------------------------------------------------

TEST(SimpleRiskManagerTest, ConstructorThrowsOnZeroLimits) {
    EXPECT_THROW(SimpleRiskManager(0, kMaxPrice), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(kMaxQty, 0), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(0, 0), std::invalid_argument);
}

TEST(SimpleRiskManagerTest, ConstructorThrowsOnNegativeLimits) {
    EXPECT_THROW(SimpleRiskManager(-1, kMaxPrice), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(kMaxQty, -1), std::invalid_argument);
    EXPECT_THROW(SimpleRiskManager(std::numeric_limits<Quantity>::min(), kMaxPrice), std::invalid_argument);
}

TEST(SimpleRiskManagerTest, ConstructorAcceptsSmallestValidLimits) {
    EXPECT_NO_THROW(SimpleRiskManager(1, 1));
    SimpleRiskManager rm(1, 1);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, 1, 1)));
    EXPECT_FALSE(rm.approve(makeOrder(2, "AAPL", Side::BUY, 2, 1)));
    EXPECT_FALSE(rm.approve(makeOrder(3, "AAPL", Side::BUY, 1, 2)));
}

TEST(SimpleRiskManagerTest, MaxLimitsApproveMaxValues) {
    constexpr Price kMaxP = std::numeric_limits<Price>::max();
    constexpr Quantity kMaxQ = std::numeric_limits<Quantity>::max();
    EXPECT_NO_THROW(SimpleRiskManager(kMaxQ, kMaxP));
    SimpleRiskManager rm(kMaxQ, kMaxP);
    EXPECT_TRUE(rm.approve(makeOrder(1, "AAPL", Side::BUY, kMaxP, kMaxQ)));
}

TEST(SimpleRiskManagerTest, IsStatelessAcrossCalls) {
    // Approval must not depend on prior orders (no cumulative exposure tracking).
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    for (OrderId id = 1; id <= 5; ++id)
        EXPECT_TRUE(rm.approve(makeOrder(id, "AAPL", Side::BUY, kMaxPrice, kMaxQty)));
    EXPECT_FALSE(rm.approve(makeOrder(6, "AAPL", Side::BUY, kMaxPrice + 1, 1)));
    EXPECT_TRUE(rm.approve(makeOrder(7, "AAPL", Side::BUY, 1, 1)));
}

TEST(SimpleRiskManagerTest, DuplicateOrderIdsAreNotCheckedByRiskManager) {
    SimpleRiskManager rm(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm.approve(makeOrder(1)));
    EXPECT_TRUE(rm.approve(makeOrder(1)));
}

// ---------------------------------------------------------------------------
// Polymorphism via IRiskManager
// ---------------------------------------------------------------------------

TEST(IRiskManagerTest, SimpleRiskManagerUsableThroughInterface) {
    std::unique_ptr<IRiskManager> rm = std::make_unique<SimpleRiskManager>(kMaxQty, kMaxPrice);
    EXPECT_TRUE(rm->approve(makeOrder(1, "AAPL", Side::BUY, 10, 10)));
    EXPECT_FALSE(rm->approve(makeOrder(2, "AAPL", Side::BUY, 10, kMaxQty + 1)));
}

TEST(IRiskManagerTest, FakeRiskManagerUsableThroughInterface) {
    std::unique_ptr<IRiskManager> rm = std::make_unique<FakeRiskManager>();
    EXPECT_FALSE(rm->approve(makeOrder()));
}

TEST(IRiskManagerTest, IsAbstractAndHasVirtualDestructor) {
    static_assert(std::is_abstract_v<IRiskManager>);
    static_assert(std::has_virtual_destructor_v<IRiskManager>);
    static_assert(std::is_base_of_v<IRiskManager, SimpleRiskManager>);
    static_assert(std::is_base_of_v<IRiskManager, FakeRiskManager>);
}

// ---------------------------------------------------------------------------
// FakeRiskManager
// ---------------------------------------------------------------------------

TEST(FakeRiskManagerTest, RejectsValidOrder) {
    FakeRiskManager fake;
    EXPECT_FALSE(fake.approve(makeOrder(1, "AAPL", Side::BUY, 1, 1)));
}

TEST(FakeRiskManagerTest, RejectsEverythingRegardlessOfInput) {
    FakeRiskManager fake;
    EXPECT_FALSE(fake.approve(makeOrder(1, "AAPL", Side::SELL, 0, 0)));
    EXPECT_FALSE(fake.approve(makeOrder(0, "", Side::BUY, 0, 0)));
    EXPECT_FALSE(fake.approve(makeOrder(std::numeric_limits<OrderId>::max(), "MSFT", Side::BUY,
                                        std::numeric_limits<Price>::max(),
                                        std::numeric_limits<Quantity>::max())));
}

// ---------------------------------------------------------------------------
// TradingSystem
// ---------------------------------------------------------------------------

TEST(TradingSystemTest, DelegatesToSimpleRiskManager) {
    SimpleRiskManager simple(kMaxQty, kMaxPrice);
    TradingSystem system(simple);

    EXPECT_FALSE(system.submit(Order{1, "AAPL", Side::BUY, 39, 101}));  // qty over
    EXPECT_FALSE(system.submit(Order{2, "AAPL", Side::BUY, 41, 99}));   // price over
    EXPECT_TRUE(system.submit(Order{3, "AAPL", Side::BUY, 39, 99}));
}

TEST(TradingSystemTest, DelegatesBoundaryOrdersToSimpleRiskManager) {
    SimpleRiskManager simple(kMaxQty, kMaxPrice);
    TradingSystem system(simple);

    EXPECT_TRUE(system.submit(makeOrder(1, "AAPL", Side::BUY, kMaxPrice, kMaxQty)));
    EXPECT_FALSE(system.submit(makeOrder(2, "AAPL", Side::BUY, kMaxPrice + 1, kMaxQty)));
    EXPECT_FALSE(system.submit(makeOrder(3, "", Side::BUY, 1, 1)));
    EXPECT_FALSE(system.submit(makeOrder(0, "AAPL", Side::BUY, 1, 1)));
}

TEST(TradingSystemTest, FakeRiskManagerCanBeSubstituted) {
    FakeRiskManager fake;
    TradingSystem system(fake);

    EXPECT_FALSE(system.submit(Order{1, "AAPL", Side::BUY, 101, 39}));
    EXPECT_FALSE(system.submit(Order{2, "AAPL", Side::BUY, 99, 41}));
    EXPECT_FALSE(system.submit(Order{3, "AAPL", Side::BUY, 1, 1}));
}

TEST(TradingSystemTest, SameOrderGetsDifferentResultsUnderDifferentPolicies) {
    Order order = makeOrder(1, "AAPL", Side::BUY, 10, 10);

    SimpleRiskManager simple(kMaxQty, kMaxPrice);
    FakeRiskManager fake;
    TradingSystem simpleSystem(simple);
    TradingSystem fakeSystem(fake);

    EXPECT_TRUE(simpleSystem.submit(order));
    EXPECT_FALSE(fakeSystem.submit(order));
}

TEST(TradingSystemTest, OwnsCopyOfRiskManager) {
    // TradingSystem copies the policy into its own unique_ptr, so it must keep
    // working after the original risk manager goes out of scope.
    std::unique_ptr<TradingSystem> system;
    {
        SimpleRiskManager simple(kMaxQty, kMaxPrice);
        system = std::make_unique<TradingSystem>(simple);
    }
    EXPECT_TRUE(system->submit(makeOrder(1, "AAPL", Side::BUY, 10, 10)));
    EXPECT_FALSE(system->submit(makeOrder(2, "AAPL", Side::BUY, 10, kMaxQty + 1)));
}

TEST(TradingSystemTest, IsNotCopyable) {
    // unique_ptr member makes TradingSystem non-copyable. (The user-declared
    // destructor also suppresses the implicit move constructor.)
    static_assert(!std::is_copy_constructible_v<TradingSystem>);
    static_assert(!std::is_copy_assignable_v<TradingSystem>);
}

TEST(TradingSystemTest, TakesOwnershipOfRawSimpleRiskManagerPointer) {
    TradingSystem system(new SimpleRiskManager(kMaxQty, kMaxPrice));
    EXPECT_TRUE(system.submit(makeOrder(1, "AAPL", Side::BUY, kMaxPrice, kMaxQty)));
    EXPECT_FALSE(system.submit(makeOrder(2, "AAPL", Side::BUY, kMaxPrice + 1, 1)));
    // No delete here: TradingSystem's unique_ptr frees it (ASan/leaks would flag otherwise).
}

TEST(TradingSystemTest, TakesOwnershipOfRawFakeRiskManagerPointer) {
    TradingSystem system(new FakeRiskManager());
    EXPECT_FALSE(system.submit(makeOrder()));
}

TEST(TradingSystemTest, SubmitIsRepeatable) {
    SimpleRiskManager simple(kMaxQty, kMaxPrice);
    TradingSystem system(simple);
    Order order = makeOrder();
    for (int i = 0; i < 100; ++i)
        EXPECT_TRUE(system.submit(order));
}

