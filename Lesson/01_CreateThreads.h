#pragma once

namespace lesson_01
{
	void HelloThread()
	{
		cout << "Hello Thread" << endl;
	}

	void HelloThread_Params(int32 num)
	{
		cout << num << endl;
	}

	void lesson_01()
	{
		// thread t(HelloThread);
		thread t;
		thread::id id1 = t.get_id(); // 쓰레드마다 부여되는 고유한 id
		// t = thread(HelloThread);
		t = thread(HelloThread_Params, 10);

		uint32 count = t.hardware_concurrency(); // CPU 논리 프로세서 개수
		thread::id id2 = t.get_id(); // 쓰레드마다 부여되는 고유한 id

		t.detach(); // thread 객체에서 실제 쓰레드를 분리

		if (t.joinable()) // 객체가 관리하고 있는 쓰레드가 현재 동작하고 있는지 알려줌
			t.join(); // 해당 쓰레드가 끝날때까지 대기

		cout << "Hello Main" << endl;

		vector<thread> v;

		for (int32 i = 0; i < 10; i++)
		{
			v.push_back(thread(HelloThread_Params, i));
		}

		for (int32 i = 0; i < 10; i++)
		{
			if (v[i].joinable())
				v[i].join();
		}
	}
}
