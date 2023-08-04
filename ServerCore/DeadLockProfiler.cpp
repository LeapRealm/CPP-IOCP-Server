#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디를 찾거나 발급한다
	int32 lockID = 0;

	auto findIt = _nameToID.find(name);
	if (findIt == _nameToID.end())
	{
		lockID = static_cast<int32>(_nameToID.size());
		_nameToID[name] = lockID;
		_IDToName[lockID] = name;
	}
	else
	{
		lockID = findIt->second;
	}

	// 잡고 있는 락이 이미 있다면
	if (LLockStack.empty() == false)
	{
		// 처음 발견하는 길이라면 데드락 여부를 확인한다
		const int32 prevID = LLockStack.top();
		if (lockID != prevID)
		{
			set<int32>& history = _lockHistory[prevID];
			if (history.contains(lockID) == false)
			{
				history.insert(lockID);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockID);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockID = _nameToID[name];
	if (LLockStack.top() != lockID)
		CRASH("INVALID_UNLOCK");

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToID.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockID = 0; lockID < lockCount; lockID++)
		DFS(lockID);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::DFS(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회한다
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문한다
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			DFS(there);
			continue;
		}

		// here가 there보다 먼저 발견되었다면, 순방향 간선
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// there가 here보다 먼저 발견되었고, DFS(there)가 아직 종료하지 않았다면, 역방향 간선
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _IDToName[here], _IDToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _IDToName[_parent[now]], _IDToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;
}
