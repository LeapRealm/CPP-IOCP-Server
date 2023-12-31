#pragma once

namespace lesson_05
{
	class SpinLock
	{
	public:
		void lock()
		{
			// 아래의 코드를 atomic하게 실행해야함
			// while (_locked)
			// {
			//	
			// }
			// _locked = true;

			// CAS (Compare-And-Swap)
			bool expected = false;
			bool desired = true;

			while (_locked.compare_exchange_strong(expected, desired) == false)
			{
				expected = false;
			}

			// compare_exchange_strong 함수 의사 코드
			// 아래의 코드를 atomic하게 실행함
			// if (_locked == expected)
			// {
			//		expected = _locked;
			//		_locked = desired;
			//		return true;
			// }
			// else
			// {
			//		expected = _locked;
			//		return false;
			// }
		}

		void unlock()
		{
			_locked.store(false);
		}

	private:
		atomic<bool> _locked = false;
	};

	int32 sum = 0;
	mutex m;
	SpinLock spinLock;

	void Add()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SpinLock> guard(spinLock);
			sum++;
		}
	}

	void Sub()
	{
		for (int32 i = 0; i < 10'0000; i++)
		{
			lock_guard<SpinLock> guard(spinLock);
			sum--;
		}
	}

	void lesson_05()
	{
		thread t1(Add);
		thread t2(Sub);

		t1.join();
		t2.join();

		cout << sum << endl;
	}
}
