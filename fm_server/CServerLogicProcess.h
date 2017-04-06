
#include "stdafx.h"

class CServerLogicProcess
{
public:
	static void dealCloseSocket(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		const NetProtocol::tagCloseSocketDnr &_rqt);
	static void dealHeartBeat(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagHeartBeatRqt &_rqt, int _uiPkgNum);
	static void dealUserLogin(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagUserLoginRqt &_rqt, int _uiPkgNum);
	static void dealMonitorGeographySearchRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorGeographySearchRqt &_rqt, int _uiPkgNum);
	static void dealMonitorAddNewGeographyRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorAddNewGeographyRqt &_rqt, int _uiPkgNum);
	static void dealMonitorGeographyRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
		TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorGeographyRqt &_rqt, int _uiPkgNum);
};
