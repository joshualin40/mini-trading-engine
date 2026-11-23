Risk Manager
Module: 03-risk-manager

Objective: Add a replaceable policy using an abstract class and smart-pointer ownership.
Relevant concepts: abstract classes, polymorphism, unique_ptr, dependency injection.

Implementation phases
Phase 1 — Interface
Define IRiskManager with virtual destructor and approve(const Order&) const.

Phase 2 — Limits
Implement SimpleRiskManager(maxQuantity,maxNotional) and reject malformed or oversized orders.

Phase 3 — Ownership
TradingSystem will own std::unique_ptr<IRiskManager>; allow a test-specific fake risk manager.

Phase 4 — Tests
Test exact limits, invalid values, and that a fake policy can be substituted.