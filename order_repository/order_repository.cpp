#include "order_repository.h"


bool OrderRepository::insert(const Order& order) {
    if (!validate_insert(order)) 
        return false; 

    lookup[order.order_id] = storage.size(); 
    storage.push_back(order); 

    return true; 
}

std::optional<size_t> OrderRepository::find(const Order& order) const {
    // use .at() since [] is non-const
    if (!validate_find(order) || 
        !lookup.contains(order.order_id) || 
        storage.at(lookup.at(order.order_id)) != order) 
        return std::nullopt; 
    
    return lookup.at(order.order_id); 
}

std::vector<Order> OrderRepository::all() const {
    return storage; 
}

// private helpers 
bool OrderRepository::validate_insert(const Order& order) const {
    if (order.symbol == "" || order.order_id <= 0 || order.price < 0 || order.quantity < 0 || lookup.contains(order.order_id))
        return false;
    
    return true; 
}

bool OrderRepository::validate_find(const Order& order) const {
    if (order.symbol == "" || order.order_id <= 0 || order.price < 0 || order.quantity < 0)
        return false;
    
    return true; 
}