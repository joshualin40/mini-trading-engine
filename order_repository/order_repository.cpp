#include "order_repository.h"


bool OrderRepository::insert(const Order& order) {
    if (!validate_insert(order)) 
        return false; 

    m_storage.push_back(order); 
    m_lookup[order.order_id] = std::prev(m_storage.end()); 
    

    return true; 
}

OptionalOrderIterator OrderRepository::find(OrderId orderid) const {
    if (!m_lookup.contains(orderid)) 
        return std::nullopt; 
    
    // use .at() since [] is non-const
    return m_lookup.at(orderid); 
}

std::list<Order> OrderRepository::all() const {
    return m_storage; 
}

bool OrderRepository::erase(OrderId orderid) {
    if (!m_lookup.contains(orderid))
        return false;
    
    auto erase_itr = m_lookup[orderid]; 
    m_storage.erase(erase_itr);
    m_lookup.erase(orderid); 

    return true; 
}

// private helpers 
bool OrderRepository::validate_insert(const Order& order) const {
    if (order.symbol == "" || order.order_id <= 0 || order.price < 0 || order.quantity < 0 || m_lookup.contains(order.order_id))
        return false;
    
    return true; 
}

