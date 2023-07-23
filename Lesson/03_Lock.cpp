#include "CorePch.h"

vector<int32> v;

// Mutual Exclusive (상호배타적)
mutex m;

// RAII (Resource Acquisition Is Initialization)
template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard()
	{
		_mutex->unlock();
	}

private:
	T* _mutex;
};

void Push()
{
	//lock_guard<mutex> lockGuard_0(m);

	for (int32 i = 0; i < 10000; i++)
	{
		//LockGuard<mutex> lockGuard_1(m);
		//lock_guard<mutex> lockGuard_2(m);

		unique_lock<mutex> uniqueLock(m, defer_lock);
		// ...
		uniqueLock.lock();

		// 자물쇠 잠그기
		//m.lock();
		// m.lock(); -> 재귀적 lock 크래시

		v.push_back(i);

		// 자물쇠 풀기
		//m.unlock();
		// m.unlock(); -> 재귀적 unlock 크래시
	}
}

void lesson_03()
{
	thread t1(Push);
	thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}
