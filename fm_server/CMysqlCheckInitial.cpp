#include "stdafx.h"
#include "CMysqlCheckInitial.h"
#include "CMysqlOperation.h"
#include "CFmMainThreadMgr.h"
#include "CFmResourceMgr.hpp"

CMysqlCheckInitial::CMysqlCheckInitial()
{
}

bool CMysqlCheckInitial::init_db_version_()
{
	bool ret_false = false;
	int lc_cur_ver = CMysqlOperation::getMysqlVariant(&this->m_MysqlWrap, MYSQL_CONFIG_KEY_db_script_version).toDatumRet(-1);
	std::map<int, TxBlobString> lc_sql_script = CFmResourceMgr::object()->sql_script;
	this->m_MysqlWrap.transaction([this, &ret_false, &lc_sql_script, &lc_cur_ver]()->bool {
		for each(auto elem in lc_sql_script)
		{
			if (elem.first > lc_cur_ver)
			{
				if (elem.second.length() > 0 && !CMysqlOperation::executeMysql(&this->m_MysqlWrap, elem.second))
				{
					CFmMainThreadMgr::object()->print(L"�������ݿ�ű�ʧ�ܣ���{0}��").args(elem.second);
					CFmMainThreadMgr::object()->quit();
					return false;
				}
				if (!CMysqlOperation::setMysqlVariant(&this->m_MysqlWrap, MYSQL_CONFIG_KEY_db_script_version, TxBlobString::fromDatum(elem.first)))
				{
					CFmMainThreadMgr::object()->print(L"�޷��������ݿ�ű������ţ�");
					CFmMainThreadMgr::object()->quit();
					return false;
				}
			}
		}
		ret_false = true;
		return true;
	});
	return ret_false;
}

bool CMysqlCheckInitial::init_default_value_()
{
	//��ʼ����������Ա����
	TxBlobString lc_super_admin_password = CMysqlOperation::getMysqlVariant(&this->m_MysqlWrap, MYSQL_CONFIG_KEY_super_admin_password);
	if (lc_super_admin_password.length() <= 0)
		CMysqlOperation::setMysqlVariant(&this->m_MysqlWrap, MYSQL_CONFIG_KEY_super_admin_password, "fm_2007");
	return true;
}

bool CMysqlCheckInitial::upgrade()
{
	CMysqlCheckInitial lcMysqlCheckInitial;
	if (!CMysqlOperation::openMysqlDb(&lcMysqlCheckInitial.m_MysqlWrap))
	{
		CFmMainThreadMgr::object()->print(L"�����ݿ�ʧ�ܣ�");
		CFmMainThreadMgr::object()->quit();
		return false;
	}
	if (!lcMysqlCheckInitial.init_db_version_())
	{
		CFmMainThreadMgr::object()->print(L"�������ݿ�汾ʧ�ܣ�");
		CFmMainThreadMgr::object()->quit();
		return false;
	}
	if (!lcMysqlCheckInitial.init_default_value_())
	{
		CFmMainThreadMgr::object()->print(L"��ʼ�����ݿ�Ĭ��ֵʧ�ܣ�");
		CFmMainThreadMgr::object()->quit();
		return false;
	}
	return true;
}

