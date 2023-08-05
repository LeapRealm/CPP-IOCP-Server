#pragma once

namespace lesson_04
{
	class Account
	{
		// Variables...
	};

	class AccountManager
	{
	public:
		static AccountManager* Instance()
		{
			static AccountManager instance;
			return &instance;
		}

		Account* GetAccount(int32 id)
		{
			lock_guard<mutex> guard(_mutex);

			if (_accounts.contains(id))
				return _accounts[id];

			return nullptr;
		}

		void ProcessLogin();

	private:
		mutex _mutex;
		map<int32, Account*> _accounts;
	};
}
