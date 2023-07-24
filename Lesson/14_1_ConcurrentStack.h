#pragma once

#include <CorePch.h>

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
class LockFreeStack
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

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false) { }

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

		while (_pendingList.compare_exchange_weak(last->next, first) == false) { }
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
class LockFreeStack_SP
{
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr) { }

		shared_ptr<T> data;
		shared_ptr<Node> next;
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
};