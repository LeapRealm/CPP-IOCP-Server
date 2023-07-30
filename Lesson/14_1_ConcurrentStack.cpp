#include "14_1_ConcurrentStack.h"

LockFreeStack2<int32> s;

void StackPushTest()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(1ms);
	}
}

void StackPopTest()
{
	while (true)
	{
		auto data = s.TryPop();
		if (data != nullptr)
			cout << (*data) << endl;
	}
}

void lesson_14_1()
{
	thread t1(StackPushTest);
	thread t2(StackPopTest);
	thread t3(StackPopTest);

	t1.join();
	t2.join();
	t3.join();
}
