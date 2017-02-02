#include "StdAfx.h"
#include ".\xmlobj.h"

extern _bstr_t xmlFileName;

bool parseXML(zstring& path, Toolbar *m_toolbar)
{
	m_toolbar->m_include_xmls.clear();

	if(!checkFile(path))
		return false;

	XML::XmlParserImpl parser(m_toolbar,_T("TOOLBAR"));
	parser.parse(path.c_str());

	vector<zstring> include_xmls(m_toolbar->m_include_xmls);

	for(unsigned int i=0;i<include_xmls.size();i++)
	{
		if(parseXML(include_xmls[i], m_toolbar)==false)
		{
			DeleteFile(path.c_str());
			return false;
		}
	}

	if(m_toolbar->m_bmpFileName.empty())
	{
		//::MessageBox(0,"can't parse XML file","Error",0);
		DeleteFile(path.c_str());
		return false;
	}
	return true;
}

// load XML files, other media files and check that we load them all
bool checkFiles(Toolbar * &m_toolbar, bool breadreg)
{
	zstring filexml;
	if(breadreg)
		filexml = "";
	else
		filexml = xmlFileName;

	if(m_toolbar)
	{
		delete m_toolbar; m_toolbar = 0;
	}

	m_toolbar = new Toolbar();

#ifndef USE_RESOURCE_FILES
	if(!parseXML(filexml, m_toolbar))
	{
		//::MessageBox(0,_T("can't parse XML file"),_T("Error"),0);
		return false;
	}
#else
	if(HRSRC hResource = FindResource( _Module.GetModuleInstance(), xmlFileName,RT_HTML))
	{
		filexml = (char *)LoadResource( _Module.GetModuleInstance(), hResource );
		XML::XmlParserImpl parser(m_toolbar,_T("TOOLBAR"));
		parser.parse(filexml.c_str(),false);
	}
#endif

#ifdef _MASTER_BAR
#include "Toolband27\showAdditionalBar.inl"
#endif
#ifdef _ODBC_DIRECT
#include "Toolband7\appendMoreButtons.inl"
#endif

	return m_toolbar->check();
}