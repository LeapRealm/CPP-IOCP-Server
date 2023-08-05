#pragma once

namespace lesson_11
{
	// 가시성
	// - store를 RAM에 하지 않고 임시적으로 캐시에만 했다면, 다른 쓰레드가 RAM에서 load한 데이터가 최신 데이터가 아닐 수 있다
	// - RAM에 최신의 데이터가 있는데 load를 RAM에서 안하고 예전에 보관해 두었던 캐시에서 하면 load한 데이터가 최신 데이터가 아닐 수 있다.
	// 코드 재배치
	// - CPU와 컴파일러가 코드의 실행 순서를 바꾸었을때 단일 쓰레드 기준으로 결과가 같고 더 효율적으로 동작한다고 생각한다면 얼마든지 코드를 재배치해서 실행할 수 있다

	int32 x = 0;
	int32 y = 0;
	int32 r1 = 0;
	int32 r2 = 0;

	volatile bool ready = false;

	void Thread_1()
	{
		while (ready == false)
			;

		y = 1;		// store y
		r1 = x;		// load X

		//r1 = x;	// load X
		//y = 1;	// store y
	}

	void Thread_2()
	{
		while (ready == false)
			;

		x = 1;		// store x
		r2 = y;		// load y

		//r2 = y;	// load y
		//x = 1;	// store x
	}

	void lesson_11()
	{
		int count = 0;

		while (true)
		{
			ready = false;
			count++;

			x = y = r1 = r2 = 0;

			thread t1(Thread_1);
			thread t2(Thread_2);

			ready = true;

			t1.join();
			t2.join();

			if (r1 == 0 && r2 == 0)
				break;
		}

		cout << count << endl;
	}
}
