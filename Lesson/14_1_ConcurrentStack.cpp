#include "14_1_ConcurrentStack.h"

LockFreeStack<int32> s;

void StackPushTest()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void StackPopTest()
{
	while (true)
	{
		int32 data = 0;
		if (s.TryPop(OUT data))
			cout << data << endl;
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

int main()
{
	lesson_14_1();
}