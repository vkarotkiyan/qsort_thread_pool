#pragma once
#include <iostream>
#include <queue>
#include <future>
#include <condition_variable>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include "blockedqueue.h"
using namespace std;

typedef function<void()> task_type; // удобное определение для сокращения кода
typedef void (*FuncType)(int*, long, long); // тип указатель на функцию, которая является эталоном для функций задач

class ThreadPool { // пул потоков
public:
    ThreadPool();
    void start(); // запуск
    void stop(); // остановка
    future<void> push_task(FuncType f, int* array, long arg1, long arg2); // проброс задач
    void threadFunc(int qindex); // функция входа для потока
    void threadFunc2();
private:
    int m_thread_count; // количество потоков
    vector<thread> m_threads; // потоки
    vector<BlockedQueue<task_type>> m_thread_queues; // очереди задач для потоков
    int m_index; // для равномерного распределения задач
};
