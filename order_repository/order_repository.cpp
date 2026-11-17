#include "order_repository.h"


bool OrderRepository::insert(const Order& order) {
    if (!validate_insert(order)) 
        return false; 

    lookup[order.order_id] = storage.size(); 
    storage.push_back(order); 

    return true; 
}

std::optional<size_t> OrderRepository::find(OrderId orderid) const {
    if (!lookup.contains(orderid)) 
        return std::nullopt; 
    
    // use .at() since [] is non-const
    return lookup.at(orderid); 
}

std::vector<Order> OrderRepository::all() const {
    return storage; 
}

bool OrderRepository::erase(OrderId orderid) {
    if (!lookup.contains(orderid))
        return false;
    
    size_t erase_index = lookup[orderid]; 
    if (erase_index == storage.size() - 1) // last element
        storage.pop_back();
    else {
        OrderId back_id = storage[storage.size() - 1].order_id; 
        lookup[back_id] = erase_index; 
        storage[erase_index] = storage[storage.size() - 1]; 
        storage.pop_back(); 
    } 

    lookup.erase(orderid); 

    return true; 
}

// private helpers 
bool OrderRepository::validate_insert(const Order& order) const {
    if (order.symbol == "" || order.order_id <= 0 || order.price < 0 || order.quantity < 0 || lookup.contains(order.order_id))
        return false;
    
    return true; 
}

