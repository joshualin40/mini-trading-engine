#include "market_data.h"

void MarketData::publish(const Quote& quote) {
    // iterate over functions, call all functions with the quote
    for (auto it = functions.begin(); it != functions.end(); ++it) {
        (*it).second(quote); 
    }
} 

int MarketData::subscribe(CallBack func) { 
    if (func == nullptr) 
        throw std::invalid_argument("Invalid Subscriber");
    // returning an ID and publish(Quote). Support multiple lambda subscribers.
    functions[id_counter] = func; // make a copy
    
    return id_counter++; 
}

