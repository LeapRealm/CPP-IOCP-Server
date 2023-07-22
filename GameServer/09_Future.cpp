#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <future>

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 10'0000; i++)
		sum += i;

	return sum;
}

void PromiseWorker(promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(packaged_task<int64(void)>&& task)
{
	task();
}

void lesson_09()
{
	// 동기(synchronous) 실행
	int64 sum1 = Calculate();
	cout << sum1 << endl;

	// std::future
	{
		// 1) deferred -> 지연 실행 (lazy evaluation)
		// 2) async -> 별도의 쓰레드를 만들어서 실행
		// 3) deferred | async -> 둘 중 알아서 골라 실행

		// 언젠가 미래에 결과물을 밷어줄 것이다
		future<int64> future1 = async(launch::async, Calculate);

		future_status status = future1.wait_for(1ms); // 1ms 동안 기다려본다
		if (status == future_status::ready) 
		{
			// 완료되었으면 실행
		}

		int64 sum2 = future1.get(); // 실행이 완료 될때까지 기다려서 결과를 받아온다
		cout << sum2 << endl;

		// 클래스 멤버 함수 사용하기
		class Knight
		{
		public:
			int64 GetHP() { return 100; }
		};

		Knight knight;
		future<int64> future2 = async(launch::async, &Knight::GetHP, knight); // knight.GetHP();
	}

	// std::promise
	{
		// 미래(std::future)에 결과물을 반환해줄꺼라 약속(std::promise)
		promise<string> promise;
		future<string> future = promise.get_future();

		thread t(PromiseWorker, move(promise));

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	{
		packaged_task<int64(void)> task(Calculate);
		future<int64> future = task.get_future();

		thread t(TaskWorker, move(task));

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}

	// 결론)
	// mutex, condition_variable까지 가지 않고 단순한 애들을 처리할 수 있다
	// 특히나, 한 번 발생하는 이벤트에 유용하다
	// 1) async
	// 원하는 함수를 비동기적으로 실행
	// 2) promise
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packaged_task
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌
}
