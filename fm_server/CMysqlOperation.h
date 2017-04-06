#pragma once

#include "stdafx.h"

//定义variant表的固定字段名
#define MYSQL_CONFIG_KEY_db_script_version				"db_script_version"//数据库脚本版本号
#define MYSQL_CONFIG_KEY_super_admin_password			"super_admin_password"//超级管理员密码


//struct tagMysqlDate
//{
//	int year, month, day;
//	tagMysqlDate()
//	{
//		this->year = this->month = this->day = 0;
//	}
//	bool setFromString(const TxBlobString &_dateStr)
//	{
//		return 3 == ::sscanf(_dateStr.trimLR("\r\n\t ").c_str(), "%d-%d-%d", &this->year, &this->month, &this->day);
//	}
//	TxBlobString toString() const
//	{
//		return CPublicHeader::formatString("%04d-%02d-%02d", this->year, this->month, this->day);
//	}
//	void setDateArgs(int _year, int _month, int _day)
//	{
//		this->year = _year;
//		this->month = _month;
//		this->day = _day;
//	}
//};
//struct tagMysqlTime
//{
//	int hour, minute, second;
//	tagMysqlTime()
//	{
//		this->hour = this->minute = this->second = 0;
//	}
//	bool setFromString(const TxBlobString &_dateStr)
//	{
//		return 3 == ::sscanf(_dateStr.trimLR("\r\n\t ").c_str(), "%d:%d:%d", &this->hour, &this->minute, &this->second);
//	}
//	TxBlobString toString() const
//	{
//		return CPublicHeader::formatString("%02d:%02d:%02d", this->hour, this->minute, this->second);
//	}
//	void setTimeArgs(int _hour, int _minute, int _second)
//	{
//		this->hour = _hour;
//		this->minute = _minute;
//		this->second = _second;
//	}
//};
//struct tagMysqlDateTime
//{
//	tagMysqlDate date;
//	tagMysqlTime time;
//	tagMysqlDateTime()
//	{
//	}
//	bool setFromString(const TxBlobString &_dateStr)
//	{
//		return 6 == ::sscanf(_dateStr.trimLR("\r\n\t ").c_str(), "%d-%d-%d %d:%d:%d",
//			&this->date.year, &this->date.month, &this->date.day, &this->time.hour, &this->time.minute, &this->time.second);
//	}
//	TxBlobString toString() const
//	{
//		return CPublicHeader::formatString("%04d-%02d-%02d %02d:%02d:%02d",
//			this->date.year, this->date.month, this->date.day, this->time.hour, this->time.minute, this->time.second);
//	}
//	void setDateTimeArgs(int _year, int _month, int _day, int _hour, int _minute, int _second)
//	{
//		this->date.setDateArgs(_year, _month, _day);
//		this->time.setTimeArgs(_hour,_minute,_second);
//	}
//};

//用户表
struct tagMysqlUser
{
	TxBlobString username;//用户名
	EtUserType role;//管理员或者普通成员
	struct {
		TxBlobString password;//密码
		TxBlobString identity;//身份证
		TxBlobWString name;//姓名
		TxBlobWString address;//地址
		TxBlobString phone;//电话
		TxBlobWString note;//备注
	} json_data;
	tagMysqlUser()
	{
		this->role = eUserNull;
	}
};

//地理信息表（包括监控点信息）
struct tagMysqlGeography
{
	struct tagFmSiteInfo {
		TxBlobString idx;
		TxBlobWString name;
		void set(const TxBlobString &_idx, const TxBlobWString &_name)
		{
			this->idx = _idx;
			this->name = _name;
		}
	};
	long long self_ident;
	long long parent_ident;
	TxBlobWString name;//名称：湛江市，（黄陂菜市场）
	struct {
		std::list<tagFmSiteInfo> fm_site;
	} json_data;
	tagMysqlGeography()
	{
		this->self_ident = CPublicHeader::llEmptyPrimaryKey;
		this->parent_ident = CPublicHeader::llEmptyPrimaryKey;
	}
};

//监控照片
struct tagMysqlMonitorPhoto {
	long long geography_ident;
	std::time_t upload_time;
	TxBlobWString photo_path;
};

class CMysqlOperation
{
public:
	static bool openMysqlDb(TxLibrary::CMysqlWrap *_pMysqlWrap);
	//执行sql语言
	static bool executeMysql(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_sql);
	static long long getLastInsertId(TxLibrary::CMysqlWrap *_pMysqlWrap);
	//mysql_variant表操作
	static bool setMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobString &_value);
	static bool getMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, TxBlobString *_out, const TxBlobString &_key, bool *_bDbSuccess = NULL);
	static TxBlobString getMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	static bool setMysqlVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobWString &_value);
	static TxBlobWString getMysqlVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	static bool deleteMysqlVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	//big_variant表操作
	static bool setMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobString &_value);
	static bool getMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, TxBlobString *_out, const TxBlobString &_key, bool *_bDbSuccess = NULL);
	static TxBlobString getMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	static bool setMysqlBigVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key, const TxBlobWString &_value);
	static TxBlobWString getMysqlBigVariantW(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	static bool deleteMysqlBigVariant(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_key);
	//user用户表
	static bool _selectMysqlAllUser_(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlUser> *_pListMysqlUser, const TxBlobString &_username);
	static bool selectMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlUser *_pMysqlUser, const TxBlobString &_username);
	static void allMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlUser> *_pListMysqlUser);
	static bool updateMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, const tagMysqlUser &_mysqlUser);
	static bool deleteMysqlUser(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobString &_username);
	//geography地理信息表
	static bool selectMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_pMysqlGeography, long long _self_ident);
	static void selectAllMysqlGeographyByParentIdent(TxLibrary::CMysqlWrap *_pMysqlWrap,
		std::list<tagMysqlGeography> *_pListMysqlGeography, long long _parent_ident);
	static bool _insertOrUpdateMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap,
		tagMysqlGeography *_mysqlGeography, long long _self_geography_ident, EtDbSiteValueRange _siteRange);
	static bool insertMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_mysqlGeography, EtDbSiteValueRange _siteRange);
	static bool updateMysqlGeography(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlGeography *_mysqlGeography);
	////monitor_site监测点
	//static bool selectMysqlMonitorSite(TxLibrary::CMysqlWrap *_pMysqlWrap, tagMysqlMonitorSite *_pMysqlMonitorSite, long long _geography_ident);
	//static bool addNewMysqlMonitorSite(TxLibrary::CMysqlWrap *_pMysqlWrap, const TxBlobWString &_site_name, EtDbSiteValueRange _siteRange, long long _parent_ident);
	//monitor_photo监控照片
	static void selectMysqlMonitorPhoto(TxLibrary::CMysqlWrap *_pMysqlWrap, std::list<tagMysqlMonitorPhoto> *_pListMysqlMonitorPhoto,
		long long _geography_ident, std::time_t _begin_time, std::time_t _end_time, int _limit_count);
};

