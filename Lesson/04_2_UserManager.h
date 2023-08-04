#pragma once

namespace Lesson
{
	class User
	{
		// Variables...
	};

	class UserManager
	{
	public:
		static UserManager* Instance()
		{
			static UserManager instance;
			return &instance;
		}

		User* GetUser(int32 id)
		{
			lock_guard<mutex> guard(_mutex);

			if (_users.contains(id))
				return _users[id];

			return nullptr;
		}

		void ProcessSave();

	private:
		mutex _mutex;
		map<int32, User*> _users;
	};
}
