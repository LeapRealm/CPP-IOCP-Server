#pragma once

namespace lesson_06
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

	int32 sum = 0;
	SleepLock sleepLock;

	void Add()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SleepLock> guard(sleepLock);
			sum++;
		}
	}

	void Sub()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SleepLock> guard(sleepLock);
			sum--;
		}
	}

	void lesson_06()
	{
		thread t1(Add);
		thread t2(Sub);

		t1.join();
		t2.join();

		cout << sum << endl;
	}
}
