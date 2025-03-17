// CHTTPClient.cpp : Implementation of the CHTTPClient class.
//

#include "stdafx.h"
//#include "DownCtrl.h"
#include "HTTPClient.h"
#include <winreg.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "WS2_32.lib")

inline char * Wc2KoreanMb(char *koreanStr, const TCHAR *unicodeStr, int bufSize)
{
	// 	if(lstrlen(unicodeStr) == 0)
	// 	{
	// 		koreanStr[0] = '\0';
	// 		return koreanStr;
	// 	}
#ifdef _UNICODE
	WideCharToMultiByte(949, 0, unicodeStr, -1/*lstrlen(unicodeStr) + 1*/, koreanStr, bufSize, 0, 0);
#else
	lstrcpy(koreanStr, unicodeStr);
#endif
	return koreanStr;
};

inline TCHAR * KoreanMb2Wc(TCHAR *unicodeStr, const char *koreanStr, int bufSize)
{
	// 	if(strlen(koreanStr) == 0)
	// 	{
	// 		unicodeStr[0] = _T('\0');
	// 		return unicodeStr;
	// 	}
#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, koreanStr, -1/*(int)strlen(koreanStr) + 1*/, unicodeStr, bufSize);
#else
	lstrcpy(unicodeStr, koreanStr);
#endif

	return unicodeStr;
};

CHTTPClient::CHTTPClient()
{
	m_hHTTPOpen			= NULL;
	m_hHTTPConnection	= NULL;
	m_hHTTPRequest		= NULL;
	m_nSecureFlags		= INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|
		INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID|INTERNET_FLAG_NO_AUTO_REDIRECT;
	m_bUseSSL			= FALSE;

	m_szResponseBody = new TCHAR[HTTP_BUFFER_SIZE];
	m_szResponseHeader = new TCHAR[HTTP_BUFFER_SIZE];
	m_pTempBuffer1 = new TCHAR[HTTP_BUFFER_SIZE];
	m_pTempBuffer2 = new BYTE[HTTP_BUFFER_SIZE];
	m_nAgent = 0;
	m_szHost = NULL;

	SetHeaderClear();

	m_bNoNeedResponse = FALSE;
	m_bSpecial = FALSE;
	m_szLoginCookie[0] = 0;
	m_dwError = 0;
}

void	CHTTPClient::SetAgent(int nAgent)
{	
	m_nAgent = nAgent;
}

void	CHTTPClient::SetPragma(const TCHAR *szPragma)
{
	if(szPragma == NULL)
	{
		m_strPragma = L"";
		return;
	}
	m_strPragma.Format(_T("Pragma: %s\r\n"), szPragma);
}

void	CHTTPClient::SetXRequestedWith(const TCHAR *szXRequestedWith)
{
	if(szXRequestedWith == NULL)
	{
		m_strXRequestedWith = L"";
		return;
	}
	m_strXRequestedWith.Format(_T("X-Requested-With: %s\r\n"), szXRequestedWith);
}
void	CHTTPClient::SetHost(const TCHAR *szHost)
{
	if(szHost == NULL)
	{
		m_strHost = L"";
		return;
	}
	m_strHost.Format(_T("Host: %s\r\n"), szHost);
}
void	CHTTPClient::SetRefer(const TCHAR *szRefer)
{
	if(szRefer == NULL)
	{
		m_strRefer = L"";
		return;
	}
	m_strRefer.Format(_T("Referer: %s\r\n"), szRefer);
}

void	CHTTPClient::SetContentType(const TCHAR *szContentType)
{
	if(szContentType == NULL)
	{
		m_strContentType = L"";
		return;
	}
	m_strContentType.Format(_T("Content-Type: %s\r\n"), szContentType);
}

void	CHTTPClient::SetMethod(const TCHAR *szMethod)
{
	if(szMethod == NULL)
	{
		m_strMethod = L"";
		return;
	}
	m_strMethod.Format(_T("method: %s\r\n"), szMethod);
}

void	CHTTPClient::SetOrigin(const TCHAR *szOrigin)
{
	if(szOrigin == NULL)
	{
		m_strOrigin = L"";
		return;
	}
	m_strOrigin.Format(_T("Origin: %s\r\n"), szOrigin);
}

void	CHTTPClient::SetTopic(const TCHAR *szTopic)
{
	if(szTopic == NULL)
	{
		m_strTopic = L"";
		return;
	}
	m_strTopic.Format(_T("topic: %s\r\n"), szTopic);
}

void	CHTTPClient::SetTransporttimeout(const TCHAR *szTransporttimeout)
{
	if(szTransporttimeout == NULL)
	{
		m_strTransporttimeout = L"";
		return;
	}
	m_strTransporttimeout.Format(_T("transporttimeout: %s\r\n"), szTransporttimeout);
}

void	CHTTPClient::SetType(const TCHAR *szType)
{
	if(szType == NULL)
	{
		m_strType = L"";
		return;
	}
	m_strType.Format(_T("type: %s\r\n"), szType);
}

void	CHTTPClient::SetClientid(const TCHAR *szClientid)
{
	if(szClientid == NULL)
	{
		m_strClientid = L"";
		return;
	}
	m_strClientid.Format(_T("clientid: %s\r\n"), szClientid);
}

void	CHTTPClient::SetS(const TCHAR *szS)
{
	if(szS == NULL)
	{
		m_strS = L"";
		return;
	}
	m_strS.Format(_T("s: %s\r\n"), szS);
}

void	CHTTPClient::SetACRHeaders(const TCHAR *szACRHeaders)
{
	if(szACRHeaders == NULL)
	{
		m_strACRHeaders = L"";
		return;
	}
	m_strACRHeaders.Format(_T("Access-Control-Request-Headers: %s\r\n"), szACRHeaders);
}

void	CHTTPClient::SetAcceptLanguage(const TCHAR *szAcceptLanguage)
{
	if(szAcceptLanguage == NULL)
	{
		m_strAcceptLanguage = L"";
		return;
	}
	m_strAcceptLanguage.Format(_T("Accept-Language: %s\r\n"), szAcceptLanguage);
}

void	CHTTPClient::SetAcceptEncoding(const TCHAR *szAcceptEncoding)
{
	if(szAcceptEncoding == NULL)
	{
		m_strAcceptEncoding = L"";
		return;
	}
	m_strAcceptEncoding.Format(_T("Accept-Encoding: %s\r\n"), szAcceptEncoding);
}

void	CHTTPClient::SetSubs(const TCHAR *szSubs)
{
	if(szSubs == NULL)
	{
		m_strSubs = L"";
		return;
	}
	m_strSubs.Format(_T("subs: %s\r\n"), szSubs);
}

void	CHTTPClient::SetUnsubs(const TCHAR *szUnsubs)
{
	if(szUnsubs == NULL)
	{
		m_strUnsubs = L"";
		return;
	}
	m_strUnsubs.Format(_T("unsubs: %s\r\n"), szUnsubs);
}

void	CHTTPClient::SetDNT(const TCHAR *szDNT)
{
	if (szDNT == NULL)
	{
		m_strDNT = L"";
		return;
	}
	m_strDNT.Format(_T("DNT: %s\r\n"), szDNT);
}

void	CHTTPClient::SetHeaderClear()
{
	m_strHost = L"";
	m_strRefer = L"";
	m_strContentType = L"";
	m_strXRequestedWith = L"";
	m_strPragma = L"";
	m_strMethod = L"";
	m_strOrigin = L"";
	m_strTopic = L"";
	m_strTransporttimeout = L"";
	m_strType = L"";
	m_strClientid = L"";
	m_strS = L"";
	m_strAcceptLanguage = L"";
	m_strAcceptEncoding = L"";
	m_strSubs = L"";
	m_strUnsubs = L"";
}

CHTTPClient::~CHTTPClient()
{
	m_hHTTPOpen			= NULL;
	m_hHTTPConnection	= NULL;
	m_hHTTPRequest		= NULL;

	if(m_szResponseBody)
		delete [] m_szResponseBody;
	if(m_szResponseHeader)
		delete [] m_szResponseHeader;
	if(m_pTempBuffer1)
		delete [] m_pTempBuffer1;
	if(m_pTempBuffer2)
		delete [] m_pTempBuffer2;
}

CHTTPClient::RequestMethod CHTTPClient::GetMethod(int nMethod)
{	
	return nMethod <= 0 ? CHTTPClient::RequestGetMethod : static_cast<CHTTPClient::RequestMethod>(nMethod);
}

CHTTPClient::TypePostArgument CHTTPClient::GetPostArgumentType(int nType)
{
	return nType <= 0 ? CHTTPClient::TypeNormal : static_cast<CHTTPClient::TypePostArgument>(nType);
}

BOOL CHTTPClient::Connect(CString szAddress, LPCTSTR szAgent, unsigned short nPort, LPCTSTR szUserAccount, LPCTSTR szPassword)
{
	if(m_nAgent)
		szAgent = __DEFAULT_AGENT_NAME2;
	else
		szAgent = __DEFAULT_AGENT_NAME;

	m_hHTTPOpen = ::InternetOpen(
						szAgent,						// agent name
						INTERNET_OPEN_TYPE_PRECONFIG,	// proxy option
						_T(""),							// proxy
						_T(""),							// proxy bypass
						0);								// flags
	if(!m_hHTTPOpen)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}	

	//TCHAR lpszData[HTTP_BUFFER_SIZE];              // buffer to hold the cookie data
	//DWORD dwCookieSize=HTTP_BUFFER_SIZE;           // variable to get the buffer size needed

	//if(InternetGetCookie(_T("http://aion.plaync.co.kr"), NULL, lpszData, &dwCookieSize ))
	//{
	//	_stprintf(m_szLoginCookie, _T("Cookie: %s\r\n"),lpszData);
	//	//InternetSetCookie(szAddress, NULL, m_szLoginCookie);
	//}

	m_hHTTPConnection=::InternetConnect(
							m_hHTTPOpen,	// internet opened handle
							(LPCTSTR)szAddress,		// server name
							nPort,			// ports
							_T(""),	// user name
							_T(""),		// password 
							INTERNET_SERVICE_HTTP, // service type
							INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,	// service option
							0);				// context call-back option

	if(!m_hHTTPConnection)
	{		
		m_dwError=::GetLastError();
		::CloseHandle(m_hHTTPOpen);
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

//	if(::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
//	{
//		m_dwError = ::GetLastError();
//		::CloseHandle(m_hHTTPConnection);
//		::CloseHandle(m_hHTTPOpen);
//#ifdef	_DEBUG
//		LPVOID     lpMsgBuffer;
//		DWORD dwRet = FormatMessage(
//							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//							NULL,
//							::GetLastError(),
//							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
//							0,
//							NULL);
//		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
//		LocalFree(lpMsgBuffer);		
//#endif
//		return FALSE;
//	}
	return TRUE;															
}

BOOL CHTTPClient::Close()
{
	if(m_hHTTPRequest)
		::InternetCloseHandle(m_hHTTPRequest);

	if(m_hHTTPConnection)
		::InternetCloseHandle(m_hHTTPConnection);

	if(m_hHTTPOpen)
		::InternetCloseHandle(m_hHTTPOpen);

	return TRUE;
}

BOOL CHTTPClient::Request(CString szURL, DWORD serverPort, BOOL bUseSSL, int nMethod, LPCTSTR szAgent)
{
	BOOL	bReturn = TRUE;
	DWORD	dwPort = 0;
	CString szProtocol, szAddress, szURI;
	DWORD	dwSize = 0;

	if(m_nAgent)
		szAgent = __DEFAULT_AGENT_NAME2;
	else
		szAgent = __DEFAULT_AGENT_NAME;

	m_bUseSSL = bUseSSL;
	ParseURL(szURL, szProtocol, szAddress, dwPort, szURI);
	dwPort = serverPort;
	if(Connect(szAddress, szAgent, (unsigned short)serverPort))
	{
		if(!RequestOfURI(szURI, nMethod))
		{
			bReturn = FALSE;
		}
		else
		{
			memset(m_szResponseHeader, 0, sizeof(TCHAR)*HTTP_BUFFER_SIZE);
			memset(m_szResponseBody, 0, sizeof(TCHAR)*HTTP_BUFFER_SIZE);
			if(!Response((PBYTE)m_szResponseHeader, HTTP_BUFFER_SIZE, (PBYTE)m_szResponseBody, HTTP_BUFFER_SIZE, dwSize))
			{
				bReturn = FALSE;		
			}
		}

		Close();
	}
 	else
	{
		bReturn = FALSE;
	}

	m_bNoNeedResponse = FALSE;
	return bReturn;
}

BOOL CHTTPClient::RequestOfURI(CString szURI, int nMethod)
{
	BOOL bReturn=TRUE;

	TRY
	{
		switch(nMethod)
		{
		case	CHTTPClient::RequestGetMethod:
		default:
			bReturn = RequestGet(szURI);
			break;

		case	CHTTPClient::RequestPostMethod:		
			bReturn = RequestPost(szURI);
			break;

		case	CHTTPClient::RequestPostMethodMultiPartsFormData:
			bReturn = RequestPostMultiPartsFormData(szURI);
			break;
		case	CHTTPClient::RequestOptionsMethod:
			bReturn = RequestOptions(szURI);
			break;
		}
	}
	CATCH(CException, e)
	{
#ifdef	_DEBUG
		TRACE(_T("\nEXCEPTION\n"));
		TCHAR szERROR[1024];
		e->GetErrorMessage(szERROR, 1024);
		TRACE(szERROR);
#endif
	}
	END_CATCH

	return bReturn;
}

BOOL CHTTPClient::RequestGet(CString szURI){

	CONST TCHAR *szAcceptType[] = {__HTTP_ACCEPT_TYPE, NULL};

	m_hHTTPRequest = ::HttpOpenRequest(
							m_hHTTPConnection,
							__HTTP_VERB_GET,	// HTTP Verb
							(LPCTSTR)szURI,		// Object Name
							HTTP_VERSIONNEW/*HTTP_VERSION*/,		// Version
							_T(""),				// Reference
							szAcceptType,		// Accept Type
							(m_bUseSSL) ? m_nSecureFlags : (INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE),
							0);					// context call-back point

	if(!m_hHTTPRequest)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strHost.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strHost, m_strHost.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}
	if(m_strRefer.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strRefer, m_strRefer.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}
	if(m_strXRequestedWith.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strXRequestedWith, m_strXRequestedWith.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if (m_strDNT.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strDNT, m_strDNT.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);
#endif
		return FALSE;
	}

	// SEND REQUEST
	if(!::HttpSendRequest(m_hHTTPRequest, NULL,	0, NULL, 0))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPClient::RequestPost(CString szURI)
{
	TCHAR	*szAcceptType[] = {__HTTP_ACCEPT_TYPE, NULL};
	TCHAR	szPostArguments[BUFFER_SIZE];
	char	strPostArguments[BUFFER_SIZE];
	memset(szPostArguments, 0, sizeof(TCHAR)*BUFFER_SIZE);
	memset(strPostArguments, 0, sizeof(char)*BUFFER_SIZE);
	CString		szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	DWORD		dwPostArgumentsLegnth = GetPostArguments(szPostArguments, BUFFER_SIZE);
	Wc2KoreanMb(strPostArguments, szPostArguments, BUFFER_SIZE);
	if (!m_bUseSSL)
	{
		m_hHTTPRequest = ::HttpOpenRequest(
							m_hHTTPConnection,
							__HTTP_VERB_POST,	// HTTP Verb
							(LPCTSTR)szURI,		// Object Name
							HTTP_VERSIONNEW/*HTTP_VERSION*/,		// Version
							/*__HTTP_REFER*/NULL,				// Reference
							(LPCTSTR *)szAcceptType,		// Accept Type
							//INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
							INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT,
							0);					// context call-back point
	}
	else
	{
		m_hHTTPRequest = HttpOpenRequest(m_hHTTPConnection, 
			__HTTP_VERB_POST,
			(LPCTSTR)szURI,		// Object Name
			HTTP_VERSIONNEW/*HTTP_VERSION*/,
			/*__HTTP_REFER*/NULL,
			(LPCTSTR *)szAcceptType,		// Accept Type
			m_nSecureFlags, 
			0);
	}

	if(!m_hHTTPRequest)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders(m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}		

// 	if(m_strAcceptEncoding.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strAcceptEncoding, m_strAcceptEncoding.GetLength(), HTTP_ADDREQ_FLAG_ADD))
// 	{
// 		m_dwError = ::GetLastError();
// #ifdef	_DEBUG
// 		LPVOID     lpMsgBuffer;
// 		DWORD dwRet = FormatMessage(
// 			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
// 			NULL,
// 			::GetLastError(),
// 			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
// 			0,
// 			NULL);
// 		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
// 		LocalFree(lpMsgBuffer);		
// #endif
// 		return FALSE;
// 	}
// 
// 	if(m_strAcceptLanguage.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strAcceptLanguage, m_strAcceptLanguage.GetLength(), HTTP_ADDREQ_FLAG_ADD))
// 	{
// 		m_dwError = ::GetLastError();
// #ifdef	_DEBUG
// 		LPVOID     lpMsgBuffer;
// 		DWORD dwRet = FormatMessage(
// 			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
// 			NULL,
// 			::GetLastError(),
// 			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
// 			0,
// 			NULL);
// 		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
// 		LocalFree(lpMsgBuffer);		
// #endif
// 		return FALSE;
// 	}

	if(m_strHost.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strHost, m_strHost.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}
	
	if(m_strRefer.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strRefer, m_strRefer.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strContentType.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strContentType, m_strContentType.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strXRequestedWith.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strXRequestedWith, m_strXRequestedWith.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strPragma.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strPragma, m_strPragma.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strMethod.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strMethod, m_strMethod.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strOrigin.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strOrigin, m_strOrigin.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strTopic.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strTopic, m_strTopic.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strTransporttimeout.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strTransporttimeout, m_strTransporttimeout.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strType.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strType, m_strType.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strClientid.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strClientid, m_strClientid.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strS.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strS, m_strS.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strSubs.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strSubs, m_strSubs.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strUnsubs.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strUnsubs, m_strUnsubs.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_bSpecial)
	{
		m_bSpecial = FALSE;
// 		CString	strAcceptEncoding = _T("Accept-Encoding: gzip, deflate\r\n");
// 		CString strAcceptLanguage = _T("Accept-Language: en-US,en;q=0.5\r\n");
// 
// 		if(strAcceptEncoding.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, strAcceptEncoding, strAcceptEncoding.GetLength(), HTTP_ADDREQ_FLAG_ADD))
// 		{
// 			m_dwError = ::GetLastError();
// 			return FALSE;
// 		}
// 		if(strAcceptLanguage.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, strAcceptLanguage, strAcceptLanguage.GetLength(), HTTP_ADDREQ_FLAG_ADD))
// 		{
// 			m_dwError = ::GetLastError();
// 			return FALSE;
// 		}

		if(!::HttpSendRequest(m_hHTTPRequest,0, NULL, strPostArguments, (DWORD)strlen(strPostArguments)))
		{
			m_dwError = ::GetLastError();
			//#ifdef	_DEBUG
			LPVOID     lpMsgBuffer;
			DWORD dwRet = FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				::GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPTSTR>(&lpMsgBuffer),
				0,
				NULL);
			TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
			LocalFree(lpMsgBuffer);		
			//#endif
			return FALSE;
		}
		m_dwError = ::GetLastError();
		return TRUE;
	}
	// SEND REQUEST
	if(!::HttpSendRequest(m_hHTTPRequest, NULL, 0, strPostArguments, (DWORD)strlen(strPostArguments)))
	{
		m_dwError = ::GetLastError();
//#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
//#endif
		return FALSE;
	}

// 	LPVOID lpOutBuffer = NULL;
// 	DWORD	dwSize = 0;
// 	BOOL bRet = ::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_HOST   , (LPVOID)lpOutBuffer, &dwSize,NULL);
// 	lpOutBuffer = (LPVOID) new char[dwSize+100];
// 	bRet = ::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_HOST, (LPVOID)lpOutBuffer, &dwSize,NULL);

	return TRUE;
}

BOOL CHTTPClient::RequestPostMultiPartsFormData(LPCTSTR szURI)
{

	CONST TCHAR *szAcceptType[] = {__HTTP_ACCEPT_TYPE, NULL};	
	LPCTSTR		szContentType = TEXT("Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n");
	
	// ALLOCATE POST MULTI-PARTS FORM DATA ARGUMENTS
	PBYTE pPostBuffer = NULL;
	DWORD dwPostBufferLength = AllocMultiPartsFormData(pPostBuffer, _T("--MULTI-PARTS-FORM-DATA-BOUNDARY"));

	m_hHTTPRequest = ::HttpOpenRequest(
							m_hHTTPConnection,
							__HTTP_VERB_POST,	// HTTP Verb
							szURI,				// Object Name
							HTTP_VERSIONNEW/*HTTP_VERSION*/,		// Version
							_T(""),					// Reference
							szAcceptType,		// Accept Type
							INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,	// flags
							0);					// context call-back point

	if(!m_hHTTPRequest)
	{
		m_dwError= ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(!::HttpAddRequestHeaders( m_hHTTPRequest, szContentType, (DWORD)_tcslen(szContentType), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	TCHAR	szContentLength[BUFFER_SIZE] = _T("");
	::ZeroMemory(szContentLength, BUFFER_SIZE);

	_stprintf(szContentLength, _T("Content-Length: %d\r\n"), dwPostBufferLength);

	if(!::HttpAddRequestHeaders( m_hHTTPRequest, szContentLength, (DWORD)_tcslen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

#ifdef	_DEBUG
	TCHAR	*szHTTPRequest = m_pTempBuffer1;
	szHTTPRequest[0] = 0;
	DWORD	dwHTTPRequestLength = HTTP_BUFFER_SIZE;

	::ZeroMemory(szHTTPRequest, HTTP_BUFFER_SIZE);
	if(!::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHTTPRequest, &dwHTTPRequestLength, NULL))
	{
		m_dwError = ::GetLastError();
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(_T("\n%d : %s\n"), ::GetLastError(), reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
	}

	TRACE(szHTTPRequest);
#endif

	// SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
	INTERNET_BUFFERS InternetBufferIn = {0};
	InternetBufferIn.dwStructSize	= sizeof(INTERNET_BUFFERS);
	InternetBufferIn.Next			= NULL;	
	
	if(!::HttpSendRequestEx(m_hHTTPRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	DWORD dwOutPostBufferLength = 0;
	if(!::InternetWriteFile(m_hHTTPRequest, pPostBuffer, dwPostBufferLength, &dwOutPostBufferLength))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(!::HttpEndRequest(m_hHTTPRequest, NULL, HSR_INITIATE, 0))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// FREE POST MULTI-PARTS FORM DATA ARGUMENTS
	FreeMultiPartsFormData(pPostBuffer);

	return TRUE;
}

DWORD CHTTPClient::ResponseOfBytes(PBYTE pBuffer, DWORD dwSize)
{
	static DWORD dwBytes = 0;

	if(!m_hHTTPRequest)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return 0;
	}

	::ZeroMemory(pBuffer, dwSize);
	if(!::InternetReadFile(	m_hHTTPRequest,	pBuffer, dwSize, &dwBytes))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return 0;
	}

	return dwBytes;
}

WCHAR	g_pUniToAsc[0x300][2] = 
{
	{0x0000, 0},
	{0x0001, 0},
	{0x0002, 0},
	{0x0003, 0},
	{0x0004, 0},
	{0x0005, 0},
	{0x0006, 0},
	{0x0007, 0},
	{0x0008, 0},
	{0x0009, 0},
	{0x000A, 0},
	{0x000B, 0},
	{0x000C, 0},
	{0x000D, 0},
	{0x000E, 0},
	{0x000F, 0},
	{0x0010, 0},
	{0x0011, 0},
	{0x0012, 0},
	{0x0013, 0},
	{0x0014, 0},
	{0x0015, 0},
	{0x0016, 0},
	{0x0017, 0},
	{0x0018, 0},
	{0x0019, 0},
	{0x001A, 0},
	{0x001B, 0},
	{0x001C, 0},
	{0x001D, 0},
	{0x001E, 0},
	{0x001F, 0},
	{0x0020, 0},
	{0x0021, 0},
	{0x0022, 0},
	{0x0023, 0},
	{0x0024, 0},
	{0x0025, 0},
	{0x0026, 0},
	{0x0027, 0},
	{0x0028, 0},
	{0x0029, 0},
	{0x002A, 0},
	{0x002B, 0},
	{0x002C, 0},
	{0x002D, 0},
	{0x002E, 0},
	{0x002F, 0},
	{0x0030, 0},
	{0x0031, 0},
	{0x0032, 0},
	{0x0033, 0},
	{0x0034, 0},
	{0x0035, 0},
	{0x0036, 0},
	{0x0037, 0},
	{0x0038, 0},
	{0x0039, 0},
	{0x003A, 0},
	{0x003B, 0},
	{0x003C, 0},
	{0x003D, 0},
	{0x003E, 0},
	{0x003F, 0},
	{0x0040, 0},
	{0x0041, 0},
	{0x0042, 0},
	{0x0043, 0},
	{0x0044, 0},
	{0x0045, 0},
	{0x0046, 0},
	{0x0047, 0},
	{0x0048, 0},
	{0x0049, 0},
	{0x004A, 0},
	{0x004B, 0},
	{0x004C, 0},
	{0x004D, 0},
	{0x004E, 0},
	{0x004F, 0},
	{0x0050, 0},
	{0x0051, 0},
	{0x0052, 0},
	{0x0053, 0},
	{0x0054, 0},
	{0x0055, 0},
	{0x0056, 0},
	{0x0057, 0},
	{0x0058, 0},
	{0x0059, 0},
	{0x005A, 0},
	{0x005B, 0},
	{0x005C, 0},
	{0x005D, 0},
	{0x005E, 0},
	{0x005F, 0},
	{0x0060, 0},
	{0x0061, 0},
	{0x0062, 0},
	{0x0063, 0},
	{0x0064, 0},
	{0x0065, 0},
	{0x0066, 0},
	{0x0067, 0},
	{0x0068, 0},
	{0x0069, 0},
	{0x006A, 0},
	{0x006B, 0},
	{0x006C, 0},
	{0x006D, 0},
	{0x006E, 0},
	{0x006F, 0},
	{0x0070, 0},
	{0x0071, 0},
	{0x0072, 0},
	{0x0073, 0},
	{0x0074, 0},
	{0x0075, 0},
	{0x0076, 0},
	{0x0077, 0},
	{0x0078, 0},
	{0x0079, 0},
	{0x007A, 0},
	{0x007B, 0},
	{0x007C, 0},
	{0x007D, 0},
	{0x007E, 0},
	{0x007F, 0},
	{0x0080, 0},
	{0x0081, 0},
	{0x0082, 0},
	{0x0083, 0},
	{0x0084, 0},
	{0x0085, 0},
	{0x0086, 0},
	{0x0087, 0},
	{0x0088, 0},
	{0x0089, 0},
	{0x008A, 0},
	{0x008B, 0},
	{0x008C, 0},
	{0x008D, 0},
	{0x008E, 0},
	{0x008F, 0},
	{0x0090, 0},
	{0x0091, 0},
	{0x0092, 0},
	{0x0093, 0},
	{0x0094, 0},
	{0x0095, 0},
	{0x0096, 0},
	{0x0097, 0},
	{0x0098, 0},
	{0x0099, 0},
	{0x009A, 0},
	{0x009B, 0},
	{0x009C, 0},
	{0x009D, 0},
	{0x009E, 0},
	{0x009F, 0},
	{0x00A0, 0},
	{0x00A1, 0},
	{0x00A2, 0},
	{0x00A3, 0},
	{0x00A4, 0},
	{0x00A5, 0},
	{0x00A6, 0},
	{0x00A7, 0},
	{0x00A8, 0},
	{0x00A9, 0},
	{0x00AA, 0},
	{0x00AB, 0},
	{0x00AC, 0},
	{0x00AD, 0},
	{0x00AE, 0},
	{0x00AF, 0},
	{0x00B0, 0},
	{0x00B1, 0},
	{0x00B2, 0},
	{0x00B3, 0},
	{0x00B4, 0},
	{0x00B5, 0},
	{0x00B6, 0},
	{0x00B7, 0},
	{0x00B8, 0},
	{0x00B9, 0},
	{0x00BA, 0},
	{0x00BB, 0},
	{0x00BC, 0},
	{0x00BD, 0},
	{0x00BE, 0},
	{0x00BF, 0},
	{0x00C0, 0},
	{0x00C1, L'A'},
	{0x00C2, L'A'},
	{0x00C3, L'A'},
	{0x00C4, L'A'},
	{0x00C5, L'A'},
	{0x00C6, L'A'},
	{0x00C7, L'C'},
	{0x00C8, L'E'},
	{0x00C9, L'E'},
	{0x00CA, L'E'},
	{0x00CB, L'E'},
	{0x00CC, L'I'},
	{0x00CD, L'I'},
	{0x00CE, L'I'},
	{0x00CF, L'I'},
	{0x00D0, L'D'},
	{0x00D1, L'N'},
	{0x00D2, L'O'},
	{0x00D3, L'O'},
	{0x00D4, L'O'},
	{0x00D5, L'O'},
	{0x00D6, L'O'},
	{0x00D7, L'x'},
	{0x00D8, L'O'},
	{0x00D9, L'U'},
	{0x00DA, L'U'},
	{0x00DB, L'U'},
	{0x00DC, L'U'},
	{0x00DD, L'Y'},
	{0x00DE, L'P'},
	{0x00DF, L'B'},
	{0x00E0, L'a'},
	{0x00E1, L'a'},
	{0x00E2, L'a'},
	{0x00E3, L'a'},
	{0x00E4, L'a'},
	{0x00E5, L'a'},
	{0x00E6, L'a'},
	{0x00E7, L'c'},
	{0x00E8, L'e'},
	{0x00E9, L'e'},
	{0x00EA, L'e'},
	{0x00EB, L'e'},
	{0x00EC, L'i'},
	{0x00ED, L'i'},
	{0x00EE, L'i'},
	{0x00EF, L'i'},
	{0x00F0, L'd'},
	{0x00F1, L'n'},
	{0x00F2, L'o'},
	{0x00F3, L'o'},
	{0x00F4, L'o'},
	{0x00F5, L'o'},
	{0x00F6, L'o'},
	{0x00F7, 0},  //나누기기호
	{0x00F8, L'o'},
	{0x00F9, L'u'},
	{0x00FA, L'u'},
	{0x00FB, L'u'},
	{0x00FC, L'u'},
	{0x00FD, L'y'},
	{0x00FE, L'p'},
	{0x00FF, L'y'},
	{0x0100, L'A'},
	{0x0101, L'a'},
	{0x0102, L'A'},
	{0x0103, L'a'},
	{0x0104, L'A'},
	{0x0105, L'a'},
	{0x0106, L'C'},
	{0x0107, L'c'},
	{0x0108, L'C'},
	{0x0109, L'c'},
	{0x010A, L'C'},
	{0x010B, L'c'},
	{0x010C, L'C'},
	{0x010D, L'c'},
	{0x010E, L'D'},
	{0x010F, L'd'},
	{0x0110, L'D'},
	{0x0111, L'd'},
	{0x0112, L'E'},
	{0x0113, L'e'},
	{0x0114, L'E'},
	{0x0115, L'e'},
	{0x0116, L'E'},
	{0x0117, L'e'},
	{0x0118, L'E'},
	{0x0119, L'e'},
	{0x011A, L'E'},
	{0x011B, L'e'},
	{0x011C, L'G'},
	{0x011D, L'g'},
	{0x011E, L'G'},
	{0x011F, L'g'},
	{0x0120, L'G'},
	{0x0121, L'g'},
	{0x0122, L'G'},
	{0x0123, L'g'},
	{0x0124, L'H'},
	{0x0125, L'h'},
	{0x0126, L'H'},
	{0x0127, L'h'},
	{0x0128, L'I'},
	{0x0129, L'i'},
	{0x012A, L'I'},
	{0x012B, L'i'},
	{0x012C, L'I'},
	{0x012D, L'i'},
	{0x012E, L'I'},
	{0x012F, L'i'},
	{0x0130, L'I'},
	{0x0131, L'i'},
	{0x0132, L'I'},//	IJ
	{0x0133, L'i'},//	ij
	{0x0134, L'J'},
	{0x0135, L'j'},
	{0x0136, L'K'},
	{0x0137, L'k'},
	{0x0138, L'k'},
	{0x0139, L'L'},
	{0x013A, L'l'},
	{0x013B, L'L'},
	{0x013C, L'l'},
	{0x013D, L'L'},
	{0x013E, L'l'},
	{0x013F, L'L'},
	{0x0140, L'l'},
	{0x0141, L'L'},
	{0x0142, L'l'},
	{0x0143, L'N'},
	{0x0144, L'n'},
	{0x0145, L'N'},
	{0x0146, L'n'},
	{0x0147, L'N'},
	{0x0148, L'n'},
	{0x0149, L'n'},
	{0x014A, L'N'},
	{0x014B, L'n'},
	{0x014C, L'O'},
	{0x014D, L'o'},
	{0x014E, L'O'},
	{0x014F, L'o'},
	{0x0150, L'O'},
	{0x0151, L'o'},
	{0x0152, L'C'},	//CE
	{0x0153, L'c'},	//ce
	{0x0154, L'R'},
	{0x0155, L'r'},
	{0x0156, L'R'},
	{0x0157, L'r'},
	{0x0158, L'R'},
	{0x0159, L'r'},
	{0x015A, L'S'},
	{0x015B, L's'},
	{0x015C, L'S'},
	{0x015D, L's'},
	{0x015E, L'S'},
	{0x015F, L's'},
	{0x0160, L'S'},
	{0x0161, L's'},
	{0x0162, L'T'},
	{0x0163, L't'},
	{0x0164, L'T'},
	{0x0165, L't'},
	{0x0166, L'T'},
	{0x0167, L't'},
	{0x0168, L'U'},
	{0x0169, L'u'},
	{0x016A, L'U'},
	{0x016B, L'u'},
	{0x016C, L'U'},
	{0x016D, L'u'},
	{0x016E, L'U'},
	{0x016F, L'u'},
	{0x0170, L'U'},
	{0x0171, L'u'},
	{0x0172, L'U'},
	{0x0173, L'u'},
	{0x0174, L'W'},
	{0x0175, L'w'},
	{0x0176, L'Y'},
	{0x0177, L'y'},
	{0x0178, L'Y'},
	{0x0179, L'Z'},
	{0x017A, L'z'},
	{0x017B, L'Z'},
	{0x017C, L'z'},
	{0x017D, L'Z'},
	{0x017E, L'z'},
	{0x017F, L'l'},
	{0x0180, L'b'},
	{0x0181, L'B'},
	{0x0182, L'b'},
	{0x0183, L'b'},
	{0x0184, L'b'},
	{0x0185, L'b'},
	{0x0186, L'C'},
	{0x0187, L'C'},
	{0x0188, L'c'},
	{0x0189, L'D'},
	{0x018A, L'D'},
	{0x018B, L'b'},
	{0x018C, L'b'},
	{0x018D, L'o'},
	{0x018E, L'E'},
	{0x018F, L'e'},
	{0x0190, L'E'},
	{0x0191, L'F'},
	{0x0192, L'f'},
	{0x0193, L'G'},
	{0x0194, L'V'},
	{0x0195, L'h'},
	{0x0196, L'I'},
	{0x0197, L'l'},
	{0x0198, L'K'},
	{0x0199, L'k'},
	{0x019A, L'l'},
	{0x019B, 0},
	{0x019C, 0},
	{0x019D, L'N'},
	{0x019E, L'n'},
	{0x019F, L'O'},
	{0x01A0, L'O'},
	{0x01A1, L'O'},
	{0x01A2, 0},
	{0x01A3, 0},
	{0x01A4, L'P'},
	{0x01A5, L'p'},
	{0x01A6, L'R'},
	{0x01A7, 0},
	{0x01A8, 0},
	{0x01A9, 0},
	{0x01AA, 0},
	{0x01AB, L't'},
	{0x01AC, L'T'},
	{0x01AD, L't'},
	{0x01AE, L'T'},
	{0x01AF, L'U'},
	{0x01B0, L'u'},
	{0x01B1, L'O'},
	{0x01B2, L'U'},
	{0x01B3, L'Y'},
	{0x01B4, L'y'},
	{0x01B5, L'Z'},
	{0x01B6, L'z'},
	{0x01B7, 0},
	{0x01B8, 0},
	{0x01B9, 0},
	{0x01BA, 0},
	{0x01BB, 0},
	{0x01BC, 0},
	{0x01BD, 0},
	{0x01BE, 0},
	{0x01BF, 0},
	{0x01C0, L'I'},
	{0x01C1, L'I'},
	{0x01C2, 0},
	{0x01C3, 0},
	{0x01C4, L'D'},
	{0x01C5, L'D'},
	{0x01C6, L'd'},
	{0x01C7, L'L'},
	{0x01C8, L'L'},
	{0x01C9, L'l'},
	{0x01CA, L'N'},
	{0x01CB, L'N'},
	{0x01CC, L'n'},
	{0x01CD, L'A'},
	{0x01CE, L'a'},
	{0x01CF, L'I'},
	{0x01D0, L'i'},
	{0x01D1, L'O'},
	{0x01D2, L'o'},
	{0x01D3, L'U'},
	{0x01D4, L'u'},
	{0x01D5, L'U'},
	{0x01D6, L'u'},
	{0x01D7, L'U'},
	{0x01D8, L'u'},
	{0x01D9, L'U'},
	{0x01DA, L'u'},
	{0x01DB, L'U'},
	{0x01DC, L'u'},
	{0x01DD, 0},
	{0x01DE, L'A'},
	{0x01DF, L'a'},
	{0x01E0, L'A'},
	{0x01E1, L'a'},
	{0x01E2, L'A'},
	{0x01E3, L'a'},
	{0x01E4, L'G'},
	{0x01E5, L'g'},
	{0x01E6, L'G'},
	{0x01E7, L'g'},
	{0x01E8, L'K'},
	{0x01E9, L'k'},
	{0x01EA, L'O'},
	{0x01EB, L'o'},
	{0x01EC, L'O'},
	{0x01ED, L'o'},
	{0x01EE, 0},
	{0x01EF, 0},
	{0x01F0, L'j'},
	{0x01F1, L'D'},
	{0x01F2, L'D'},
	{0x01F3, L'd'},
	{0x01F4, L'G'},
	{0x01F5, L'g'},
	{0x01F6, L'H'},
	{0x01F7, L'P'},
	{0x01F8, L'N'},
	{0x01F9, L'n'},
	{0x01FA, L'A'},
	{0x01FB, L'a'},
	{0x01FC, L'A'},
	{0x01FD, L'a'},
	{0x01FE, L'O'},
	{0x01FF, L'o'},
	{0x0200, L'A'},
	{0x0201, L'a'},
	{0x0202, L'A'},
	{0x0203, L'a'},
	{0x0204, L'E'},
	{0x0205, L'e'},
	{0x0206, L'E'},
	{0x0207, L'e'},
	{0x0208, L'I'},
	{0x0209, L'i'},
	{0x020A, L'I'},
	{0x020B, L'i'},
	{0x020C, L'O'},
	{0x020D, L'o'},
	{0x020E, L'O'},
	{0x020F, L'o'},
	{0x0210, L'R'},
	{0x0211, L'r'},
	{0x0212, L'R'},
	{0x0213, L'r'},
	{0x0214, L'U'},
	{0x0215, L'u'},
	{0x0216, L'U'},
	{0x0217, L'u'},
	{0x0218, L'S'},
	{0x0219, L's'},
	{0x021A, L'T'},
	{0x021B, L't'},
	{0x021C, 0},
	{0x021D, 0},
	{0x021E, L'H'},
	{0x021F, L'h'},
	{0x0220, L'n'},
	{0x0221, 0},
	{0x0222, 0},
	{0x0223, 0},
	{0x0224, L'Z'},
	{0x0225, L'z'},
	{0x0226, L'A'},
	{0x0227, L'a'},
	{0x0228, L'E'},
	{0x0229, L'e'},
	{0x022A, L'O'},
	{0x022B, L'o'},
	{0x022C, L'O'},
	{0x022D, L'o'},
	{0x022E, L'O'},
	{0x022F, L'o'},
	{0x0230, L'O'},
	{0x0231, L'o'},
	{0x0232, L'Y'},
	{0x0233, L'y'},
	{0x0234, 0},
	{0x0235, 0},
	{0x0236, 0},
	{0x0237, 0},
	{0x0238, 0},
	{0x0239, 0},
	{0x023A, 0},
	{0x023B, 0},
	{0x023C, 0},
	{0x023D, 0},
	{0x023E, 0},
	{0x023F, 0},
	{0x0240, 0},
	{0x0241, 0},
	{0x0242, 0},
	{0x0243, 0},
	{0x0244, 0},
	{0x0245, 0},
	{0x0246, 0},
	{0x0247, 0},
	{0x0248, 0},
	{0x0249, 0},
	{0x024A, 0},
	{0x024B, 0},
	{0x024C, 0},
	{0x024D, 0},
	{0x024E, 0},
	{0x024F, 0},
	{0x0250, 0},
	{0x0251, 0},
	{0x0252, 0},
	{0x0253, 0},
	{0x0254, 0},
	{0x0255, 0},
	{0x0256, 0},
	{0x0257, 0},
	{0x0258, 0},
	{0x0259, 0},
	{0x025A, 0},
	{0x025B, 0},
	{0x025C, 0},
	{0x025D, 0},
	{0x025E, 0},
	{0x025F, 0},
	{0x0260, 0},
	{0x0261, 0},
	{0x0262, 0},
	{0x0263, 0},
	{0x0264, 0},
	{0x0265, 0},
	{0x0266, 0},
	{0x0267, 0},
	{0x0268, 0},
	{0x0269, 0},
	{0x026A, 0},
	{0x026B, 0},
	{0x026C, 0},
	{0x026D, 0},
	{0x026E, 0},
	{0x026F, 0},
	{0x0270, 0},
	{0x0271, 0},
	{0x0272, 0},
	{0x0273, 0},
	{0x0274, 0},
	{0x0275, 0},
	{0x0276, 0},
	{0x0277, 0},
	{0x0278, 0},
	{0x0279, 0},
	{0x027A, 0},
	{0x027B, 0},
	{0x027C, 0},
	{0x027D, 0},
	{0x027E, 0},
	{0x027F, 0},
	{0x0280, 0},
	{0x0281, 0},
	{0x0282, 0},
	{0x0283, 0},
	{0x0284, 0},
	{0x0285, 0},
	{0x0286, 0},
	{0x0287, 0},
	{0x0288, 0},
	{0x0289, 0},
	{0x028A, 0},
	{0x028B, 0},
	{0x028C, 0},
	{0x028D, 0},
	{0x028E, 0},
	{0x028F, 0},
	{0x0290, 0},
	{0x0291, 0},
	{0x0292, 0},
	{0x0293, 0},
	{0x0294, 0},
	{0x0295, 0},
	{0x0296, 0},
	{0x0297, 0},
	{0x0298, 0},
	{0x0299, 0},
	{0x029A, 0},
	{0x029B, 0},
	{0x029C, 0},
	{0x029D, 0},
	{0x029E, 0},
	{0x029F, 0},
	{0x02A0, 0},
	{0x02A1, 0},
	{0x02A2, 0},
	{0x02A3, 0},
	{0x02A4, 0},
	{0x02A5, 0},
	{0x02A6, 0},
	{0x02A7, 0},
	{0x02A8, 0},
	{0x02A9, 0},
	{0x02AA, 0},
	{0x02AB, 0},
	{0x02AC, 0},
	{0x02AD, 0},
	{0x02AE, 0},
	{0x02AF, 0},
	{0x02B0, 0},
	{0x02B1, 0},
	{0x02B2, 0},
	{0x02B3, 0},
	{0x02B4, 0},
	{0x02B5, 0},
	{0x02B6, 0},
	{0x02B7, 0},
	{0x02B8, 0},
	{0x02B9, 0},
	{0x02BA, 0},
	{0x02BB, 0},
	{0x02BC, 0},
	{0x02BD, 0},
	{0x02BE, 0},
	{0x02BF, 0},
	{0x02C0, 0},
	{0x02C1, 0},
	{0x02C2, 0},
	{0x02C3, 0},
	{0x02C4, 0},
	{0x02C5, 0},
	{0x02C6, 0},
	{0x02C7, 0},
	{0x02C8, 0},
	{0x02C9, 0},
	{0x02CA, 0},
	{0x02CB, 0},
	{0x02CC, 0},
	{0x02CD, 0},
	{0x02CE, 0},
	{0x02CF, 0},
	{0x02D0, 0},
	{0x02D1, 0},
	{0x02D2, 0},
	{0x02D3, 0},
	{0x02D4, 0},
	{0x02D5, 0},
	{0x02D6, 0},
	{0x02D7, 0},
	{0x02D8, 0},
	{0x02D9, 0},
	{0x02DA, 0},
	{0x02DB, 0},
	{0x02DC, 0},
	{0x02DD, 0},
	{0x02DE, 0},
	{0x02DF, 0},
	{0x02E0, 0},
	{0x02E1, 0},
	{0x02E2, 0},
	{0x02E3, 0},
	{0x02E4, 0},
	{0x02E5, 0},
	{0x02E6, 0},
	{0x02E7, 0},
	{0x02E8, 0},
	{0x02E9, 0},
	{0x02EA, 0},
	{0x02EB, 0},
	{0x02EC, 0},
	{0x02ED, 0},
	{0x02EE, 0},
	{0x02EF, 0},
	{0x02F0, 0},
	{0x02F1, 0},
	{0x02F2, 0},
	{0x02F3, 0},
	{0x02F4, 0},
	{0x02F5, 0},
	{0x02F6, 0},
	{0x02F7, 0},
	{0x02F8, 0},
	{0x02F9, 0},
	{0x02FA, 0},
	{0x02FB, 0},
	{0x02FC, 0},
	{0x02FD, 0},
	{0x02FE, 0},
	{0x02FF, 0},
};


BOOL CHTTPClient::Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize)
{
	BYTE	*pResponseBuffer = m_pTempBuffer2;
	pResponseBuffer[0] = 0;
	DWORD	dwNumOfBytesToRead = 0;
	int		i;

	if(!m_hHTTPRequest)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG		
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	::ZeroMemory(pBuffer, dwBufferLength);
	dwResultSize = 0;
	while((dwNumOfBytesToRead = ResponseOfBytes(pResponseBuffer+dwResultSize, BUFFER_SIZE))!=NULL && dwResultSize < dwBufferLength)
	{
		//::CopyMemory( (pBuffer + dwResultSize), uniStr, dwNumOfBytesToRead);
		dwResultSize += dwNumOfBytesToRead;
	}

// 	for(i = 0; i < dwResultSize; i++)
// 	{
// 		if(pResponseBuffer[i] >= 0x80)
// 		{
// 			for(j = 0; j < AsciiCodeNum; j++)
// 			{
// 				if(pResponseBuffer[i] == AscillConvert[j][0])
// 					pResponseBuffer[i] = AscillConvert[j][1];
// 			}
// 		}
// 		
// 	}

//	KoreanMb2Wc((TCHAR*)pBuffer, (const char*)&(pResponseBuffer[0]), dwBufferLength);

	int nLen = MultiByteToWideChar(CP_UTF8, 0, (char*)pResponseBuffer, (int)strlen((char*)pResponseBuffer), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, (char*)pResponseBuffer, (int)strlen((char*)pResponseBuffer), (TCHAR*)pBuffer, nLen);

	WCHAR	*pszBuffer = (WCHAR *)pBuffer;
	for(i = 0; i < nLen; i++)
	{
		if(pszBuffer[i] < 0x80)
			continue;
		if(pszBuffer[i] >= 0x300)
			continue;
		if(!g_pUniToAsc[pszBuffer[i]][1])
			continue;
		pszBuffer[i] = g_pUniToAsc[pszBuffer[i]][1];
	}

	::ZeroMemory(pHeaderBuffer, dwHeaderBufferLength);
	if(!::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, pHeaderBuffer, &dwHeaderBufferLength, NULL))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							reinterpret_cast<LPTSTR>(&lpMsgBuffer),
							0,
							NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_bNoNeedResponse)
		return TRUE;

	return (dwResultSize? TRUE : FALSE);
}

VOID CHTTPClient::AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary)
{
	HTTPArgument	arg;
	::ZeroMemory(&arg, sizeof(arg));

	_tcscpy(arg.szName, szName);
	_tcscpy(arg.szValue, szValue);

	if(!bBinary)
		arg.dwType = CHTTPClient::TypeNormal;
	else
		arg.dwType = CHTTPClient::TypeBinary;	

	m_vArguments.push_back(arg);

	return;
}

VOID CHTTPClient::AddPostArguments(LPCTSTR szName, DWORD nValue)
{
	HTTPArgument	arg;
	::FillMemory(&arg, sizeof(arg), 0x00);

	_tcscpy(arg.szName, szName);
	_stprintf(arg.szValue, _T("%d"), nValue);
	arg.dwType = CHTTPClient::TypeNormal;

	m_vArguments.push_back(arg);

	return;
}

DWORD CHTTPClient::GetPostArguments(LPTSTR szArguments, DWORD dwLength)
{
	std::vector<HTTPArgument>::iterator itArg;

	::ZeroMemory(szArguments, dwLength);
	for(itArg = m_vArguments.begin(); itArg < m_vArguments.end(); )
	{
		_tcscat(szArguments, itArg->szName);
		_tcscat(szArguments, _T("="));
		_tcscat(szArguments, itArg->szValue);

		if((++itArg) < m_vArguments.end())
		{
			_tcscat(szArguments, _T("&"));
		}
	}
	
	return (DWORD)_tcslen(szArguments);
}

VOID CHTTPClient::InitPostArguments()
{
	m_vArguments.clear();
	return;
}

VOID CHTTPClient::FreeMultiPartsFormData(PBYTE &pBuffer)
{
	if(pBuffer)
	{
		::LocalFree(pBuffer);
		pBuffer = NULL;
	}

	return;
}

DWORD CHTTPClient::AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary)
{
	if(pInBuffer)
	{
		::LocalFree(pInBuffer);
		pInBuffer = NULL;
	}

	pInBuffer = (PBYTE)::LocalAlloc(LPTR, GetMultiPartsFormDataLength());
	std::vector<HTTPArgument>::iterator itArgv;

	DWORD dwPosition	= 0;
	DWORD dwBufferSize	= 0;

	for(itArgv = m_vArguments.begin(); itArgv < m_vArguments.end(); ++itArgv)
	{
		PBYTE pBuffer = NULL;

		// SET MULTI_PRATS FORM DATA BUFFER
		switch(itArgv->dwType)
		{
		case	CHTTPClient::TypeNormal:
			pBuffer = (PBYTE)::LocalAlloc(LPTR, HTTP_HEAD_LINE_SIZE * 4);
			_stprintf(	(TCHAR*)pBuffer,							
						_T("--%s\r\n")
						_T("Content-Disposition: form-data; name=\"%s\"\r\n")
						_T("\r\n")
						_T("%s\r\n"),
						szBoundary,
						itArgv->szName,
						itArgv->szValue);

			dwBufferSize = (DWORD)_tcslen((TCHAR*)pBuffer);
			break;

		case	CHTTPClient::TypeBinary:
			DWORD	dwNumOfBytesToRead	= 0;
			DWORD	dwTotalBytes		= 0;

			HANDLE hFile = ::CreateFile(
								itArgv->szValue, 
								GENERIC_READ,		// desired access
								FILE_SHARE_READ,	// share mode
								NULL,				// security attribute
								OPEN_EXISTING,		// create disposition
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
								NULL);				// template file handle

			DWORD	dwSize = ::GetFileSize(hFile, NULL);

			pBuffer = (PBYTE)::LocalAlloc(LPTR, HTTP_HEAD_LINE_SIZE * 3 + dwSize + 1);
			BYTE	pBytes[BUFFER_SIZE + 1] = "";
			
			_stprintf( (TCHAR*)pBuffer,
							_T("--%s\r\n")
							_T("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n")
							_T("Content-Type: %s\r\n")
							_T("\r\n"),
							szBoundary,
							itArgv->szName, itArgv->szValue,
							GetContentType(itArgv->szValue)
							);

			DWORD	dwBufPosition = (DWORD)_tcslen((TCHAR*)pBuffer);	
	
			while(::ReadFile(hFile, pBytes, BUFFER_SIZE, &dwNumOfBytesToRead, NULL) && dwNumOfBytesToRead > 0 && dwTotalBytes <= dwSize)
			{
				::CopyMemory((pBuffer + dwBufPosition + dwTotalBytes), pBytes, dwNumOfBytesToRead);
				::ZeroMemory(pBytes, BUFFER_SIZE + 1);
				dwTotalBytes += dwNumOfBytesToRead;				
			}

			::CopyMemory((pBuffer + dwBufPosition + dwTotalBytes), _T("\r\n"), _tcslen(_T("\r\n")));
			::CloseHandle(hFile);
			dwBufferSize = dwBufPosition + dwTotalBytes + (DWORD)_tcslen(_T("\r\n"));
			break;			
		}

		::CopyMemory((pInBuffer + dwPosition), pBuffer, dwBufferSize);
		dwPosition += dwBufferSize;

		if(pBuffer)
		{
			::LocalFree(pBuffer);
			pBuffer = NULL;
		}
	}

	::CopyMemory((pInBuffer + dwPosition), "--", 2);
	::CopyMemory((pInBuffer + dwPosition + 2), szBoundary, _tcslen(szBoundary));
	::CopyMemory((pInBuffer + dwPosition + 2 + _tcslen(szBoundary)), "--\r\n", 3);

	return dwPosition + 5 + (DWORD)_tcslen(szBoundary);
}

DWORD CHTTPClient::GetMultiPartsFormDataLength()
{
	std::vector<HTTPArgument>::iterator itArgv;
	DWORD	dwLength = 0;

	for(itArgv = m_vArguments.begin(); itArgv < m_vArguments.end(); ++itArgv)
	{
		switch(itArgv->dwType)
		{
		case	CHTTPClient::TypeNormal:
			dwLength += HTTP_HEAD_LINE_SIZE * 4;
			break;

		case	CHTTPClient::TypeBinary:
			HANDLE hFile = ::CreateFile(itArgv->szValue, 
									GENERIC_READ,		// desired access
									FILE_SHARE_READ,	// share mode
									NULL,				// security attribute
									OPEN_EXISTING,		// create disposition
									FILE_ATTRIBUTE_NORMAL, // flag and attributes
									NULL);				// template file handle

			dwLength += HTTP_HEAD_LINE_SIZE * 3 + ::GetFileSize(hFile, NULL);
			::CloseHandle(hFile);			
			break;
		}
	}

	return dwLength;
}

LPCTSTR CHTTPClient::GetContentType(LPCTSTR szName)
{
	static TCHAR	szReturn[1024] = _T("");
	LONG	dwLen = 1024;
	DWORD	dwDot = 0;

	for(dwDot = (DWORD)_tcslen(szName); dwDot > 0; dwDot--)
	{
		if(!_tcsncmp((szName + dwDot), _T("."), 1))
			break;
	}

	HKEY	hKEY;
	LPTSTR	szWord = (LPTSTR)(szName + dwDot);

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, szWord, 0, KEY_QUERY_VALUE, &hKEY) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKEY, TEXT("Content Type"), NULL, NULL, (LPBYTE)szReturn, (unsigned long*)&dwLen) != ERROR_SUCCESS)
			_tcsncpy(szReturn, _T("application/octet-stream"), _tcslen(_T("application/octet-stream")));
		RegCloseKey(hKEY);
	}
	else
	{
		_tcsncpy(szReturn, _T("application/octet-stream"), _tcslen(_T("application/octet-stream")));
	}

	return szReturn;
}

DWORD CHTTPClient::GetLastError()
{
	return m_dwError;
}

VOID CHTTPClient::ParseURL(CString szURL, CString &szProtocol, CString &szAddress, DWORD &dwPort, CString &szURI)
{
	int		dwPos = 0;
	int		dwPos1 = 0;

	szURL.TrimLeft(); szURL.TrimRight();

	dwPos = szURL.Find(_T(":"), 0);
	if(dwPos == -1)
	{
		return;
	}
	else
	{
		szProtocol = szURL.Left(dwPos);
	}

	CString strTmp;
	strTmp = szURL.Mid(dwPos + 3);
	dwPos1 = strTmp.Find(_T("/"), 0);
	if(dwPos1 == -1)
	{
		return;
	}
	else
	{
		szAddress = strTmp.Left(dwPos1);
	}

	szURI = strTmp.Mid(dwPos1 + 1);


	if (m_bUseSSL)
		dwPort = INTERNET_DEFAULT_HTTPS_PORT;
	else
		dwPort = 9001;

	dwPort = 9001;
	return;
}


LPCTSTR CHTTPClient::QueryHTTPResponseHeader()
{
	return m_szResponseHeader;
}

LPCTSTR CHTTPClient::QueryHTTPResponse()
{
	return m_szResponseBody;
}

BOOL CHTTPClient::RequestPostWithSSL(HINTERNET hRequest, CString szURI)
{
	TCHAR	*szAcceptType[] = {__HTTP_ACCEPT_TYPE, NULL};
	TCHAR	szPostArguments[BUFFER_SIZE];
	char	strPostArguments[BUFFER_SIZE];

	memset(szPostArguments, 0, sizeof(TCHAR)*BUFFER_SIZE);
	memset(strPostArguments, 0, sizeof(char)*BUFFER_SIZE);
	CString		szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");
	
	DWORD		dwPostArgumentsLegnth = GetPostArguments(szPostArguments, BUFFER_SIZE);
	Wc2KoreanMb(strPostArguments, szPostArguments, BUFFER_SIZE);

//	if(!::HttpAddRequestHeaders(hRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
//	{
//		m_dwError = ::GetLastError();
//#ifdef	_DEBUG
//		LPVOID     lpMsgBuffer;
//		DWORD dwRet = FormatMessage(
//			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//			NULL,
//			::GetLastError(),
//			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
//			0,
//			NULL);
//		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
//		LocalFree(lpMsgBuffer);		
//#endif
//		return FALSE;
//	}

	// SEND REQUEST
	DWORD len = (DWORD)strlen(strPostArguments);
	if(!::HttpSendRequest(hRequest, szContentType, (DWORD)szContentType.GetLength(),
		strPostArguments, (DWORD)strlen(strPostArguments)))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	return TRUE;
}

#define		__HTTP_ACRMETHOD		_T("Access-Control-Request-Method: POST\r\n")

BOOL CHTTPClient::RequestOptions(CString szURI)
{
	TCHAR	*szAcceptType[] = {__HTTP_ACCEPT_TYPE, NULL};
	TCHAR	szPostArguments[BUFFER_SIZE];
	char	strPostArguments[BUFFER_SIZE];
	memset(szPostArguments, 0, sizeof(TCHAR)*BUFFER_SIZE);
	memset(strPostArguments, 0, sizeof(char)*BUFFER_SIZE);
	CString		szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	DWORD		dwPostArgumentsLegnth = GetPostArguments(szPostArguments, BUFFER_SIZE);
	Wc2KoreanMb(strPostArguments, szPostArguments, BUFFER_SIZE);
	if (!m_bUseSSL)
	{
		m_hHTTPRequest = ::HttpOpenRequest(
			m_hHTTPConnection,
			L"OPTIONS",	// HTTP Verb
			(LPCTSTR)szURI,		// Object Name
			HTTP_VERSIONNEW/*HTTP_VERSION*/,		// Version
			/*__HTTP_REFER*/NULL,				// Reference
			(LPCTSTR *)szAcceptType,		// Accept Type
			//INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
			INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT,
			0);					// context call-back point
	}
	else
	{
		m_hHTTPRequest = HttpOpenRequest(m_hHTTPConnection, 
			L"OPTIONS",
			(LPCTSTR)szURI,		// Object Name
			HTTP_VERSIONNEW/*HTTP_VERSION*/,
			/*__HTTP_REFER*/NULL,
			(LPCTSTR *)szAcceptType,		// Accept Type
			m_nSecureFlags, 
			0);
	}

	if(!m_hHTTPRequest)
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders(m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}		

	
	if(m_strHost.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strHost, m_strHost.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strPragma.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strPragma, m_strPragma.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(m_strOrigin.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strOrigin, m_strOrigin.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(!::HttpAddRequestHeaders(m_hHTTPRequest, __HTTP_ACRMETHOD, (DWORD)_tcslen(__HTTP_ACRMETHOD), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}	

	if(m_strACRHeaders.GetLength() && !::HttpAddRequestHeaders(m_hHTTPRequest, m_strACRHeaders, m_strACRHeaders.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		m_dwError = ::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}
	
	// SEND REQUEST
	if(!::HttpSendRequest(m_hHTTPRequest, NULL, 0, NULL, 0))
	{
		m_dwError = ::GetLastError();
		//#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&lpMsgBuffer),
			0,
			NULL);
		TRACE(_T("\n%d : %s\n"), m_dwError, reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
		//#endif
		return FALSE;
	}

	// 	LPVOID lpOutBuffer = NULL;
	// 	DWORD	dwSize = 0;
	// 	BOOL bRet = ::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_HOST   , (LPVOID)lpOutBuffer, &dwSize,NULL);
	// 	lpOutBuffer = (LPVOID) new char[dwSize+100];
	// 	bRet = ::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_HOST, (LPVOID)lpOutBuffer, &dwSize,NULL);

	return TRUE;
}