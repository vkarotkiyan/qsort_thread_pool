#pragma once
#include <queue>
#include <mutex>

template<class T> class BlockedQueue {
public:
    void push(T& item){
        std::lock_guard<std::mutex> l(m_locker);
        m_task_queue.push(item); // обычный потокобезопасный push
        m_notifier.notify_one(); // делаем оповещение, чтобы поток, вызвавший pop проснулся и забрал элемент из очереди
    }
    
    void pop(T& item) { // блокирующий метод получения элемента из очереди
        std::unique_lock<std::mutex> l(m_locker);
        if (m_task_queue.empty())
            m_notifier.wait(l, [this]{return !m_task_queue.empty();}); // ждем, пока вызовут push
        item = m_task_queue.front();
        m_task_queue.pop();
    }
    
    bool fast_pop(T& item) { // неблокирующий метод получения элемента из очереди возвращает false, если очередь пуста
        std::lock_guard<std::mutex> l(m_locker);
        if(m_task_queue.empty())
            return false; // просто выходим
        item = m_task_queue.front(); // забираем элемент
        m_task_queue.pop();
        return true;
    }
private:
    std::mutex m_locker;
    std::queue<T> m_task_queue; // очередь задач
    std::condition_variable m_notifier; // уведомитель
};
