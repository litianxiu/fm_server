#pragma once

#include "stdafx.h"

class CFmNetServerImpl : public TxIocpTcpApi::TxTcpServerSocket
{
private:
	struct tagPeerSocket : public TxIocpTcpApi::TxTcpPeerSocket
	{
		NetProtocol::CShellPackData package;
		TxBlobString pc_guid;
		TxBlobString ip;
	public:
		tagPeerSocket(std::shared_ptr<CFmNetServerImpl> &_spServer);
		~tagPeerSocket();
	};
private:
	TxSystemDependent::TxThreadSafeBoolean bServerRunning;
	struct {
		std::map<tagPeerSocket*, std::weak_ptr<tagPeerSocket>> mapPeerSocket;
		TxSystemDependent::TxMutexWrap mPeerMutex;
	} mPeerSocketCs;
public:
	CFmNetServerImpl();
	~CFmNetServerImpl();
	void triggerServerStop();
	virtual std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> onNewSocketEvent();
	virtual bool onAcceptPeerSocketEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp, const SOCKADDR_IN *_addr);
	virtual bool onPeerSocketReadExceptionEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp);
	virtual bool onPeerSocketReadBufferEvent(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,
		const unsigned char *_data, int _size);
};

