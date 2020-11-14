// concurrent6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//The thread pool pattern
//A thread pool is a group of pre - instantiated, idle worker threads which stand ready to be given work.
//These are preferred over instantiating new threads for each task when there is a large number of short tasks to be done,
//rather than a small number of long ones.
//This prevents having to incur the overhead of creating a thread a large number of times.

//The file threadpool.cpp implements such a threadpool.
//However, the worker threads in this thread pool suffer from the busy - wait problem.

//Study the program and change it(using std::condition_variable) 
//so that worker threads no longer busy - wait for new tasks to arrive.

// threadpool.cpp
// Compile with:
// g++ -std=c++11 -pthread threadpool.cpp -o threadpool

#include "pch.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <deque>
#include <atomic>

using namespace std;
class ThreadPool; 
// forward declare
//condition_variable ready_cv;
//condition_variable processed_cv;
//atomic<bool> ready(false);
//atomic<bool> processed(false);

class Worker {
public:
	Worker(ThreadPool &s) : pool(s) { }
	void operator()();
private:
	ThreadPool &pool;
};

class ThreadPool {
public:
	ThreadPool(size_t threads);
	template<class F> void enqueue(F f);
	~ThreadPool();
private:
	friend class Worker;
	std::condition_variable cond; //condition here
	std::vector<std::thread> workers;
	std::deque<std::function<void()>> tasks;

	std::mutex queue_mutex;
	bool stop;
};

void Worker::operator()()
{
	std::function<void()> task;
	while (true)
	{
		if (pool.stop) return;
		std::unique_lock<std::mutex> locker(pool.queue_mutex);
		pool.cond.wait(locker, [this]() { return !pool.tasks.empty() || pool.stop; });

			if (pool.stop) return;
			task = pool.tasks.front();
		pool.tasks.pop_front();
		locker.unlock();
		task();
		//if (!pool.tasks.empty())
		//{	
			//processed_cv.notify_one(); //notify		
			//processed = true; //proces			
			//processed_cv.wait(locker); //wait
		//}
		//else {
		//	locker.unlock();
		//}
	}
}

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
	for (size_t i = 0; i < threads; ++i)
		workers.push_back(std::thread(Worker(*this)));
}

//destruct
ThreadPool::~ThreadPool()
{
	
	stop = true; // stop all threads
	cond.notify_all();//hier notify allemaal
	
	for (auto &thread : workers)
		thread.join();
}

//en enque notify?, een conditie var?
template<class F>
void ThreadPool::enqueue(F f)
{
	std::unique_lock<std::mutex> lock(queue_mutex);
	tasks.push_back(std::function<void()>(f));
	//processed = false; //unproces
	//ready = true; //ready
	lock.unlock();
	cond.notify_all(); //notify allemaal
	//processed_cv.wait(lock, [] { return processed.load(); }); //wait load
}

int main()
{
	ThreadPool pool(4);

	// queue a bunch of "work items"
	for (int i = 0; i < 8; ++i)
		pool.enqueue([i]() { std::cout << "Hello from work item " << i << std::endl; });

	// wait for keypress to give worker threads the opportunity to finish tasks
	std::cin.ignore();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
