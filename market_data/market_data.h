#include <string> 
#include <list> 
#include <unordered_map> 
#include <functional>
#include <exception> 
#include <print> 

struct Quote {
    std::string symbol;
    int bid; 
    int ask; 
    int sequence; // ID  
};

class MarketData {
    public: 
        using CallBack = std::function<void (const Quote&)>; 
        using SubscriberId = size_t; 

        void publish(const Quote& quote); 
        int subscribe(CallBack func); 

    private:
        std::list<Quote> quotes;
        std::unordered_map<SubscriberId, CallBack> functions; 
        SubscriberId id_counter = 0; 
};