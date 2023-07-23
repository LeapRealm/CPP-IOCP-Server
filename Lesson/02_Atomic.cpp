#include "CorePch.h"

// atomic : All-Or-Nothing

//int32 sum = 0;
atomic<int32> sum = 0;

void Add()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		//sum++;
		sum.fetch_add(1);

		//int32 eax = sum;	// 1. eax = 0
		//eax = eax + 1;	// 3. eax = 1
		//sum = eax;		// 5. sum = 1
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		//sum--;
		sum.fetch_add(-1);

		//int32 eax = sum;	// 2. eax = 0
		//eax = eax - 1;	// 4. eax = -1
		//sum = eax;		// 6. sum = -1
	}
}

void lesson_02()
{
	Add();
	Sub();
	cout << sum << endl;

	thread t1(Add);
	thread t2(Sub);
	t1.join();
	t2.join();
	cout << sum << endl;
}
