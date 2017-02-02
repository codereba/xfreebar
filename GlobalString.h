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

#ifndef __GLOBAL_STRING_H__
#define __GLOBAL_STRING_H__

#define ADD_TO_DROP_MENU 20

// some toolbar messages
#define XTOOLBAR_OPTION_TITLE _T("X")
#ifdef _GERMAN
#define XTOOLBAR_ALERT_TITLE XTOOLBAR_OPTION_TITLE _T(" Toolbar Warnung")
#define XTOOLBAR_CRITICAL_TITLE XTOOLBAR_OPTION_TITLE _T( " Toolbar kann keine Daten von " ) XUPDATE_SERVER_URL _T( " erhalten.\nBitte eine Internet-Verbindung herstellen und den Browser neu starten." )
#define XTOOLBAR_UNINSTALL_MSG _T( "Sind Sie sicher, dass die " ) XTOOLBAR_OPTION_TITLE _T(" Toolbar von Ihrem Computer entfernt werden soll?")
#else
#define XTOOLBAR_ALERT_TITLE XTOOLBAR_OPTION_TITLE _T(" Toolbar Alert")
#define XTOOLBAR_CRITICAL_TITLE XTOOLBAR_OPTION_TITLE _T(" Toolbar can't retrive infomation from ") XUPDATE_SERVER_URL _T("\nPlease connect the internet and restart your browser.")
#define XTOOLBAR_UNINSTALL_MSG _T("This will remove ") XTOOLBAR_OPTION_TITLE _T(" Toolbar from your computer! Are you sure?")
#endif

#define XML_CONF_FILE_NAME	_T( "xfreebar_config.xml" )
#define XUPDATE_SERVER_URL _T( "C:/xfreebar/web/" ) //_T( "http://127.0.0.1/toolbar/" )
#define XREG_KEY_NAME _T( "CoderebaSoft\\XFreeBar" )
#define XTOOLBAR_CONF_FILE_NAME _T( "xfreebar_config.xml" ) //_T( "mBLASTtoolbar.xml" )
#define OPTION_PAGE_TITLE _T( "XToolBar Options" )
#define PROFILE_FILE_NAME _T( "Config" )
#define URL_AFTER_UNINSTALL _T( "http://www.google.com" ) //_T( "http://www.sysvw.com" )//_T( "http://www.zmike.net" )
#define URL_AFTER_UPDATE _T( "http://www.google.com" ) // _T( "http://www.sysvw.com" ) //_T( "http://www.zmike.net" )
#define MAIN_SITE_URL _T( "http://www.google.com" ) // _T( "http://www.sysvw.com" ) //_T( "http://zmanagers.virtualave.net/" ) 
#define XTOOLBAR_UUID _T( "{D8B50602-86BE-4887-B5D5-017C59D3EB14}" ) //"{5F1ABCDB-A875-46c1-8345-B72A4567E486}"

#endif //__GLOBAL_STRING_H__