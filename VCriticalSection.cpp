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

#include "stdafx.h"
#include "VCriticalSection.h"

#ifndef _WIN32
#include <pthread.h>
#endif

VCriticalSection::VCriticalSection()
{
#ifdef WIN32
	InitializeCriticalSection( &m_CriticalSection ); 
#else 
 
  pthread_mutexattr_t Attribute;
  int Result = pthread_mutexattr_init(&Attribute);
  if( Result != 0 )
    return;
   
  try
  {
    Result = pthread_mutexattr_settype( &Attribute, PTHREAD_MUTEX_RECURSIVE );
    if( Result != 0 )
      return;

    Result = pthread_mutex_init( &m_CriticalSection, &Attribute );
    pthread_mutexattr_destroy(&Attribute);

  }
  catch( ... )
  {
     pthread_mutexattr_destroy(&Attribute);
  }
#endif
}

VCriticalSection::~VCriticalSection()
{
#ifdef WIN32
	DeleteCriticalSection( &m_CriticalSection );
#else 
  pthread_mutex_destroy( &m_CriticalSection );
#endif
}

void VCriticalSection::Enter()
{
#ifdef WIN32
	EnterCriticalSection( &m_CriticalSection );
#else 
  pthread_mutex_lock( &m_CriticalSection );
#endif
}

void VCriticalSection::Leave()
{
#ifdef WIN32
	LeaveCriticalSection( &m_CriticalSection );
#else 
  pthread_mutex_unlock( &m_CriticalSection );
#endif
}
