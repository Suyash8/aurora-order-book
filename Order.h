enum class OrderType {
    BUY,
    SELL
};

struct Order {
    int id;
    double price;
    int quantity;
    OrderType type;
};