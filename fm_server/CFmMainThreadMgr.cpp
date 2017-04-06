#include "stdafx.h"
#include "CFmMainThreadMgr.h"

CFmMainThreadMgr::CFmMainThreadMgr()
{
	this->bThreadRunning = true;
}

CFmMainThreadMgr::~CFmMainThreadMgr()
{
	this->bThreadRunning = false;
	this->deal_all_function_();
	this->mEvent.setEvent();
}

void CFmMainThreadMgr::deal_all_function_()
{
	this->mMutex.lock();
	std::vector<T_FUNC_BASIC*> lc_vctFunc = this->vctFunc;
	this->vctFunc.clear();
	this->mMutex.unlock();
	for (int ii = 0; ii < (int)lc_vctFunc.size(); ++ii)
	{
		lc_vctFunc[ii]->vfExec();
		delete lc_vctFunc[ii];
	}
}

void CFmMainThreadMgr::loop()
{
	while (this->bThreadRunning)
	{
		this->deal_all_function_();
		this->mEvent.waitEvent();
	}
	this->deal_all_function_();
}

void CFmMainThreadMgr::quit()
{
	this->bThreadRunning = false;
	this->mEvent.setEvent();
}

bool CFmMainThreadMgr::is_running()
{
	return this->bThreadRunning;
}

TxBlobWString::TxComposeParameter CFmMainThreadMgr::print(const TxBlobWString &_compose)
{
	return TxBlobWString::compose(_compose, [this](const TxBlobWString &_wstr) {
		this->_push_task_([_wstr]() {
			txLogConsole("{0}").args(_wstr);
			::printf_s("%s", TxSystemDependent::TxCharsetCoding::transUnicodeToAscii(_wstr + L"\r\n").c_str());
		});
	});
}
