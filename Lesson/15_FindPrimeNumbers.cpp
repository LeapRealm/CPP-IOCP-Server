#include "pch.h"

namespace Lesson
{
	bool IsPrime(int32 number)
	{
		if (number <= 1)
			return false;

		if (number == 2 || number == 3)
			return true;

		for (int32 i = 2; i < number; i++)
		{
			if ((number % i) == 0)
				return false;
		}
		return true;
	}

	int CountPrime(int32 start, int32 end)
	{
		int32 count = 0;
		for (int32 number = start; number <= end; number++)
		{
			if (IsPrime(number))
				count++;
		}
		return count;
	}

	void lesson_15()
	{
		const int32 MAX_NUMBER = 100'0000;

		vector<thread> threads;

		int32 coreCount = thread::hardware_concurrency();
		int32 numCount = (MAX_NUMBER / coreCount) + 1;

		atomic<int32> primeCount = 0;

		for (int32 i = 0; i < coreCount; i++)
		{
			int32 start = (i * numCount) + 1;
			int32 end = min(MAX_NUMBER, (i + 1) * numCount);

			threads.push_back(thread([start, end, &primeCount]
			{
				primeCount += CountPrime(start, end);
			}));
		}

		for (thread& t : threads)
			t.join();

		cout << primeCount << endl;
	}
}
