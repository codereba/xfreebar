/*
 * Copyright 2010 www.codereba.com
 *
 * This file is part of xfreebar.
 *
 * xfreebar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xfreebar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreebar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once
#include <vector>

#define REGT_TSTR	0
#define REGT_STR		1
#define REGT_WSTR	2
#define REGT_DWORD	3
#define REGT_BSTR	4

#define AUTO_DESTROY	0x00
#define AUTO_TIMER		0x01
#define AUTO_SETVAL		0x02 

//extern CRegRefMgr RefMgr;

__interface IAutoRefresh
{
	void AutoRefresh() = 0;
};

class CRegRefMgr
{
#define REFRESH_TIME	1000 * 60 * 5

	typedef std::vector<IAutoRefresh*> REF_LIST;
	REF_LIST m_RefMembers;
	//static CWinThread m_RefreshThread;
	static void AutoRefreshOnTimer(REF_LIST *pList)
	{
		
		for(REF_LIST::iterator i = pList->begin();i != pList->end();i++) 
		{
			(*i)->AutoRefresh();
		}
	}

	CRegRefMgr()
	{
	}

	~CRegRefMgr()
	{
		m_RefMembers.clear();
	}
	
	INT32 AddRefMember(IAutoRefresh* pRefMember)
	{
		m_RefMembers.push_back(pRefMember);
		return 1;
	}
};

HKEY GetAppRegistryKey();
template <class TVal, class ARG_TYPE = TVal& >
class CRegWrapper
{
public:

	CRegWrapper(LPCTSTR lpszKeyPath, UINT nAutoType = AUTO_DESTROY)
	{
		InitParam(lpszKeyPath, nAutoType );
	}

	bool InitParam(LPCTSTR lpszKeyPath, UINT nAutoType = AUTO_DESTROY)
	{
		m_strName = lpszKeyPath;
		m_nAutoType = nAutoType;
		m_regKey.Attach(GetAppRegistryKey());
#ifdef REG_MANAGER
		if(m_nAutoType, AUTO_TIMER)
		{
			RefMgr.AddRefMember(this);
		}
#endif
		if(ERROR_SUCCESS!=m_regKey.QueryValue(m_Value, m_strName))
		{
			TVal TmpVal;
			OrgValue(TmpVal);
			return false;
		}
		CheckValue(m_Value);
		return true;
	}


	~CRegWrapper(void)
	{
		SaveValue();
	}

	CRegKey m_regKey;
	bstr_t m_strName;
	//UINT m_nValType;
	UINT m_nAutoType;
	TVal m_Value;

	void SetValue(TVal Value)
	{
		m_Value = Value;
		if(m_nAutoType, AUTO_SETVAL)
		{
			m_regKey.SetValue(m_Value,m_strName);
		}
	}

	void OrgValue(DWORD &uOrgVal)
	{
		m_Value = 0;
		SaveValue();
	}

	void OrgValue(LPCTSTR &lpszValue)
	{
		m_Value = _T("0");
		SaveValue();
	}

	void CheckValue(DWORD Value)
	{
		//if(m_Value > 0x7fffffff)
		if((long)m_Value < 0)
		{
			m_Value = 0;
		}
	}

	inline void SaveValue()
	{
		m_regKey.SetValue(m_Value,m_strName);
	}

	operator DWORD () throw()
	{
		return m_Value;;
	}

	CRegWrapper<TVal>& operator = (TVal Value)
	{
		m_Value = Value;
		if(m_nAutoType, AUTO_SETVAL)
		{
			m_regKey.SetValue(m_Value,m_strName);
		}
		return *this;
	}

	CRegWrapper<TVal>& operator += (TVal Value)
	{
		m_Value += Value;
		if(m_nAutoType, AUTO_SETVAL)
		{
			m_regKey.SetValue(m_Value,m_strName);
		}
		return *this;
	}

	inline bool GetAutoState(UINT uState, UINT uMask)
	{
		return uState & uMask;
	}
};
