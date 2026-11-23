#include "../order_repository/order_repository.h"
#include <memory> 

class IRiskManager
{
public:
    virtual bool approve(const Order &order) = 0;
    virtual ~IRiskManager() {}
};

class SimpleRiskManager : public IRiskManager
{
public:
    SimpleRiskManager(Quantity maxQuantity, Price maxNotional) : 
    m_maxQuantity(maxQuantity)
    , m_maxNotional(maxNotional) {}

    // rejects malformed or oversized orders
    bool approve(const Order& order) override {
        if (order.quantity > m_maxQuantity || order.price > m_maxNotional || !validate_order(order))
            return false; 

        return true; 
    }

    ~SimpleRiskManager() {}
private:
    bool validate_order(const Order &order) {
        if (order.symbol == "" || order.order_id <= 0 || order.price < 0 || order.quantity < 0)
            return false;
    
        return true; 
    }
    Quantity m_maxQuantity;
    Price m_maxNotional;
};

class FakeRiskManager: public IRiskManager 
{
    public: 
        bool approve(const Order& order) override {
            return false; 
        }

        ~FakeRiskManager() {}
}; 


class TradingSystem {
    public: 
        TradingSystem(const SimpleRiskManager& riskmanager): 
            m_riskManager(std::make_unique<SimpleRiskManager>(riskmanager)) {}

        TradingSystem(const FakeRiskManager& riskmanager): 
            m_riskManager(std::make_unique<FakeRiskManager>(riskmanager)) {}

        bool submit(const Order& order) {
            return m_riskManager->approve(order); 
        }
    private:
        std::unique_ptr<IRiskManager> m_riskManager; 
}; 