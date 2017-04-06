// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include "../../public/include/CLibraryHeader.hpp"
#include "../../public/net_protocol/CNetProtocolMgr.hpp"

//mysql
#include <errmsg.h>
#include <mysql.h>
#include "../../public/library_wrap/CMysqlWrap.hpp"
#pragma comment(lib, "libmysql.lib")

