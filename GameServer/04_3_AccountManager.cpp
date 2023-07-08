#include "pch.h"
#include "04_3_AccountManager.h"

#include "04_2_UserManager.h"

void AccountManager::ProcessLogin()
{
	// Account Lock -> User Lock
	//lock_guard<mutex> guard(_mutex);
	//User* user = UserManager::Instance()->GetUser(100);

	// Lock 순서를 다른 곳과 똑같이 맞춰줘야 한다
	// User Lock -> Account Lock
	User* user = UserManager::Instance()->GetUser(100);
	lock_guard<mutex> guard(_mutex);
}
