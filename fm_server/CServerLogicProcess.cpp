#include "stdafx.h"
#include "CServerLogicProcess.h"
#include "CFmNetServerImpl.h"
#include "CFmNetServerMgr.h"
#include "CFmResourceMgr.hpp"
#include "CMysqlOperation.h"
#include "CFmMainThreadMgr.h"


void CServerLogicProcess::dealCloseSocket(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer, const NetProtocol::tagCloseSocketDnr &_rqt)
{
	(void)_rqt;
	_spPeer->close();
}

void CServerLogicProcess::dealHeartBeat(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
	TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagHeartBeatRqt &_rqt, int _uiPkgNum)
{
	NetProtocol::tagHeartBeatRpd lc_rpd;
	lc_rpd.set();
	NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);
}

void CServerLogicProcess::dealUserLogin(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
	TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagUserLoginRqt &_rqt, int _uiPkgNum)
{
	NetProtocol::tagUserLoginRpd::EtLoginResultStatus lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_failure;
	if (_rqt.etUserType == eUserSuper)
	{
		TxBlobString lc_super_admin_password = CMysqlOperation::getMysqlVariant(_pMysqlWrap, MYSQL_CONFIG_KEY_super_admin_password);
		assert(lc_super_admin_password.length() > 0);
		if (lc_super_admin_password.length() > 0 && _rqt.password.length() > 0
			&& _rqt.username.length() == 0 && lc_super_admin_password == _rqt.password)
		{
			CFmMainThreadMgr::object()->print(L"【一个】超级管理员登录成功。");
			lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_succeed;
		}
		else
		{
			lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_wrong_password;
		}
	}
	else if (_rqt.etUserType == eUserAdmin || _rqt.etUserType == eUserNormal)
	{
		tagMysqlUser lcMysqlUser;
		if (_rqt.username.length() <= 0 && !CPublicHeader::isStandardString(_rqt.username))
		{//用户名错误
			lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_failure;
		}
		else if (!CMysqlOperation::selectMysqlUser(_pMysqlWrap, &lcMysqlUser, _rqt.username))
		{//查找不到用户
			lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_no_user;
		}
		else
		{
			assert(lcMysqlUser.json_data.password.length() > 0);
			if (_rqt.password != lcMysqlUser.json_data.password)
				lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_wrong_password;
			else
			{
				CFmMainThreadMgr::object()->print(L"【一个】用户登录成功：username:{0}, etUserType:{1}").args(_rqt.username, _rqt.etUserType);
				lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_succeed;
			}
		}
	}
	else
	{
		assert(false);
		lc_result_status = NetProtocol::tagUserLoginRpd::ELRS_failure;
	}
	NetProtocol::tagUserLoginRpd lc_rpd;
	lc_rpd.set(lc_result_status);
	NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);
}

void CServerLogicProcess::dealMonitorGeographySearchRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
	TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorGeographySearchRqt &_rqt, int _uiPkgNum)
{
	std::list<NetProtocol::tagMonitorGeographySearchRpd::tagUnitGeography> lcListGeography;
	std::list<tagMysqlGeography> lcListMysqlGeography;
	CMysqlOperation::selectAllMysqlGeographyByParentIdent(_pMysqlWrap, &lcListMysqlGeography, _rqt.llParentGeographyIdent);
	for (auto iter = lcListMysqlGeography.begin(); iter != lcListMysqlGeography.end(); ++iter)
	{
		std::list<NetProtocol::tagMonitorGeographySearchRpd::tagFmSiteInfo> lc_listFmSite;
		for (auto it = iter->json_data.fm_site.cbegin(); it != iter->json_data.fm_site.cend(); ++it)
		{
			lc_listFmSite.push_back(NetProtocol::tagMonitorGeographySearchRpd::tagFmSiteInfo());
			lc_listFmSite.back().set(it->idx, it->name);
		}
		NetProtocol::tagMonitorGeographySearchRpd::tagUnitGeography lc_elem;
		lc_elem.set(iter->self_ident, iter->name, lc_listFmSite);
		lcListGeography.push_back(lc_elem);
	}
	NetProtocol::tagMonitorGeographySearchRpd lc_rpd;
	lc_rpd.set(lcListGeography);
	NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);
}

void CServerLogicProcess::dealMonitorAddNewGeographyRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
	TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorAddNewGeographyRqt &_rqt, int _uiPkgNum)
{
	NetProtocol::tagMonitorAddNewGeographyRpd::EtResultStatus lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_failure;
	tagMysqlGeography lcMysqlGeography;
	lcMysqlGeography.name = _rqt.site_name;
	lcMysqlGeography.parent_ident = _rqt.parent_ident;
	lcMysqlGeography.self_ident = _rqt.self_ident;
	for (auto iter = _rqt.fm_site.cbegin(); iter != _rqt.fm_site.cend(); ++iter)
	{
		lcMysqlGeography.json_data.fm_site.push_back(tagMysqlGeography::tagFmSiteInfo());
		lcMysqlGeography.json_data.fm_site.back().set(iter->idx, iter->name);
	}

	if (lcMysqlGeography.self_ident != CPublicHeader::llEmptyPrimaryKey ?
		CMysqlOperation::updateMysqlGeography(_pMysqlWrap, &lcMysqlGeography)
		: CMysqlOperation::insertMysqlGeography(_pMysqlWrap, &lcMysqlGeography, _rqt.siteRange))
	{
		lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_seccess;
	}
	else lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_failure;
	NetProtocol::tagMonitorAddNewGeographyRpd lc_rpd;
	lc_rpd.set(lcResultStatus);
	NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);

	//NetProtocol::tagMonitorAddNewGeographyRpd::EtResultStatus lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_failure;
	//if (CMysqlOperation::addNewMysqlMonitorGeography(_pMysqlWrap, _rqt.site_name, _rqt.siteRange, _rqt.parent_ident))
	//	lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_seccess;
	//else lcResultStatus = NetProtocol::tagMonitorAddNewGeographyRpd::ERS_failure;
	//NetProtocol::tagMonitorAddNewGeographyRpd lc_rpd;
	//lc_rpd.set(lcResultStatus);
	//NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);
}

void CServerLogicProcess::dealMonitorGeographyRqt(std::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spPeer,
	TxLibrary::CMysqlWrap *_pMysqlWrap, const NetProtocol::tagMonitorGeographyRqt &_rqt, int _uiPkgNum)
{
	NetProtocol::tagMonitorGeographyRpd lc_rpd;

	tagMysqlGeography lcMysqlGeography;
	if (!CMysqlOperation::selectMysqlGeography(_pMysqlWrap, &lcMysqlGeography, _rqt.llGeographyIdent))
		lc_rpd.set_failure();
	else
	{
		std::list<NetProtocol::tagMonitorGeographyRpd::tagFmSiteInfo> lc_fm_site;
		for (auto iter = lcMysqlGeography.json_data.fm_site.cbegin(); iter != lcMysqlGeography.json_data.fm_site.cend(); ++iter)
		{
			lc_fm_site.push_back(NetProtocol::tagMonitorGeographyRpd::tagFmSiteInfo());
			lc_fm_site.back().set(iter->idx, iter->name);
		}
		lc_rpd.set(lcMysqlGeography.self_ident, lcMysqlGeography.parent_ident, lcMysqlGeography.name, lc_fm_site);
	}
	NetProtocol::CShellPackData::packSend(_spPeer, &lc_rpd, _uiPkgNum);
}

