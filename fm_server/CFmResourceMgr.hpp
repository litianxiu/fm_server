#pragma once

#include "stdafx.h"

struct CFmResourceMgr : public TxSingletonTmpl<CFmResourceMgr>
{
	struct {
		TxBlobString host;
		int port;
		TxBlobString database;
		TxBlobString username;
		TxBlobString password;
	} mysql;
	std::map<int, TxBlobString> sql_script;
public:
	CFmResourceMgr()
	{
		this->init_default_();
	}
	~CFmResourceMgr()
	{
	}
	void init_default_()
	{
		this->mysql.host = "localhost";
		this->mysql.port = 3306;
		this->mysql.database = "fm_database";
		this->mysql.username = "fm_admin";
		this->mysql.password = "fm_2017";
	}
	TxBlobWString load()
	{
		CBoostMarkupWrap<false> lc_doc;
		//加载资源文件
		if (!lc_doc.parseJsonFile(TxBlobWString(TxSystemDependent::TxRuntimeWrap::getWorkingDirectory()) + L"fm_resource.json"))
		{
			return TxBlobWString(L"无法加载资源文件【fm_resource.json】");
		}
		//mysql
		lc_doc.getValueString("mysql.host").toDatum(this->mysql.host);
		lc_doc.getValueString("mysql.port").toDatum(this->mysql.port);
		lc_doc.getValueString("mysql.database").toDatum(this->mysql.database);
		lc_doc.getValueString("mysql.username").toDatum(this->mysql.username);
		lc_doc.getValueString("mysql.password").toDatum(this->mysql.password);
		//sql_script
		CBoostMarkupWrap<false> lc_sql_script_pt = lc_doc.getPtreeChild("sql_script");
		for (auto iter = lc_sql_script_pt.getBoostTree()->begin(); iter != lc_sql_script_pt.getBoostTree()->end(); ++iter)
		{
			CBoostMarkupWrap<false> lc_sql_item_pt;
			lc_sql_item_pt.attach(&iter->second);
			int lc_sql_ver_idx = TxBlobString(iter->first).toDatumRet(-1);
			if (lc_sql_ver_idx <= 0)
			{
				return TxBlobWString(L"数据库脚本索引号异常！");
			}
			if (this->sql_script.find(lc_sql_ver_idx) != this->sql_script.end())
			{
				return TxBlobWString(L"数据库脚本索引号重复！");
			}
			this->sql_script[lc_sql_ver_idx] = lc_sql_item_pt.dataString();
		}
		return TxBlobWString();
	}
};
