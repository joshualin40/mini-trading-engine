#include <vector> 
#include <string> 
#include <exception> 
#include <unordered_map>
#include <cstdint>
#include <optional> 

using OrderId = uint64_t; 
using Quantity = uint64_t; 
using Price = uint64_t; 

enum class Side {
    BUY,
    SELL
}; 

struct Order {
    std::string symbol; 
    OrderId order_id; 
    Side side; 
    Price price; 
    Quantity quantity;

    bool operator==(const Order&) const = default;
}; 

class OrderRepository {
    public: 
        bool insert(const Order& order);
        std::optional<size_t> find(OrderId orderid) const;
        bool erase(OrderId orderid); 
        std::vector<Order> all() const; 
        
    private: 
        bool validate_insert(const Order& order) const; 

        std::vector<Order> storage; 
        std::unordered_map<OrderId, size_t> lookup; // map OrderIds to index
};