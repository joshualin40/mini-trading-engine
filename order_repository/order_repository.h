#include <list>
#include <string>
#include <exception>
#include <unordered_map>
#include <cstdint>
#include <optional>

using OrderId   = uint64_t;
using Quantity  = int64_t;
using Price     = int64_t;

enum class Side
{
    BUY,
    SELL
};

struct Order
{
    OrderId order_id;
    std::string symbol;
    Side side;
    Price price;
    Quantity quantity;

    Order(OrderId order_id_, const std::string symbol_, Side side_, Price price_, Quantity quantity_)
        : order_id(order_id_)
        , symbol(symbol_)
        , side(side_)
        , price(price_)
        , quantity(quantity_) {}

    Order(const Order &other)
        : symbol((other.symbol))
        , order_id((other.order_id))
        , side((other.side))
        , price((other.price))
        , quantity((other.quantity)) {}

    Order &operator=(Order &other)
    {
        symbol = (other.symbol);
        order_id = (other.order_id);
        side = other.side;
        price = (other.price);
        quantity = (other.quantity);

        return *this;
    }
    Order(Order &&other) noexcept
        : order_id(std::move(other.order_id)), symbol(std::move(other.symbol)), side(std::move(other.side)), price(std::move(other.price)), quantity(std::move(other.quantity)) {}

    Order &operator=(Order &&other) noexcept
    {
        order_id = (std::move(other.order_id));
        symbol = (std::move(other.symbol));
        side = (std::move(other.side));
        price = (std::move(other.price));
        quantity = (std::move(other.quantity));

        return (*this);
    }
    bool operator==(const Order &other) const = default;
};

using OrderIterator = std::list<Order>::iterator;
using OptionalOrderIterator = std::optional<OrderIterator>;

class OrderRepository
{
public:
    bool insert(const Order &order);
    OptionalOrderIterator find(OrderId orderid) const;
    bool erase(OrderId orderid);
    std::list<Order> all() const;

private:
    bool validate_insert(const Order &order) const;

    std::list<Order> m_storage;
    std::unordered_map<OrderId, OrderIterator> m_lookup; // map OrderId to to list iterator
};
