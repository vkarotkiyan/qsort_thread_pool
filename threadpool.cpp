#include "threadpool.h"
using namespace std;

ThreadPool::ThreadPool() :
    m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 4),
    m_thread_queues(m_thread_count) {}

void ThreadPool::start(){
    for (int i = 0; i < m_thread_count; i++)
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
}

void ThreadPool::stop(){
    for (int i = 0; i < m_thread_count; i++) {
        task_type empty_task; // кладем задачу-пустышку в каждую очередь для завершения потока
        m_thread_queues[i].push(empty_task);
    }
    for (auto& t: m_threads)
        t.join();
}

future<void> ThreadPool::push_task(FuncType f, int* array, long arg1, long arg2){
    int queue_to_push = m_index++ % m_thread_count; // вычисляем индекс очереди, куда положим задачу
    auto promise = make_shared<std::promise<void>>();
    auto result = promise->get_future();
    task_type task = ([=]{ f(array, arg1, arg2); promise->set_value(); }); // формируем функтор
    m_thread_queues[queue_to_push].push(task); // кладем в очередь
    return result;
}

void ThreadPool::threadFunc(int qindex){
    while(true) {
        task_type task_to_do; // обработка очередной задачи
        bool res;
        int i = 0;
        for(; i < m_thread_count; i++) {
            // попытка быстро забрать задачу из любой очереди, начиная со своей
            if(res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
                break;
        }
        if (!res) {
            m_thread_queues[qindex].pop(task_to_do); // вызываем блокирующее получение очереди
        } else if (!task_to_do)
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do); // чтобы не допустить зависания потока кладем обратно задачу-пустышку
        if (!task_to_do){
            return;
        }
        task_to_do(); // выполняем задачу
    }
}

void ThreadPool::threadFunc2() {
    task_type task_to_do;
    int i = 0;
    for (; i < m_thread_count; ++i)
        if (m_thread_queues[i % m_thread_count].fast_pop(task_to_do))
            break;
    if (!task_to_do)
        this_thread::yield();
    else
        task_to_do();
}
