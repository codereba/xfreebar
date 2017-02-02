/** $Header:: /ApoBase/ApoBaseMembers.h 1     8.06.01 1 $*
/* Contents-----------------------------------------------------------------**
**                                                                          **
**                                                                          **
$*                                                                          */
/*--------------------------------------------------------------------------**
** Designed by Mike Melnikov.                                               **
** COPYRIGHT (C) 2001 by ApoServer Crew of ApoSoft                          **
** All rights reserved.                                                     **
** -------------------------------------------------------------------------*/

#ifndef APOBASE_API_H
#define APOBASE_API_H

#ifdef APOBASE_EXPORTS
#define APOBASE_API __declspec(dllexport)
#elif defined(APOBASE_LIB)
#define APOBASE_API 
#else
#define APOBASE_API __declspec(dllimport)
#endif

#define APOBASE_API 
#endif

