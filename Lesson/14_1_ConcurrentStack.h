#pragma once

namespace lesson_14
{
	template<typename T>
	class LockStack
	{
	public:
		LockStack() { }

		LockStack(const LockStack&) = delete;
		LockStack& operator=(const LockStack&) = delete;

		void Push(T value)
		{
			lock_guard<mutex> lock(_mutex);
			_stack.push(move(value));
			_condVar.notify_one();
		}

		bool TryPop(T& value)
		{
			lock_guard<mutex> lock(_mutex);
			if (_stack.empty())
				return false;

			value = move(_stack.top());
			_stack.pop();

			return true;
		}

		void WaitPop(T& value)
		{
			unique_lock<mutex> lock(_mutex);
			_condVar.wait(lock, [this] { return _stack.empty() == false; });
			value = move(_stack.top());
			_stack.pop();
		}

	private:
		stack<T> _stack;
		mutex _mutex;
		condition_variable _condVar;
	};

	template<typename T>
	class LockFreeStack1
	{
		struct Node
		{
			Node(const T& value) : data(value), next(nullptr) { }

			T data;
			Node* next;
		};

	public:
		void Push(const T& value)
		{
			Node* node = new Node(value);
			node->next = _head;

			//if (_head == node->next)
			//{
			//	_head = node;
			//	return true;
			//}
			//else
			//{
			//	node->next = _head;
			//	return false;
			//}

			while (_head.compare_exchange_weak(node->next, node) == false) {}
		}

		bool TryPop(T& value)
		{
			++_popCount;

			Node* oldHead = _head;

			//if (_head == oldHead)
			//{
			//	_head = oldHead->next;
			//	return true;
			//}
			//else
			//{
			//	oldHead = _head;
			//	return false;
			//}

			while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false) {}

			if (oldHead == nullptr)
			{
				--_popCount;
				return false;
			}

			value = oldHead->data;
			TryDelete(oldHead);
			return true;
		}

		void TryDelete(Node* oldHead)
		{
			// 나말고 누가 있는가?
			if (_popCount == 1)
			{
				// 만약 나 혼자라면 이왕 혼자인거 삭제 예약된 다른 데이터들도 삭제하려고 시도해보자
				Node* node = _pendingList.exchange(nullptr);

				if (--_popCount == 0)
				{
					// 끼어든 애가 없음 -> 삭제 진행
					// 이제와서 끼어들어도, 어차피 데이터는 이미 분리해둔 상태
					DeleteNodes(node);
				}
				else if (node)
				{
					// 누가 끼어들었으니 다시 갖다 놓자
					ChainPendingNodeList(node);
				}

				// 내 데이터는 삭제
				delete oldHead;
			}
			else
			{
				// 다른 애가 있으면 지금 삭제하지 않고 삭제 예약만 한다
				ChainPendingNode(oldHead);
				--_popCount;
			}
		}

		void ChainPendingNodeList(Node* first, Node* last)
		{
			last->next = _pendingList;

			while (_pendingList.compare_exchange_weak(last->next, first) == false) {}
		}

		void ChainPendingNodeList(Node* node)
		{
			Node* last = node;
			while (last->next)
				last = last->next;

			ChainPendingNodeList(node, last);
		}

		void ChainPendingNode(Node* node)
		{
			ChainPendingNodeList(node, node);
		}

		static void DeleteNodes(Node* node)
		{
			while (node)
			{
				Node* next = node->next;
				delete node;
				node = next;
			}
		}

	private:
		atomic<Node*> _head = nullptr;

		atomic<uint32> _popCount = 0;			// Pop을 실행중인 쓰레드 개수
		atomic<Node*> _pendingList = nullptr;	// 삭제 되어야할 노드들 중에서 첫번째 노드
	};

	template<typename T>
	class LockFreeStack2
	{
		struct Node;

		struct CountedNodePtr
		{
			int32 externalCount = 0;
			Node* ptr = nullptr;
		};

		struct Node
		{
			Node(const T& value) : data(make_shared<T>(value)) { }

			shared_ptr<T> data;
			atomic<int32> internalCount = 0;
			CountedNodePtr next;
		};

	public:
		void Push(const T& value)
		{
			CountedNodePtr node;
			node.ptr = new Node(value);
			node.externalCount = 1;

			node.ptr->next = _head;
			while (_head.compare_exchange_weak(node.ptr->next, node) == false) {}
		}

		shared_ptr<T> TryPop()
		{
			CountedNodePtr oldHead = _head;
			while (true)
			{
				// 참조권 획득 (externalCount를 현 시점 기준 +1 한 애가 이김)
				IncreaseHeadCount(oldHead);

				// 최소한 externalCount >= 2 일테니 안전하게 접근할 수 있음
				Node* ptr = oldHead.ptr;

				// 데이터 없음
				if (ptr == nullptr)
					return shared_ptr<T>();

				// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)
				if (_head.compare_exchange_strong(oldHead, ptr->next))
				{
					shared_ptr<T> res;
					res.swap(ptr->data);

					// 나 말고 누군가가 더 있는가?
					const int32 countIncrease = oldHead.externalCount - 2;
					if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
						delete ptr;

					return res;
				}
				// 참조권은 얻었으나, 소유권은 실패
				else if (ptr->internalCount.fetch_sub(1) == 1)
				{
					// 내가 마지막이면 내가 삭제한다
					delete ptr;
				}
			}
		}

	private:
		void IncreaseHeadCount(CountedNodePtr& oldCounter)
		{
			while (true)
			{
				CountedNodePtr newCounter = oldCounter;
				newCounter.externalCount++;

				if (_head.compare_exchange_strong(oldCounter, newCounter))
				{
					oldCounter.externalCount = newCounter.externalCount;
					break;
				}
			}
		}

	private:
		atomic<CountedNodePtr> _head;
	};

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
}
