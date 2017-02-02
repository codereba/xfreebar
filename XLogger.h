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

#ifndef _XLOGGER_H_
#define _XLOGGER_H_

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "VCriticalSection.h"

#ifdef _WIN32
#else
#include <unistd.h>
#endif

class XLogger
{
public:
	//��־��¼ģʽ������
#define LOG_TO_DEBUGER		0x01
#define LOG_TO_FILE			0x02
#define LOG_TO_CONSOLE		0x04
#define LINE_BUFFER_SIZE	1024
	//����ʵ����������־,����Ӧ��ģʽ,����,�ļ���,ͬʱ˵���Ƿ�����ӷ�ʽ�����м�¼
	XLogger( LPCTSTR filename = NULL, INT32 level = 1, INT32 mode = LOG_TO_DEBUGER | LOG_TO_FILE | LOG_TO_CONSOLE, bool append = false);
	
	//��ӡ��־��Ϣ
	inline VOID Print(INT32 level, LPCTSTR format, ...) {
		if (level > m_level) return;
			va_list ap;
			va_start(ap, format);
			ReallyPrint( format, ap);
			va_end(ap);
	}

	void SetMode(INT32 mode);

	INT32 GetMode();

	//������־�Ĺ�������
	VOID SetLevel(INT32 level);
	//�õ���־�Ĺ�������
	INT32  GetLevel();

	//������־�ļ�·��
	VOID SetFile(LPCTSTR filename, bool append = false);
	virtual ~XLogger();
private:
	//���մ�ӡ��־��Ϣ
	VOID ReallyPrintLine( LPCTSTR line);
	//���մ�ӡ��־��Ϣ
	VOID ReallyPrint( LPCTSTR format, va_list ap);
	//����־�ļ�
	VOID OpenFile();
	//�ر���־�ļ�
	VOID CloseFile();

	bool m_tofile, m_todebug, m_toconsole;
	INT32 m_level;
	INT32 m_mode;
	HANDLE hlogfile;
	LPSTR m_filename;
	bool m_append;

	time_t m_lastLogTime;
	VCriticalSection m_CriticalSection;
};

#endif // _XLOGGER_H_
