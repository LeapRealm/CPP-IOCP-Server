#include "pch.h"

namespace Lesson
{
	mutex m3;
	queue<int32> q;
	HANDLE handle;

	void Producer()
	{
		while (true)
		{
			{
				unique_lock<mutex> lock(m3);
				q.push(100);
			}

			::SetEvent(handle);

			this_thread::sleep_for(1000ms);
		}
	}

	void Consumer()
	{
		while (true)
		{
			::WaitForSingleObject(handle, INFINITE);

			// ManualReset을 true로 설정했을 경우, 수동으로 Non-Signal로 세팅해줘야함
			// ::ResetEvent(handle);

			unique_lock<mutex> lock(m3);
			while (q.empty() == false)
			{
				int32 data = q.front();
				q.pop();

				cout << data << endl;
			}
		}
	}

	void lesson_07()
	{
		// 커널 오브젝트
		// Usage Count (몇명이 사용하고 있는가?)
		// Signal(파란불-true) / Non-Signal(빨간불-false) << bool
		// Auto(자동으로 Non-Signal로 세팅됨-false) / Manual(수동으로 Non-Signal로 세팅해줘야함-true) << bool

		handle = ::CreateEvent(NULL/*보안속성*/, FALSE/*ManualReset*/, FALSE/*InitialState*/, NULL/*이름*/);

		thread t1(Producer);
		thread t2(Consumer);

		t1.join();
		t2.join();

		::CloseHandle(handle);
	}
}
