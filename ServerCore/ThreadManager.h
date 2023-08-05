#pragma once
#include <functional>

/*-----------------------
	 Thread Manager
-----------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void()> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			_lock;
	vector<thread>	_threads;
};
