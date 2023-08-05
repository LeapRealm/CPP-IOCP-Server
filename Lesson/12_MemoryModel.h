#pragma once

namespace lesson_12
{
	atomic<bool> flag;

	atomic<bool> ready;
	int32 value;

	void Producer()
	{
		value = 10;
		ready.store(true, memory_order::memory_order_seq_cst);

		// atomic 클래스를 사용하지 않고 절취선을 그어서 절취선 이전의 코드가 절취선 이후로 넘어가지 않도록 막고 싶다면
		//atomic_thread_fence(memory_order::memory_order_release);
		// -> 가시성 보장 & 코드 재배치 문제 해결
	}

	void Consumer()
	{
		while (ready.load() == false)
			;

		cout << value << endl;
	}

	void lesson_12()
	{
		flag = false;
		cout << flag.is_lock_free() << endl;

		//flag = true;
		flag.store(true, memory_order::memory_order_seq_cst);

		//bool val = flag;
		bool val = flag.load(memory_order::memory_order_seq_cst);

		// 이전 flag 값을 prev에 넣고, flag 값을 수정
		{
			bool prev = flag.exchange(true); // (O)

			//bool prev = flag; // (X)
			//flag = true;
		}

		// CAS(Compare-And-Swap) 조건부 수정
		{
			bool expected = false;
			bool desired = true;
			flag.compare_exchange_strong(expected, desired);

			//if (flag == expected)
			//{
			//	//expected = flag;
			//	flag = desired;
			//	return true;
			//}
			//else
			//{
			//	expected = flag;
			//	return false;
			//}
		}
		{
			bool expected = false;
			bool desired = true;
			flag.compare_exchange_weak(expected, desired);

			// Spurious Failure
			// 다른 쓰레드의 Interruption을 받아서 중간에 실패할 수 있음

			//if (flag == expected)
			//{
			//	if (묘한 상황)
			//		return false;
			//
			//	//expected = flag;
			//	flag = desired;
			//	return true;
			//}
			//else
			//{
			//	expected = flag;
			//	return false;
			//}
		}

		{
			ready = false;
			value = 0;
			thread t1(Producer);
			thread t2(Consumer);
			t1.join();
			t2.join();

			// Memory Model (정책)
			// 1) Sequentially Consistent (seq_cst)
			// 2) Acquire-Release (acquire, release)
			// 3) Relaxed (relaxed)

			// 1) seq_cst (가장 엄격 = 컴파일러 최적화 여지 적음 = 직관적)
			// - 가시성 문제 해결! 코드 재배치 문제 해결!

			// 2) acquire-release
			// 딱 중간!
			// release 명령 이전의 메모리 명령들이 release 명령 이후로 재배치 되는 것을 금지
			// acquire로 같은 변수를 읽는 쓰레드가 있다면, release 이전의 명령들이 acquire 하는 순간에 관찰 가능 (가시성 보장)

			// 3) relaxed (자유롭다 = 컴파일러 최적화 여지 많음 = 직관적이지 않음)
			// 너무나도 자유롭다!
			// 코드 재배치도 멋대로 가능! 가시성 해결 NO!
			// 가장 기본 조건(동일 객체에 대한 동일 관전 순서)만 보장한다

			// 인텔, AMD의 경우 애당초 순차적 일관성을 보장해서 seq_cst를 써도 성능 부하가 없음
			// ARM의 경우 꽤 차이가 있을 수 있다!
		}
	}
}
