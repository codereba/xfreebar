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

#ifndef __VCRITICAL_SECTION_H__
#define __VCRITICAL_SECTION_H__

/***************************************************************************************************
	作者:史继杰
	文件名:VCriticalSection.h
	文件描述:多个线程之间进行同步的临界区
	日期:2008-12-28
***************************************************************************************************/
class VCriticalSection
{
public:
	VCriticalSection(void);
	~VCriticalSection(void);

	//进入临界区，只有一个线程可以访问之后的代码
	void Enter();
	//离开临界区，之后的代码访问不再受单线程的限制
	void Leave();

protected:
#ifdef _WIN32
	//WIN32的临界区类
	CRITICAL_SECTION m_CriticalSection;
#else  // Linux X86 or ARM9
	//Linux pthread 库的互斥体类
	pthread_mutex_t m_CriticalSection;
#endif
};

#endif //__VCRITICAL_SECTION_H__

