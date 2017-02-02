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
	����:ʷ�̽�
	�ļ���:VCriticalSection.h
	�ļ�����:����߳�֮�����ͬ�����ٽ���
	����:2008-12-28
***************************************************************************************************/
class VCriticalSection
{
public:
	VCriticalSection(void);
	~VCriticalSection(void);

	//�����ٽ�����ֻ��һ���߳̿��Է���֮��Ĵ���
	void Enter();
	//�뿪�ٽ�����֮��Ĵ�����ʲ����ܵ��̵߳�����
	void Leave();

protected:
#ifdef _WIN32
	//WIN32���ٽ�����
	CRITICAL_SECTION m_CriticalSection;
#else  // Linux X86 or ARM9
	//Linux pthread ��Ļ�������
	pthread_mutex_t m_CriticalSection;
#endif
};

#endif //__VCRITICAL_SECTION_H__

