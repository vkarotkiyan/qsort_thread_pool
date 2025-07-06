#pragma once
#include "threadpool.h"

class RequestHandler {
public:
    RequestHandler();
    ~RequestHandler();
    std::future<void> pushRequest(FuncType f, int* array, long arg1, long arg2); // отправка запроса на выполнение
    void pushRequest2();
private:
    ThreadPool m_tpool; // пул потоков
};
