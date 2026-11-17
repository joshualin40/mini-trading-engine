# Market Data Publisher

**Module:** `01-market-data`

**Objective:** Implement a deterministic quote publisher with subscribers and lambda callbacks.

**Relevant concepts:** std::function, lambda captures, std::map, vector snapshot, callback lifetime.

## Implementation phases

### Phase 1 — Domain model
Create. Document bid <= ask and positive prices. Publish a supplied quote; no randomness yet. Quote {symbol, bid, ask, sequence}


### Phase 2 — Subscriptions
Implement subscribe(std::function<void(const Quote&)>) returning an ID and publish(Quote). Support multiple lambda subscribers.