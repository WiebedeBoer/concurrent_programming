// concurrent3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//The dot product of two vectors
//The dot product of two vectors A = [A1, A2, ..., An] and B = [B1, B2, ..., Bn] is defined as :
//A · B = A1B1 + A2B2 + · · · + AnBn
//The file dotproduct.cpp contains a program that calculates the dot product of two vectors.
//The program uses threading and suffers from a race condition.
//The exercise consists of solving this problem using a mutex lock.

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>  

using Vector = std::vector<int>;
static std::mutex barrier;

struct DotProduct
{
	DotProduct(const Vector &a, const Vector &b) : a(a), b(b)
	{
		if (a.size() != b.size())
			throw "The vectors are of unequal length";
	}

	double operator()()
	{
		size_t nr_threads = 2;
		size_t length = a.size();

		int delta = length / nr_threads;
		int remainder = length % nr_threads;
		int L = 0, R = 0;

		result = 0;

		

		// Create threads
		for (int i = 0; i < nr_threads; ++i) {
			//mutex.lock();
			R = L + delta;
			if (i == nr_threads - 1)
				R += remainder;
			
			workers.push_back(std::thread(&DotProduct::partial_dot_product, this, L, R));			
			L = R;
			//mutex.unlock();
		}
		// Join threads
		for (auto &thread : workers) {
			thread.join();
		}

		return result;
	}

private:
	const Vector &a;
	const Vector &b;

	std::vector<std::thread> workers;
	int result;

	std::mutex mutex;

	void partial_dot_product(int L, int R)
	{
		
		for (int i = L; i < R; ++i) {
			mutex.lock();
			result += a[i] * b[i];
			mutex.unlock();
		}
		
	}

};

int main()
{
	int nr_elements = 100000;

	// Fill two vectors with some values 
	Vector v1(nr_elements, 1), v2(nr_elements, 2);

	// Create Functor object
	DotProduct dp(v1, v2);

	// Print the result
	std::cout << dp() << std::endl;

	return 0;
}
