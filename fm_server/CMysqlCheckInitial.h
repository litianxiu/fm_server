#pragma once

#include "stdafx.h"

class CMysqlCheckInitial
{
private:
	TxLibrary::CMysqlWrap m_MysqlWrap;
private:
	CMysqlCheckInitial();
	bool init_db_version_();
	bool init_default_value_();
public :
	static bool upgrade();
};

