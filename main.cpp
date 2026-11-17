#include "market_data/market_data.h"
#include <functional>
#include <print>

int main() {
    MarketData md; 
    std::function<void (const Quote&)> myFunc = [](const Quote&) {
        std::println("Got quote");
    };
    // ask marketdata object to notify us if there was any new quote
    // pass function to marketdata object, so the object will call the function 
    md.subscribe(myFunc); 
}