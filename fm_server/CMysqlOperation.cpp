#include "stdafx.h"
#include "../../public/common/TxLog.hpp"
#include "CMysqlOperation.h"
#include "CFmResourceMgr.hpp"

bool CMysqlOperation::openMysqlDb(TxLibrary::CMysqlWrap *_pMysqlWrap)
{
	////"初始数据库信息": {
	////		"host" : "localhost",
	////		"port" : "3306",
	////		"database" : "",
	////		"username" : "root",
	////		"password" : "123456"
	////}
	////"实际数据库信息": {
	////		"host" : "localhost",
	////		"port" : "3306",
	////		"database" : "fm_database",
	////		"username" : "root",
	////		"password" : "fm_2017"
	////}
	//auto lc_open_db = [_pMysqlWrap]()->bool{
	//	return _pMysqlWrap->open("localhost", "root", "fm_2017", "fm_database", 3306);
	//};
	//auto lc_init_db = [_pMysqlWrap]()->bool{
	//	bool ret = false;
	//	if (_pMysqlWrap->open("localhost", "root", "123456", "", 3306))
	//	{
	//		_pMysqlWrap->executeSql("CREATE DATABASE IF NOT EXISTS fm_database;");
	//		_pMysqlWrap->executeSql("SET PASSWORD FOR 'root'@'localhost' = PASSWORD('fm_2017');");
	//		ret = true;
	//	}
	//	else if (_pMysqlWrap->open("localhost", "root", "fm_2017", "", 3306))
	//	{
	//		_pMysqlWrap->executeSql("CREATE DATABASE IF NOT EXISTS fm_database;");
	//		ret = true;
	//	}
	//	_pMysqlWrap->close();
	//	return ret;
	//};
	//if (lc_open_db())
	//	return true;
	//if (!lc_init_db())
	//	return false;
	//bool ret=lc_open_db();
	//if (ret)
	//{
	//	_pMysqlWrap->setExcptCallBack([](unsigned int _nErr, const char *_strErr){
	//		txLogError("MYSQL数据库操作错误：[{0}]->[{1}]").args(_nErr, _strErr);
	//	});
	//}
	//return ret;

	bool ret = _pMysqlWrap->open(CFmResourceMgr::object()->mysql.host, CFmResourceMgr::object()->mysql.username,
		CFmResourceMgr::object()->mysql.password, CFmResourceMgr::object()->mysql.database, CFmResourceMgr::object()->mysql.port);
	if (ret)
	{
		_pMysqlWrap->setExcptCallBack([](unsigned int _nErr, const char *_strErr) {
			txLogError("MYSQL数据库操作错误：[{0}]->[{1}]").args(_nErr, _strErr);
		});
	}
	return ret;
}

bool CMysqlOperation::executeMysql(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_sql)
{
	return _pMysqlWrap->executeSql(_sql);
}

long long CMysqlOperation::getLastInsertId(TxLibrary::CMysqlWrap *_pMysqlWrap)
{
	long long ret = CPublicHeader::llEmptyPrimaryKey;
	_pMysqlWrap->executeSql("select LAST_INSERT_ID();", [&ret](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		ret = _dataSet->getValue("LAST_INSERT_ID()").toDatumRet(CPublicHeader::llEmptyPrimaryKey);
		return true;
	});
	return ret;
}

bool CMysqlOperation::setMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobString &_value)
{
	_pMysqlWrap->beginStmt("REPLACE INTO variant(name,data) VALUES(?,?);");
	_pMysqlWrap->bindParamString(_key);
	_pMysqlWrap->bindParamString(_value);
	bool ret = (_pMysqlWrap->endStmt() >= 0);
	DefAssertWrap(ret);
	return ret;
}

bool CMysqlOperation::getMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, TxBlobString *_out, const TxBlobString &_key, bool *_bDbSuccess)
{
	_out->clear();
	bool ret = false;
	bool bbbbb_temp = false;
	if (_bDbSuccess == NULL)
		_bDbSuccess = &bbbbb_temp;
	*_bDbSuccess = _pMysqlWrap->executeSql(CPublicHeader::formatString("SELECT * FROM variant where name='%s';", _key.c_str()),
		[_out, &ret](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		*_out = _dataSet->getValue("data");
		ret = true;
		return true;
	});
	return ret;
}

TxBlobString CMysqlOperation::getMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	TxBlobString ret;
	getMysqlVariant(_pMysqlWrap, &ret, _key, NULL);
	return ret;
}

bool CMysqlOperation::setMysqlVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobWString &_value)
{
	return setMysqlVariant(_pMysqlWrap, _key, _value.codecvt());
}

TxBlobWString CMysqlOperation::getMysqlVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	return CBoostLocaleConv::utf8ToUnicode(getMysqlVariant(_pMysqlWrap, _key));
}

bool CMysqlOperation::deleteMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	TxBlobString lc_sql;
	lc_sql.compose("DELETE FROM variant WHERE name=\'{0}\';").args(_key);
	bool ret = _pMysqlWrap->executeSql(lc_sql.c_str());
	DefAssertWrap(ret);
	return ret;
}

bool CMysqlOperation::setMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobString &_value)
{
	_pMysqlWrap->beginStmt("REPLACE INTO big_variant(name,data) VALUES(?,?);");
	_pMysqlWrap->bindParamString(_key);
	_pMysqlWrap->bindParamString(_value);
	bool ret = (_pMysqlWrap->endStmt() >= 0);
	DefAssertWrap(ret);
	return ret;
}

bool CMysqlOperation::getMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, TxBlobString *_out, const TxBlobString &_key, bool *_bDbSuccess)
{
	_out->clear();
	bool ret = false;
	bool bbbbb_temp = false;
	if (_bDbSuccess == NULL)
		_bDbSuccess = &bbbbb_temp;
	TxBlobString lc_sql;
	lc_sql.compose("SELECT * FROM big_variant where name='{0}';").args(_key);
	*_bDbSuccess = _pMysqlWrap->executeSql(lc_sql.c_str(),
		[_out, &ret](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		*_out = _dataSet->getValue("data");
		ret = true;
		return true;
	});
	return ret;
}

TxBlobString CMysqlOperation::getMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	TxBlobString ret;
	getMysqlBigVariant(_pMysqlWrap, &ret, _key, NULL);
	return ret;
}

bool CMysqlOperation::setMysqlBigVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobWString &_value)
{
	return setMysqlBigVariant(_pMysqlWrap, _key, _value.codecvt());
}

TxBlobWString CMysqlOperation::getMysqlBigVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	return getMysqlBigVariant(_pMysqlWrap, _key).codecvt();
}

bool CMysqlOperation::deleteMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key)
{
	TxBlobString lc_sql;
	lc_sql.compose("DELETE FROM big_variant WHERE name=\'{0}\';").args(_key);
	bool ret = _pMysqlWrap->executeSql(lc_sql.c_str());
	DefAssertWrap(ret);
	return ret;
}


bool CMysqlOperation::_selectMysqlAllUser_(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlUser> *_pListMysqlUser, const TxBlobString &_username)
{
	TxBlobString lc_sql("SELECT * FROM `users` ");
	TxBlobString lc_username = _username.trimLR("\r\n\t;, ");
	if (lc_username.length() > 0)
		lc_sql.appendFormat(" where username='%s' ", lc_username.c_str());
	lc_sql.append(";");
	_pListMysqlUser->clear();
	return _pMysqlWrap->executeSql(lc_sql, [_pListMysqlUser](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		_pListMysqlUser->push_back(tagMysqlUser());
		_pListMysqlUser->back().username = _dataSet->getValue("username");
		_pListMysqlUser->back().role = _dataSet->getValue("role").toDatumRet(eUserNull);
		CBoostMarkupWrap<false> lc_json_pt;
		if (!lc_json_pt.parseJsonString(_dataSet->getValue("json_data")))
			assert(false);
		else
		{
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.password, "password");
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.identity, "identity");
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.name, "name");
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.address, "address");
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.phone, "phone");
			lc_json_pt.getValue(&_pListMysqlUser->back().json_data.note, "note");
		}
		return false;
	});
}

bool CMysqlOperation::selectMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlUser *_pMysqlUser, const TxBlobString &_username)
{
	if (_username.length() <= 0)
	{
		assert(false);
		return false;
	}
	std::list<tagMysqlUser> lc_pListMysqlUser;
	_selectMysqlAllUser_(_pMysqlWrap, &lc_pListMysqlUser, _username);
	if (lc_pListMysqlUser.size() <= 0)
		return false;
	*_pMysqlUser = lc_pListMysqlUser.front();
	return true;
}

void CMysqlOperation::allMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlUser> *_pListMysqlUser)
{
	_selectMysqlAllUser_(_pMysqlWrap, _pListMysqlUser, "");
	_pListMysqlUser->sort([](const tagMysqlUser &_a1, const tagMysqlUser &_a2)->bool {return _a1.username < _a2.username; });
}

bool CMysqlOperation::updateMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, const tagMysqlUser &_mysqlUser)
{
	_pMysqlWrap->beginStmt("REPLACE INTO users(username,role,json_data) VALUES(?,?,?);");
	_pMysqlWrap->bindParamString(_mysqlUser.username.c_str());
	_pMysqlWrap->bindParamInt((int)_mysqlUser.role);

	CBoostMarkupWrap<false> lc_json_pt;
	lc_json_pt.putValue("password", _mysqlUser.json_data.password);
	lc_json_pt.putValue("identity", _mysqlUser.json_data.identity);
	lc_json_pt.putValue("name", _mysqlUser.json_data.name);
	lc_json_pt.putValue("address", _mysqlUser.json_data.address);
	lc_json_pt.putValue("phone", _mysqlUser.json_data.phone);
	lc_json_pt.putValue("note", _mysqlUser.json_data.note);
	_pMysqlWrap->bindParamString(lc_json_pt.saveJsonString());

	bool ret = (_pMysqlWrap->endStmt() >= 0);
	assert(ret);
	return ret;
}

bool CMysqlOperation::deleteMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_username)
{
	bool ret = _pMysqlWrap->executeSql(CPublicHeader::formatString("DELETE FROM users WHERE username=\'%s\';", _username.c_str()));
	assert(ret);
	return ret;
}


bool CMysqlOperation::selectMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_pMysqlGeography, long long _self_ident)
{
	if (_self_ident == CPublicHeader::llEmptyPrimaryKey)
	{
		assert(false);
		return false;
	}
	bool ret = false;
	TxBlobString lc_sql;
	lc_sql.compose("SELECT * FROM geography where self_ident='{0}';").args(_self_ident);
	_pMysqlWrap->executeSql(lc_sql.c_str(),
		[_pMysqlGeography, &ret](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		_dataSet->getValue("self_ident").toDatum(_pMysqlGeography->self_ident);
		_dataSet->getValue("parent_ident").toDatum(_pMysqlGeography->parent_ident);
		_dataSet->getValue("name").toDatum(_pMysqlGeography->name);
		CBoostMarkupWrap<false> lc_json_data_pt;
		if (lc_json_data_pt.parseJsonString(_dataSet->getValue("json_data")))
		{
			CBoostMarkupWrap<false> lc_fm_site_pt = lc_json_data_pt.getPtreeChild("fm_site", false);
			for (auto iter = lc_fm_site_pt.getBoostTree()->begin(); iter != lc_fm_site_pt.getBoostTree()->end(); ++iter)
			{
				if (iter->first.length() > 0)
					DefAssertWrap(false);
				else
				{
					tagMysqlGeography::tagFmSiteInfo lcFmSiteInfo;
					CBoostMarkupWrap<false> lc_elem_pt(iter->second);
					lc_elem_pt.getValueString("idx").toDatum(lcFmSiteInfo.idx);
					lc_elem_pt.getValueString("name").toDatum(lcFmSiteInfo.name);
					_pMysqlGeography->json_data.fm_site.push_back(lcFmSiteInfo);
				}
			}
		}
		ret = true;
		return true;
	});
	return ret;
}

void CMysqlOperation::selectAllMysqlGeographyByParentIdent(TxLibrary::CMysqlWrap *_pMysqlWrap,
	std::list<tagMysqlGeography> *_pListMysqlGeography, long long _parent_ident)
{
	TxBlobString lc_sql;
	_pListMysqlGeography->clear();
	lc_sql.compose("SELECT * FROM geography where parent_ident='{0}';").args(_parent_ident);
	_pMysqlWrap->executeSql(lc_sql.c_str(),
		[_pListMysqlGeography](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		_pListMysqlGeography->push_back(tagMysqlGeography());
		_dataSet->getValue("self_ident").toDatum(_pListMysqlGeography->back().self_ident);
		_dataSet->getValue("parent_ident").toDatum(_pListMysqlGeography->back().parent_ident);
		_dataSet->getValue("name").toDatum(_pListMysqlGeography->back().name);
		CBoostMarkupWrap<false> lc_json_data_pt;
		if (lc_json_data_pt.parseJsonString(_dataSet->getValue("json_data")))
		{
			CBoostMarkupWrap<false> lc_fm_site_pt = lc_json_data_pt.getPtreeChild("fm_site", false);
			for (auto iter = lc_fm_site_pt.getBoostTree()->begin(); iter != lc_fm_site_pt.getBoostTree()->end(); ++iter)
			{
				if (iter->first.length() > 0)
					DefAssertWrap(false);
				else
				{
					tagMysqlGeography::tagFmSiteInfo lcFmSiteInfo;
					CBoostMarkupWrap<false> lc_elem_pt(iter->second);
					lc_elem_pt.getValueString("idx").toDatum(lcFmSiteInfo.idx);
					lc_elem_pt.getValueString("name").toDatum(lcFmSiteInfo.name);
					_pListMysqlGeography->back().json_data.fm_site.push_back(lcFmSiteInfo);
				}
			}
		}
		return false;
	});
}

bool CMysqlOperation::_insertOrUpdateMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap,
	tagMysqlGeography *_mysqlGeography, long long _self_geography_ident, EtDbSiteValueRange _siteRange)
{
	bool ret = false;
	if ((long)_siteRange <= 0 && _siteRange%EDSVR_value_basic != 0)
	{
		assert(false);
		return false;
	}

	CBoostMarkupWrap<false> lc_json_data_pt;
	for (auto iter = _mysqlGeography->json_data.fm_site.cbegin(); iter != _mysqlGeography->json_data.fm_site.cend(); ++iter)
	{
		CBoostMarkupWrap<false> lc_fm_site_pt;
		lc_fm_site_pt.putValue("idx", iter->idx);
		lc_fm_site_pt.putValue("name", iter->name);
		lc_json_data_pt.putArrayTree("fm_site", lc_fm_site_pt);
	}
	TxBlobString lc_json_data_text = lc_json_data_pt.saveJsonString(false);

	_pMysqlWrap->writeLock("geography", [&_pMysqlWrap, &ret, &_siteRange, &_mysqlGeography, &lc_json_data_text, &_self_geography_ident]() {
		if (_self_geography_ident == CPublicHeader::llEmptyPrimaryKey)
		{//获取最大的ident
			TxBlobString lc_sql;
			lc_sql.compose("SELECT max(self_ident) FROM geography where self_ident>='{0}' and self_ident<'{1}';").args(_siteRange, _siteRange + EDSVR_value_basic);
			_pMysqlWrap->executeSql(lc_sql.c_str(),
				[&ret, &_mysqlGeography, &_self_geography_ident](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
				_self_geography_ident = _dataSet->getValue("max(self_ident)").toDatumRet(CPublicHeader::llEmptyPrimaryKey);
				return true;
			});
			if (_self_geography_ident < (long long)_siteRange)
				_self_geography_ident = (long long)_siteRange;
			++_self_geography_ident;
			//插入数据
			_pMysqlWrap->beginStmt("INSERT INTO geography(parent_ident,name,json_data,self_ident) VALUES(?,?,?,?);");
		}
		else
		{
			_pMysqlWrap->beginStmt("UPDATE geography SET parent_ident=?,name=?,json_data=? WHERE self_ident=?;");
		}
		_pMysqlWrap->bindParamInt(_mysqlGeography->parent_ident);
		_pMysqlWrap->bindParamString(_mysqlGeography->name.codecvt());
		_pMysqlWrap->bindParamString(lc_json_data_text);
		_pMysqlWrap->bindParamInt(_self_geography_ident);
		ret = (_pMysqlWrap->endStmt() >= 0);
	});
	assert(ret);
	return ret;
}

bool CMysqlOperation::insertMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_mysqlGeography, EtDbSiteValueRange _siteRange)
{
	return _insertOrUpdateMysqlGeography(_pMysqlWrap, _mysqlGeography, CPublicHeader::llEmptyPrimaryKey, _siteRange);
}

bool CMysqlOperation::updateMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_mysqlGeography)
{
	return _insertOrUpdateMysqlGeography(_pMysqlWrap, _mysqlGeography, _mysqlGeography->self_ident, EDSVR_null);
}

//bool CMysqlOperation::selectMysqlMonitorSite(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlMonitorSite *_pMysqlMonitorSite, long long _geography_ident)
//{
//	if (_geography_ident == CPublicHeader::llEmptyPrimaryKey)
//	{
//		assert(false);
//		return false;
//	}
//	_pMysqlMonitorSite->json_data.point_site.clear();
//	bool ret = false;
//	TxBlobString lc_sql;
//	lc_sql.compose("SELECT * FROM geography where monitor_site='{0}';").args(_geography_ident);
//	_pMysqlWrap->executeSql(lc_sql.c_str(),
//		[_pMysqlMonitorSite, &ret](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
//		_dataSet->getValue("geography_ident").toDatum(_pMysqlMonitorSite->geography_ident);
//		_dataSet->getValue("name").toDatum(_pMysqlMonitorSite->name);
//		CBoostMarkupWrap<false> lc_json_data_pt;
//		if(lc_json_data_pt.parseJsonString(_dataSet->getValue("json_data")))
//		{
//			CBoostMarkupWrap<false> lc_point_site_pt =lc_json_data_pt.getPtreeChild("point_site");
//			for (auto iter = lc_point_site_pt.getBoostTree()->begin(); iter != lc_point_site_pt.getBoostTree()->end(); ++iter)
//			{
//				if (iter->first.length() > 0)
//					DefAssertWrap(false);
//				else
//				{
//					tagMysqlMonitorSite::tagPointSiteInfo lcSiteInfoElem;
//					CBoostMarkupWrap<false> lc_elem_pt(iter->second);
//					lc_elem_pt.getValueString("idx").toDatum(lcSiteInfoElem.idx);
//					lc_elem_pt.getValueString("name").toDatum(lcSiteInfoElem.name);
//					_pMysqlMonitorSite->json_data.point_site.push_back(lcSiteInfoElem);
//				}
//			}
//		}
//		ret = true;
//		return true;
//	});
//	return ret;
//}
//
//bool CMysqlOperation::addNewMysqlMonitorSite(TxLibrary::CMysqlWrap *_pMysqlWrap,
//	const TxBlobWString &_site_name, EtDbSiteValueRange _siteRange, long long _parent_ident)
//{
//	bool ret = false;
//	long long lc_max_ident = CPublicHeader::llEmptyPrimaryKey;
//	_pMysqlWrap->writeLock("geography", [&_pMysqlWrap, &ret, &_siteRange, &lc_max_ident, &_parent_ident, &_site_name]() {
//		if (true)
//		{//获取最大的ident
//			TxBlobString lc_sql;
//			lc_sql.compose("SELECT max(self_ident) FROM geography where self_ident>='{0}' and self_ident<'{1}';").args(_siteRange, _siteRange + EDSVR_value_basic);
//			_pMysqlWrap->executeSql(lc_sql.c_str(),
//				[&ret, &lc_max_ident](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
//				lc_max_ident = _dataSet->getValue("max(self_ident)").toDatumRet(CPublicHeader::llEmptyPrimaryKey);
//				return true;
//			});
//			if (lc_max_ident < (long long)_siteRange)
//				lc_max_ident = (long long)_siteRange;
//			++lc_max_ident;
//		}
//		if (true)
//		{//插入数据
//			_pMysqlWrap->beginStmt("INSERT INTO geography(self_ident,parent_ident,name) VALUES(?,?,?);");
//			_pMysqlWrap->bindParamInt(lc_max_ident);
//			_pMysqlWrap->bindParamInt(_parent_ident);
//			_pMysqlWrap->bindParamString(_site_name.codecvt());
//			ret = (_pMysqlWrap->endStmt() == 1);
//		}
//	});
//	assert(ret);
//	return ret;
//}

void CMysqlOperation::selectMysqlMonitorPhoto(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlMonitorPhoto> *_pListMysqlMonitorPhoto,
	long long _geography_ident, std::time_t _begin_time, std::time_t _end_time, int _limit_count)
{
	_pListMysqlMonitorPhoto->clear();
	TxBlobString lc_sql;//date >= '2013-12' and date <= '2014-05';
	lc_sql.compose("SELECT * FROM monitor_photo where geography_ident='{0}' AND upload_time>='{1}' AND upload_time<='{2}' limit {3};").args(
		_geography_ident, _begin_time, _end_time, _limit_count);
	_pMysqlWrap->executeSql(lc_sql.c_str(),
		[_pListMysqlMonitorPhoto](const TxLibrary::CMysqlWrap::CResultSet *_dataSet)->bool {
		_pListMysqlMonitorPhoto->push_back(tagMysqlMonitorPhoto());
		_dataSet->getValue("geography_ident").toDatum(_pListMysqlMonitorPhoto->back().geography_ident);
		_dataSet->getValue("upload_time").toDatum(_pListMysqlMonitorPhoto->back().upload_time);
		_dataSet->getValue("photo_path").toDatum(_pListMysqlMonitorPhoto->back().photo_path);
		return false;
	});
}

