#pragma once

#include "stdafx.h"

class CFmNetServerMgr : public TxSingletonTmpl<CFmNetServerMgr>
{
private:
	std::vector<std::shared_ptr<TxLibrary::CMysqlWrap>> vctIocpSvcMysqlWrap;
	std::shared_ptr<TxIocpTcpApi::TxTcpServerSocket> spFmNetServerImpl;
public:
	CFmNetServerMgr();
	~CFmNetServerMgr();
	TxLibrary::CMysqlWrap* getCurrentThreadMysql();
};

