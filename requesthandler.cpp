#include "requesthandler.h"

RequestHandler::RequestHandler(){
    m_tpool.start();
}

RequestHandler::~RequestHandler(){
    m_tpool.stop();
}

std::future<void> RequestHandler::pushRequest(FuncType f, int* array, long arg1, long arg2){
    return m_tpool.push_task(f, array, arg1, arg2);
}

void RequestHandler::pushRequest2(){
    m_tpool.threadFunc2();
}
