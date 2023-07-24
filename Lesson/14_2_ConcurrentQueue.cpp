#include "14_2_ConcurrentQueue.h"

LockQueue<int32> q;

void QueuePushTest()
{
	while (true)
	{
		int32 value = rand() % 100;
		q.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void QueuePopTest()
{
	while (true)
	{
		int32 data = 0;
		if (q.TryPop(OUT data))
			cout << data << endl;
	}
}

void lesson_14_2()
{
	thread t1(QueuePushTest);
	thread t2(QueuePopTest);
	thread t3(QueuePopTest);

	t1.join();
	t2.join();
	t3.join();
}
