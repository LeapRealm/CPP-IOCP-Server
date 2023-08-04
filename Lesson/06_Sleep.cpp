#include "pch.h"

namespace Lesson
{
	class SleepLock
	{
	public:
		void lock()
		{
			bool expected = false;
			bool desired = true;

			while (_locked.compare_exchange_strong(expected, desired) == false)
			{
				expected = false;

				this_thread::sleep_for(100ms); // == this_thread::sleep_for(chrono::microseconds(100));
				//this_thread::yield(); // == this_thread::sleep_for(0ms);
			}
		}

		void unlock()
		{
			_locked.store(false);
		}

	private:
		atomic<bool> _locked = false;
	};

	int32 sum2 = 0;
	SleepLock sleepLock;

	void Add3()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SleepLock> guard(sleepLock);
			sum2++;
		}
	}

	void Sub3()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SleepLock> guard(sleepLock);
			sum2--;
		}
	}

	void lesson_06()
	{
		thread t1(Add3);
		thread t2(Sub3);

		t1.join();
		t2.join();

		cout << sum2 << endl;
	}
}
