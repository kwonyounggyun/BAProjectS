// LockFree.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "LockFreeQueue.h"
#include <thread>
#include <vector>

#include <mutex>
#include <queue>
#include <Windows.h>

template <typename T>
class LockQueue {
public:
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(value);
    }

    bool try_pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }

        value = m_queue.front();
        m_queue.pop();
        return true;
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
};


int main()
{
    LockFreeQueue<int> a;
    std::vector<std::thread*> threads;
    auto s_a = GetTickCount64();
    for (int i = 0; i < 20; i++)
    {
        std::thread* t;
        if (i % 2 == 0)
        {
            t = new std::thread([](LockFreeQueue<int>* queue)->void
                {
                    for(int i = 1; i< 100000; i++)
                    {
                        queue->push_back(i);
                        for (int j = i; j > 0; j--);
                    }
                }, &a);
        }
        else
        {
            t = new std::thread([](LockFreeQueue<int>* queue)->void
                {
                    for(int i = 1;i < 100000;)
                    {
                        auto val = queue->pop_front();

                        if (val != 0)
                        {
                            for (int j = val; j > 0; j--);
                            i++;
                        }
                    }

                    
                }, &a);
        }
        threads.push_back(t);
    }

    for (auto thread : threads)
    {
        thread->join();
        delete thread;
    }

    threads.clear();
    auto e_a = GetTickCount64();

    LockQueue<int> lock;
    auto s_l = GetTickCount64();
    for (int i = 0; i < 20; i++)
    {
        std::thread* t;
        if (i % 2 == 0)
        {
            t = new std::thread([](LockQueue<int>* queue)->void
                {
                    for (int i = 1; i < 100000; i++)
                    {
                        queue->push(i);
                        for (int j = i; j > 0; j--);
                    }
                }, & lock);
        }
        else
        {
            t = new std::thread([](LockQueue<int>* queue)->void
                {
                    for (int i = 1; i < 100000;)
                    {
                        int val;
                        queue->try_pop(val);

                        if (val != 0)
                        {
                            for (int j = val; j > 0; j--);
                            i++;
                        }
                    }


                }, & lock);
        }
        threads.push_back(t);
    }

    for (auto thread : threads)
    {
        thread->join();
        delete thread;
    }

    threads.clear();
    auto e_l = GetTickCount64();

    std::cout << "a : " << e_a - s_a << " b : " << e_l - s_l << std::endl;

    std::cout << "Hello World!\n";
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
