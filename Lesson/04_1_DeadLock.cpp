#include "pch.h"

#include "04_2_UserManager.h"
#include "04_3_AccountManager.h"

namespace Lesson
{
	void Func1()
	{
		for (int32 i = 0; i < 1000; i++)
		{
			UserManager::Instance()->ProcessSave();
		}
	}

	void Func2()
	{
		for (int32 i = 0; i < 1000; i++)
		{
			AccountManager::Instance()->ProcessLogin();
		}
	}

	void lesson_04()
	{
		thread t1(Func1);
		thread t2(Func2);

		t1.join();
		t2.join();

		cout << "Jobs Done" << endl;

		// etc.
		mutex m1;
		mutex m2;
		lock(m1, m2); // m1.lock(); m2.lock(); -> 항상 같은 순서가 보장된다

		// adopt_lock : 이미 lock된 상태니까, 나중에 소멸될 때 풀어주기만 해
		lock_guard<mutex> g1(m1, adopt_lock);
		lock_guard<mutex> g2(m2, adopt_lock);
	}
}
