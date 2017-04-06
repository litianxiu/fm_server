#pragma once

#include "stdafx.h"
#include <stdio.h>
#include "../../public/include/CLibraryHeader.hpp"

class CFmMainThreadMgr : public TxSingletonTmpl<CFmMainThreadMgr>
{
private:
	class T_FUNC_BASIC
	{
	public:
		virtual ~T_FUNC_BASIC() {}
		virtual void vfExec() = 0;
	};
	template<class TFUNC>
	class T_FUNC_IMPL : public T_FUNC_BASIC
	{
	private:
		TFUNC func;
	public:
		T_FUNC_IMPL(const TFUNC &_func) :func(_func) { }
		~T_FUNC_IMPL() { }
		virtual void vfExec() { return this->func(); }
	};
private:
	TxSystemDependent::TxMutexWrap mMutex;
	TxSystemDependent::TxEventWrap mEvent;
	std::vector<T_FUNC_BASIC*> vctFunc;
	TxSystemDependent::TxThreadSafeBoolean bThreadRunning;
private:
	template<class TFUNC>
	void _push_task_(const TFUNC &_func)
	{
		T_FUNC_BASIC *lc_func_ptr =new T_FUNC_IMPL<TFUNC>(_func);
		this->mMutex.lock();
		this->vctFunc.push_back(lc_func_ptr);
		this->mMutex.unlock();
		this->mEvent.setEvent();
	}
private:
	void deal_all_function_();
public:
	CFmMainThreadMgr();
	~CFmMainThreadMgr();
	void loop();
	void quit();
	bool is_running();
	TxBlobWString::TxComposeParameter print(const TxBlobWString &_compose);
};

