// HTTPClient.h : Interface of the CHTTPClient class
//


#pragma once


#include <afxwin.h>
#include <tchar.h>
#include <wininet.h>

#include <vector>

#define HTTP_VERSIONNEW            L"HTTP/1.1"

#define __DEFAULT_AGENT_NAME2	_T("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)")//_T("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)")
#define __DEFAULT_AGENT_NAME	_T("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:17.0) Gecko/17.0 Firefox/17.0")

#define	HTTP_ARGUMENT_NAME_SIZE		256
#define HTTP_ARGUMENT_VALUE_SIZE	1024

#define __HTTP_VERB_GET			_T("GET")
#define __HTTP_VERB_POST		_T("POST")
#define __HTTP_ACCEPT_TYPE		_T("*/*")
//#define __HTTP_ACCEPT			_T("Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*\r\n")
#define __HTTP_ACCEPT			_T("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n")											
#define _HTTP_ACCEPT_ENCODING	_T("Accept-Encoding: g_zip, deflate\r\n")
#define _HTTP_ACCEPT_LANGUAGE	_T("Accept-Language: en-US,en;q=0.5\r\n")

#define HTTP_BUFFER_SIZE			(40000+BUFFER_SIZE)
#define HTTP_RESPONSE_BUFFER_SIZE	(40000+BUFFER_SIZE)
#define HTTP_HEAD_LINE_SIZE			2048

#define BUFFER_SIZE			4096
#define SMALL_BUFFER_SIZE	256


class CHTTPClient {
public:
	// ARGUMENTS STRUCTURE
	typedef struct HTTP_ARGUMENT
	{

		TCHAR	szName[HTTP_ARGUMENT_NAME_SIZE];
		TCHAR	szValue[HTTP_ARGUMENT_VALUE_SIZE];
		DWORD	dwType;

		BOOL operator==(const HTTP_ARGUMENT &argV)
		{
			return !_tcscmp(szName, argV.szName) && !_tcscmp(szValue, argV.szValue);
		}
	} HTTPArgument;

	// REQUEST METHOD
	enum RequestMethod
	{
		RequestUnknown		= 0,
		RequestGetMethod	= 1,
		RequestPostMethod	= 2,
		RequestPostMethodMultiPartsFormData	= 3,
		RequestOptionsMethod	= 4
	};

	// POST TYPE 
	enum TypePostArgument
	{
		TypeUnknown	= 0,
		TypeNormal	= 1,
		TypeBinary	= 2
	};

	CHTTPClient();
	virtual ~CHTTPClient();

	static CHTTPClient::RequestMethod		GetMethod(int nMethod);
	static CHTTPClient::TypePostArgument	GetPostArgumentType(int nType);

	// Connection handler	
	BOOL Connect(CString szAddress, LPCTSTR szAgent = __DEFAULT_AGENT_NAME, 
				 unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, 
				 LPCTSTR szUserAccount = NULL, LPCTSTR szPassword = NULL);
	BOOL Close();
	VOID InitPostArguments();

	// HTTP Arguments handler	
	VOID AddPostArguments(LPCTSTR szName, DWORD nValue);
	VOID AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary = FALSE);

	// HTTP Method handler 
	BOOL	Request(CString szURL, DWORD serverPort, BOOL bUseSSL, int nMethod = CHTTPClient::RequestGetMethod, LPCTSTR szAgent = __DEFAULT_AGENT_NAME);
	BOOL	RequestOfURI(CString szURI, int nMethod = CHTTPClient::RequestGetMethod);
	BOOL	Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize);	
	LPCTSTR QueryHTTPResponse();
	LPCTSTR QueryHTTPResponseHeader();	

	// General Handler
	DWORD	GetLastError();
	LPCTSTR GetContentType(LPCTSTR szName);
	VOID	ParseURL(CString szURL, 
					 CString &szProtocol, 
					 CString &szAddress, 
					 DWORD &dwPort, 
					 CString &szURI);
	BOOL	RequestPostWithSSL(HINTERNET hRequest, CString szURI);
	TCHAR		*m_szResponseBody;		// RECEIVE HTTP BODY
	TCHAR		*m_szResponseHeader;	// RECEIVE HTTP HEADR
	TCHAR		*m_pTempBuffer1;
	BYTE		*m_pTempBuffer2;

	TCHAR		m_szLoginCookie[HTTP_BUFFER_SIZE];
	HINTERNET	m_hHTTPOpen;				// internet open handle
	HINTERNET	m_hHTTPConnection;			// internet connection hadle
	HINTERNET	m_hHTTPRequest;				// internet request hadle

protected:
	// HTTP Method handler
	DWORD	ResponseOfBytes(PBYTE pBuffer, DWORD dwSize);
	DWORD	GetPostArguments(LPTSTR szArguments, DWORD dwLength);

	BOOL	RequestPost(CString szURI);
	BOOL	RequestPostMultiPartsFormData(LPCTSTR szURI);
	BOOL	RequestGet(CString szURI);

	DWORD	AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary = _T("--MULTI-PARTS-FORM-DATA-BOUNDARY-"));
	VOID	FreeMultiPartsFormData(PBYTE &pBuffer);
	DWORD	GetMultiPartsFormDataLength();
public:
	void	SetHost(const TCHAR *szHost);
	void	SetRefer(const TCHAR *szRefer);
	void	SetContentType(const TCHAR *szContentType);
	void	SetXRequestedWith(const TCHAR *szXRequestedWith);
	void	SetPragma(const TCHAR *szPragma);
	void	SetAgent(int nAgent);

	void	SetMethod(const TCHAR *szMethod);
	void	SetOrigin(const TCHAR *szOrigin);
	void	SetTopic(const TCHAR *szTopic);
	void	SetTransporttimeout(const TCHAR *szTransporttimeout);
	void	SetType(const TCHAR *szType);
	void	SetClientid(const TCHAR *szClientid);
	void	SetS(const TCHAR *szS);
	void	SetACRHeaders(const TCHAR *szACRHeaders);
	void	SetAcceptLanguage(const TCHAR *szAcceptLanguage);
	void	SetAcceptEncoding(const TCHAR *szAcceptEncoding);
	void	SetSubs(const TCHAR *szSubs);
	void	SetUnsubs(const TCHAR *szUnsubs);
	void	SetDNT(const TCHAR *szDNT);
	void	SetHeaderClear();

	BOOL	RequestOptions(CString szURI);

//protected:
	std::vector<HTTPArgument> m_vArguments;				// POST ARGUMENTS VECTOR



	DWORD		m_dwError;					// LAST ERROR CODE
	LPCTSTR		m_szHost;					// HOST NAME
	DWORD		m_dwPort;					// PORT
	int			m_nSecureFlags;
	BOOL		m_bUseSSL;

	CString		m_strHost;
	CString		m_strRefer;	
	CString		m_strContentType;
	CString		m_strXRequestedWith;
	CString		m_strPragma;
	
	int			m_nAgent;

	CString		m_strMethod;
	CString		m_strOrigin;
	CString		m_strTopic;
	CString		m_strTransporttimeout;
	CString		m_strType;
	CString		m_strClientid;
	CString		m_strS;
	CString		m_strACRHeaders;
	CString		m_strAcceptLanguage;
	CString		m_strAcceptEncoding;
	CString		m_strSubs;
	CString		m_strUnsubs;
	CString		m_strDNT;
public:
	BOOL		m_bNoNeedResponse;
	BOOL		m_bSpecial;
};