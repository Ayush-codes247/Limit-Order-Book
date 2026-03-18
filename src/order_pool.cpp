#include "order_pool.h"

OrderPool::OrderPool(uint32_t cap) {
    capacity  = cap;
    pool      = new Order[cap];
    for (uint32_t i = 0; i < cap; i++) {
        pool[i].next = (i + 1 < cap) ? &pool[i + 1] : nullptr;
    }
    free_list = &pool[0];
}

Order* OrderPool::acquire(){
    if(free_list == nullptr) return nullptr;
    Order* tmp = free_list;
    free_list = tmp->next;
    return tmp;
}

void OrderPool::release(Order* order){
    order->next = free_list;
    free_list = order;
}

OrderPool::~OrderPool() {
    delete[] pool;   // one delete frees the entire block
}