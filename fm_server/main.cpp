// fm_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CFmMainThreadMgr.h"
#include "CFmNetServerMgr.h"
#include "CFmResourceMgr.hpp"
#include "CMysqlCheckInitial.h"

int main()
{
	TxDebugObjectMemory::checkPerfect();
	CLibraryHeader::initialize_environment();
	TxLog::initialize();
	txLogInfo("{0}").args(L"fm_server程序启动。。。");
	CFmMainThreadMgr::initialize();
	CFmMainThreadMgr::object()->print(L"fm_server程序启动。。。");
	CFmResourceMgr::initialize();
	TxBlobWString lc_resource_err = CFmResourceMgr::object()->load();
	if (lc_resource_err.length() > 0)
	{
		assert(false);
		CFmMainThreadMgr::object()->print(L"{0}").args(lc_resource_err);
		CFmMainThreadMgr::object()->quit();
	}
	else
	{
		CMysqlCheckInitial::upgrade();
		TxIocpApiWrap::initialize(TxIocpTcpApi::TxIocpService::getOptimalThreadNumber());
		CFmNetServerMgr::initialize();
		CFmMainThreadMgr::object()->loop();//在此消息循环
		CFmNetServerMgr::uninitialize();
		TxIocpApiWrap::uninitialize();
	}
	CFmResourceMgr::uninitialize();
	CFmMainThreadMgr::uninitialize();
	txLogInfo("{0}").args(L"fm_server程序退出。。。");
	TxLog::uninitialize();
	TxDebugObjectMemory::checkPerfect();
	system("pause");
	return 0;
}
