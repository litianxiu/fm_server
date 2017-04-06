#include "stdafx.h"
#include "CFmNetServerImpl.h"
#include "CFmNetServerMgr.h"
#include "CFmMainThreadMgr.h"
#include "CMysqlOperation.h"

CFmNetServerMgr::CFmNetServerMgr()
{
	//每一个iocp线程绑定一个MySQL对象
	this->vctIocpSvcMysqlWrap.resize(TxIocpApiWrap::getThreadCount());
	for (int ii = 0; ii < (int)this->vctIocpSvcMysqlWrap.size(); ++ii)
	{
		this->vctIocpSvcMysqlWrap[ii].reset(new TxLibrary::CMysqlWrap());
		CMysqlOperation::openMysqlDb(this->vctIocpSvcMysqlWrap[ii].get());
	}

	//初始化server
	int lc_port = CPublicHeader::eFmServerTcpPort;
	this->spFmNetServerImpl.reset(new CFmNetServerImpl());
	if (!this->spFmNetServerImpl->listen(lc_port))
	{
		CFmMainThreadMgr::object()->print(L"服务器端口【{0}】被占用。").args(lc_port);
		CFmMainThreadMgr::object()->quit();
	}
	else
	{
		CFmMainThreadMgr::object()->print(L"启动服务器，端口: {0}。").args(lc_port);
	}
}

CFmNetServerMgr::~CFmNetServerMgr()
{
	//释放server
	std::shared_ptr<CFmNetServerImpl> lc_spFmNetServerImpl = std::dynamic_pointer_cast<CFmNetServerImpl>(this->spFmNetServerImpl);
	if (!lc_spFmNetServerImpl)
		assert(false);
	else
	{
		lc_spFmNetServerImpl->triggerServerStop();
		lc_spFmNetServerImpl->close();
		lc_spFmNetServerImpl.reset();
	}
	this->spFmNetServerImpl->close();
	this->spFmNetServerImpl.reset();
	//释放iocp的MySQL对象
	for (int ii = 0; ii < (int)this->vctIocpSvcMysqlWrap.size(); ++ii)
		this->vctIocpSvcMysqlWrap[ii]->close();
	this->vctIocpSvcMysqlWrap.clear();
}

TxLibrary::CMysqlWrap* CFmNetServerMgr::getCurrentThreadMysql()
{
	int lc_idx = TxIocpApiWrap::getInstance()->getCurrentThreadIndex();
	if (0 <= lc_idx&&lc_idx < (int)this->vctIocpSvcMysqlWrap.size())
		return this->vctIocpSvcMysqlWrap[lc_idx].get();
	assert(false);
	return NULL;
}
