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
    m_maxQuantity{maxQuantity}
    , m_maxNotional{maxNotional} {
        if (m_maxNotional == std::numeric_limits<Price>::infinity() || m_maxNotional <= 0)
            throw std::invalid_argument("Invalid maxNotional.");
        if (m_maxQuantity == std::numeric_limits<Quantity>::infinity() || m_maxQuantity <= 0) 
            throw std::invalid_argument("Invalid maxQuantity.");
    }

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

        TradingSystem(SimpleRiskManager* riskmanager):
            m_riskManager(riskmanager) {
                std::println("Trading System Constructor");
            }
        
        TradingSystem(FakeRiskManager* riskmanager):
            m_riskManager(riskmanager) {}
        
        ~TradingSystem() {
            std::println("Trading System Destructor"); 
        }
        // TradingSystem(SimpleRiskManager&& riskmanager):
        //     m_riskManager(riskmanager) {}

        bool submit(const Order& order) {
            return m_riskManager->approve(order); 
        }
    private:
        std::unique_ptr<IRiskManager> m_riskManager; 
}; 