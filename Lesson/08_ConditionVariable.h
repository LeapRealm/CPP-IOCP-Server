#pragma once

namespace lesson_08
{
	mutex m;
	queue<int32> q;

	// cv는 User-Level Object (커널 오브젝트가 아님)
	condition_variable cv;

	void Producer()
	{
		while (true)
		{
			// 1) Lock을 잡고
			// 2) 공유 변수 값을 수정
			// 3) Lock을 풀고
			// 4) 조건변수를 통해 다른 쓰레드에게 통지

			{
				unique_lock<mutex> lock(m);
				q.push(100);
			}

			cv.notify_one(); // wait중인 쓰레드가 있으면 딱 1개를 깨운다
		}
	}

	void Consumer()
	{
		while (true)
		{
			// 1) Lock을 잡고
			// 2) 조건 확인
			// - 만족O => 빠져 나와서 이어서 코드를 진행
			// - 만족X => Lock을 풀어주고 대기 상태가 됨

			// 그런데 notify_one을 했으면 항상 조건식을 만족하는거 아닐까?
			// Spurious Wakeup (가짜 기상)
			// 다른 쓰레드가 새치기해서 먼저 처리해버릴 수 있음

			unique_lock<mutex> lock(m);
			cv.wait(lock, []() { return q.empty() == false; });

			int32 data = q.front();
			q.pop();
			cout << q.size() << " : " << data << endl;
		}
	}

	void lesson_08()
	{
		thread t1(Producer);
		thread t2(Consumer);

		t1.join();
		t2.join();
	}
}
