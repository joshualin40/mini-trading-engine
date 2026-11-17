# Order Repository

**Module:** `order-repository`

**Objective:** Store and retrieve orders efficiently, while learning how vector reallocation affects indices and pointers.

**Relevant concepts:** vector, unordered_map, pointer invalidation, O(1) average lookup and swap-erase.

## Implementation phases

### Phase 1 — Order model
Define OrderId, Side, Order. Validate nonempty symbol, positive quantity/price, unique nonzero IDs.

### Phase 2 — Primary storage
Store orders in std::vector<Order>. Implement insert(), find(), and all().

### Phase 3 — Indexed lookup
Add std::unordered_map<OrderId,size_t>. Implement erase with swap-with-last and update moved indices.

### Phase 4 — Tests and complexity
Test duplicate IDs, deletion first/middle/last, pointer invalidation. Explain average hash lookup and vector reallocation costs.