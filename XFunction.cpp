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

#include "StdAfx.h"
#include "XFunction.h"

INT32 GeAllSoftWareInfoOnMachine()
{
#define ERROR_OPEN_SOFTWARE_MAIN_KEY -110

#define SOFT_PROP_VAL_MAX_LEN ( INT32 )( 1024 )
#define ALL_SOFTWARE_INSTALL_INFO _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall" )
#define SOFTWARE_MAX_NAME_LEN ( INT32 )( 1024 )
	//HKEY SoftWareInfoKey;
    INT32 nRegKeyIndex;
	//INT32 nSoftInfoKeyIndex;
	CRegKey RegKey;
	CRegKey OneSoftInfoKey;
	LONG lRet;
	TCHAR szSoftKeyName[ SOFTWARE_MAX_NAME_LEN ];
	TCHAR szSoftValue[ SOFT_PROP_VAL_MAX_LEN ];
	DWORD dwSoftKeyNameLen;
	DWORD dwSoftPropValLen;
	INT32 nRet;

	lRet = RegKey.Open( HKEY_LOCAL_MACHINE, ALL_SOFTWARE_INSTALL_INFO );
	if( lRet != ERROR_SUCCESS )
	{
		ATLTRACE( _T( "Open the registry key failed\r\n" ) );
		return ERROR_OPEN_SOFTWARE_MAIN_KEY;
	}

	nRegKeyIndex = 0;
	while( 1 )
	{
		dwSoftKeyNameLen = SOFTWARE_MAX_NAME_LEN;
		lRet = RegKey.EnumKey( nRegKeyIndex, szSoftKeyName, &dwSoftKeyNameLen );

		if( lRet != ERROR_SUCCESS )
		{
			break;
		}

		//ATLTRACE( _T( "%s\r\n" ), szSoftKeyName );

#define PROP_DISPLAYICON_VALUE _T( "DisplayIcon" )
#define PROP_DISPLAYNAME_VALUE _T( "DisplayName" )
#define PROP_DISPLAYVERSION_VAULE _T( "DisplayVersion " )
#define PROP_PUBLISHER_VALUE _T( "Publisher" )
#define PROP_UNINSTALLSTRING_VALUE _T( "UninstallString" )
#define PROP_URLINFOABOUT_VALUE _T( "URLInfoAbout" )
#define PROP_INSTALLLOCATION_VALUE _T( "InstallLocation" )
		static TCHAR *AllPropName[] = 
		{
			PROP_DISPLAYICON_VALUE,
			PROP_DISPLAYNAME_VALUE,
			PROP_DISPLAYVERSION_VAULE, 
			PROP_PUBLISHER_VALUE,
			PROP_UNINSTALLSTRING_VALUE,
			PROP_URLINFOABOUT_VALUE,
			PROP_INSTALLLOCATION_VALUE
		};

		lRet = OneSoftInfoKey.Open( ( HKEY )RegKey, szSoftKeyName );
		if( lRet != ERROR_SUCCESS )
		{
			ATLTRACE( _T( "Open the software info key %s failed\r\n" ), szSoftKeyName );
			nRegKeyIndex += 1;
			continue;
		}

		dwSoftPropValLen = SOFTWARE_MAX_NAME_LEN;
		lRet = OneSoftInfoKey.QueryStringValue( PROP_DISPLAYICON_VALUE, szSoftValue, &dwSoftPropValLen );
		if( lRet != ERROR_SUCCESS )
		{
			ATLTRACE( _T( "Get the software info key prop %s failed\r\n" ), PROP_DISPLAYICON_VALUE );
			nRegKeyIndex += 1;
			continue;
		}

		ATLTRACE( _T( "Property: %s value is %s\r\n" ), PROP_DISPLAYICON_VALUE, szSoftValue );

		nRet = GetIconFromExeFile( szSoftValue );
		Sleep( 1000 );
		nRegKeyIndex += 1;
	}
	return XSUCCESS;
}

BOOL CALLBACK EnumIconProc( HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam )
{
	INT32 nRet;
	HDC hDC;
	ULONG uType;
	ULONG uID;
	HICON hIcon;

	hDC = ::GetDC( NULL );
	if( NULL == hDC )
	{
		return TRUE;
	}

#define RES_STR_NAME_BEGIN_MARK '#'
	if( IS_INTRESOURCE( lpType ) )
	{
		uType = ( ULONG )lpType;
	}
	else
	{
		if( lpType[ 0 ] == RES_STR_NAME_BEGIN_MARK )
		{
			uType = strtoul( lpName + sizeof( TCHAR ), NULL, 10 );
		}	
	}

	if( IS_INTRESOURCE( lpName ) )
	{
		uID = ( ULONG )lpName;
	}
	else
	{
		if( lpName[ 0 ] = RES_STR_NAME_BEGIN_MARK )
		{
			uID = strtoul( lpName + sizeof( TCHAR ), NULL, 10 );
		}	
	}

	hIcon = LoadIcon( hModule, MAKEINTRESOURCE( uID ) );
	if( hIcon == NULL )
	{
		ATLTRACE( _T( "Load the icon from the file failed\r\n" ) );
		return TRUE;
	}

	nRet = DrawIconEx( hDC, 10, 10, hIcon, 100, 100, 0, NULL, DI_COMPAT | DI_DEFAULTSIZE );
	if( nRet == FALSE )
	{
		ATLTRACE( _T( "Draw the icon failed\r\n" ) );
	}

	DestroyIcon( hIcon );
	ReleaseDC( NULL, hDC );
	return TRUE;
}

INT32 GetIconFromExeFile( LPCTSTR lpszFileName )
{
	HINSTANCE hLib;
	//Load the DLL/EXE - NOTE must be a 32bit EXE/DLL for this to work
	if( ( hLib = LoadLibraryEx( lpszFileName, NULL, LOAD_LIBRARY_AS_DATAFILE ) ) == NULL )
	{
		//Failed to load - abort
		return -11;
	}

	if( !EnumResourceNames( hLib, RT_GROUP_ICON, ( ENUMRESNAMEPROC )EnumIconProc, ( LPARAM )NULL ) )
	{
		return -12;
	}

	return 0;
}