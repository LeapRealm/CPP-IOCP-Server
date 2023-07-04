#include "pch.h"
#include "04_2_UserManager.h"

#include "04_3_AccountManager.h"

void UserManager::ProcessSave()
{
	// User Lock -> Account Lock
	lock_guard<mutex> guard(_mutex);
	Account* account = AccountManager::Instance()->GetAccount(100);
}
