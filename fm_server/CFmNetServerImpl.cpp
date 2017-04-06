#include "stdafx.h"
#include "CFmNetServerImpl.h"
#include "CServerLogicProcess.h"
#include "CFmNetServerMgr.h"


CFmNetServerImpl::tagPeerSocket::tagPeerSocket(std::shared_ptr<CFmNetServerImpl> &_spServer)
	:TxIocpTcpApi::TxTcpPeerSocket(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>(_spServer),
		TxIocpApiWrap::getMtu(), TxIocpApiWrap::getWriteBufferSize())
{
}

CFmNetServerImpl::tagPeerSocket::~tagPeerSocket()
{
}


CFmNetServerImpl::CFmNetServerImpl() :TxIocpTcpApi::TxTcpServerSocket(TxIocpApiWrap::getInstance())
{
	this->bServerRunning = true;
}

CFmNetServerImpl::~CFmNetServerImpl()
{
	this->triggerServerStop();
}

void CFmNetServerImpl::triggerServerStop()
{
	this->bServerRunning = false;
}

std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> CFmNetServerImpl::onNewSocketEvent()
{
	return std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>(new tagPeerSocket(
		std::dynamic_pointer_cast<CFmNetServerImpl>(this->shared_from_this())));
}

bool CFmNetServerImpl::onAcceptPeerSocketEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp, const SOCKADDR_IN *_addr)
{
	std::shared_ptr<tagPeerSocket> lc_peer = std::dynamic_pointer_cast<tagPeerSocket>(_sp);
	if (!lc_peer || !this->bServerRunning)
	{
		assert(false);
		_sp->close();
		return true;
	}
	TxIocpTcpApi::TxNetAddrInfo lcNetAddrInfo(*_addr);
	char lc_ip_buffer[32] = { 0 };
	int lc_net_port = 0;
	lcNetAddrInfo.get(lc_ip_buffer, &lc_net_port);
	lc_peer->ip.assign(lc_ip_buffer);

	this->mPeerSocketCs.mPeerMutex.lock();
	auto find_iter = this->mPeerSocketCs.mapPeerSocket.find(lc_peer.get());
	if (find_iter != this->mPeerSocketCs.mapPeerSocket.end())
	{
		assert(false);
		txLogError("TxIocpSocketBasic重复地址：{0}").args((size_t)lc_peer.get());
		std::shared_ptr<tagPeerSocket> lc_old_sp_socket = find_iter->second.lock();
		if (lc_old_sp_socket)
			lc_old_sp_socket->close();
	}
	this->mPeerSocketCs.mapPeerSocket[lc_peer.get()] = lc_peer;
	this->mPeerSocketCs.mPeerMutex.unlock();

	return true;
}

bool CFmNetServerImpl::onPeerSocketReadExceptionEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp)
{
	std::shared_ptr<tagPeerSocket> lc_peer = std::dynamic_pointer_cast<tagPeerSocket>(_sp);
	if (!lc_peer)
		assert(false);
	else
	{
		this->mPeerSocketCs.mPeerMutex.lock();
		this->mPeerSocketCs.mapPeerSocket.erase(lc_peer.get());
		this->mPeerSocketCs.mPeerMutex.unlock();
		lc_peer->close();
	}
	return true;
}

bool CFmNetServerImpl::onPeerSocketReadBufferEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,
	const unsigned char *_data, int _size)
{
	if (!this->bServerRunning)
	{
		_sp->close();
		return false;
	}
	std::shared_ptr<tagPeerSocket> lc_peer = std::dynamic_pointer_cast<tagPeerSocket>(_sp);
	if (!lc_peer)
	{
		assert(false);
		if (_sp)
			_sp->close();
		return false;
	}

	lc_peer->package.pushData(_data, _size);
	for (;;)
	{
		unsigned int lc_uiPkgNumber = 0;;
		std::shared_ptr<NetProtocol::tagNetProtocolBasic> spPrtclBasic = lc_peer->package.popResult(&lc_uiPkgNumber);
		if (!spPrtclBasic)
			break;

		switch (spPrtclBasic->eType)
		{
		case NetProtocol::eNetProtocolCloseSocketDnr:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagCloseSocketDnr> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagCloseSocketDnr>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealCloseSocket(_sp, *lc_spPkg);
				}
				else assert(0);
			}
			break;
		case NetProtocol::eNetProtocolHeartBeatRqt:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagHeartBeatRqt> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagHeartBeatRqt>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealHeartBeat(_sp, CFmNetServerMgr::object()->getCurrentThreadMysql(), *lc_spPkg, lc_uiPkgNumber);
				}
				else assert(0);
			}
			break;
		case NetProtocol::eNetProtocolUserLoginRqt:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagUserLoginRqt> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagUserLoginRqt>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealUserLogin(_sp, CFmNetServerMgr::object()->getCurrentThreadMysql(), *lc_spPkg, lc_uiPkgNumber);
				}
				else assert(0);
			}
			break;
		case NetProtocol::eNetProtocolMonitorGeographySearchRqt:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagMonitorGeographySearchRqt> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagMonitorGeographySearchRqt>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealMonitorGeographySearchRqt(_sp, CFmNetServerMgr::object()->getCurrentThreadMysql(), *lc_spPkg, lc_uiPkgNumber);
				}
				else assert(0);
			}
			break;
		case NetProtocol::eNetProtocolMonitorAddNewGeographyRqt:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagMonitorAddNewGeographyRqt> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagMonitorAddNewGeographyRqt>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealMonitorAddNewGeographyRqt(_sp, CFmNetServerMgr::object()->getCurrentThreadMysql(), *lc_spPkg, lc_uiPkgNumber);
				}
				else assert(0);
			}
			break;
		case NetProtocol::eNetProtocolMonitorGeographyRqt:
			if (true)
			{
				std::shared_ptr<NetProtocol::tagMonitorGeographyRqt> lc_spPkg
					= std::dynamic_pointer_cast<NetProtocol::tagMonitorGeographyRqt>(spPrtclBasic);
				if (lc_spPkg)
				{//处理包信息
					CServerLogicProcess::dealMonitorGeographyRqt(_sp, CFmNetServerMgr::object()->getCurrentThreadMysql(), *lc_spPkg, lc_uiPkgNumber);
				}
				else assert(0);
			}
			break;
		default:
			assert(false);
			break;
		}
	}
	return true;
}
