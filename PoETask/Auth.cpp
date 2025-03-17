#include "stdafx.h"
#include <vector>
#include <string>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>
#include "Auth.h"
#include "DeviceInfo.h"
#include "json/json.h"
#ifdef _PROTECT
#include "ThemidaSDK/Include/C/ThemidaSDK.h"
#endif

using namespace std;

std::string rot13(std::string input) {

	for (std::string::size_type len = input.length(), idx = 0; idx != len; ++idx) {
		if (input[idx] >= 'a' && input[idx] <= 'm') {
			input[idx] = input[idx] + 13;
		}
		else if (input[idx] >= 'n' && input[idx] <= 'z') {
			input[idx] = input[idx] - 13;
		}
		else if (input[idx] >= 'A' && input[idx] <= 'M') {
			input[idx] = input[idx] + 13;
		}
		else if (input[idx] >= 'N' && input[idx] <= 'Z') {
			input[idx] = input[idx] - 13;
		}
	}

	return input;
}

bool xorstring(string input, char *szTemp, string strKey)
{
	//string strKey = "fpt";
	int k = 0;
	for (int i = 0; i < input.length();)
	{
		for (int j = 0; j < strKey.length() && i < input.length(); i++, j++)
		{
			szTemp[i] = input[i] ^ strKey[j];
		}
	}
	szTemp[input.length()] = 0;
	return true;
}

std::string decode64(const std::string &val) {
	using namespace boost::archive::iterators;
	using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
	return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
		return c == '\0';
	});
}

std::string encode64(const std::string &val) {
	using namespace boost::archive::iterators;
	using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
	auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
	return tmp.append((3 - val.size() % 3) % 3, '=');
}

std::string packparams(string strInput)
{
	char aszTemp[1024];
	string strKey = textonce("!@#");
	auto strout = rot13(strInput);
	xorstring(strout, aszTemp, strKey);
	strout = encode64(aszTemp);

	//auto str1 = decode64(strout);
	//xorstring(str1, aszTemp, strKey);
	//auto str2 = rot13(aszTemp);

	return strout;
}

//int RSADecrypt(Num c1, Num c2, int d, Num n)
//{
//	Num cs[] = { c1, c2 };
//	string strRet;
//
//	for (int i = 0; i < 2; i++)
//	{
//		Num power = cs[i].pow(d);
//		//vector<char> a1, a2;
//		//power.print(a1);
//		Num code = power % n;
//		//code.print(a2);
//		while (code != 0)
//		{
//			Num ascii = code % 256;
//			code = code / 256;
//			strRet += (char)ascii.words[0];
//		}
//	}
//	return atoi(strRet.c_str());
//}
//
//bool TestRSADecrypt(string str)
//{
//	string str1, str2;
//	int nLen1 = 0, nLen2 = 0;
//	nLen1 = atoi(str.substr(0, 2).c_str());
//	str1 = str.substr(2, nLen1);
//	nLen2 = atoi(str.substr(2 + nLen1, 2).c_str());
//	str2 = str.substr(4 + nLen1, nLen2);
//	//str1 = "5326934944805654776";
//	//str2 = "1457463828326616825";
//	int a = RSADecrypt(Num(str1.c_str()), Num(str2.c_str()), 5, Num("9252903959724247943", 10));
//	if (a > 0 && a < 10000000)
//		return true;
//	return false;
//}

CAuth::CAuth()
{
	GetDeviceKey(m_szDeviceKey);
	m_pHttpClientGeneral = new CHTTPClient;
	m_bMultiCount = 0;
	m_login = 0;
	//TestRSADecrypt("19855695682732648958818586767338596749336");//19855695682732648958818586767338596749336
}

CAuth::~CAuth()
{
}


BOOL	CAuth::CheckVersion(const WCHAR *szOrgVersion, WCHAR *szNewVersion)
{
	WCHAR		szTempByURL[MAX_PATH];
	WCHAR		*szResponse;
	wstring		strServerIP, strServerName, strTemp;

	szNewVersion[0] = 0;

	CHTTPClient* pHttpClient = m_pHttpClientGeneral;
	swprintf(szTempByURL, L"%s:%d", AUTH_SERVER_IP, AUTH_SERVER_PORT);
	pHttpClient->SetHost(szTempByURL);
	pHttpClient->SetDNT(L"1");
	pHttpClient->SetXRequestedWith(L"10531-10010");

	strTemp = textwonce(L"http://%s/%s/api/v1/method/account.version.php?version=version");
	strServerIP = AUTH_SERVER_IP;
	strServerName = AUTH_SERVER_NAME;
	swprintf(szTempByURL, strTemp.c_str(), strServerIP.c_str(), strServerName.c_str());
	if (!pHttpClient->Request(szTempByURL, AUTH_SERVER_PORT, FALSE, CHTTPClient::RequestGetMethod))
	{
		return FALSE;
	}
	szResponse = (WCHAR *)pHttpClient->QueryHTTPResponse();
	if (!szResponse)
	{
		return FALSE;
	}

	int nLen = (int)wcslen(szResponse);
	char *pResBuf = new char[(int)((SIZE_T)nLen + 1)];
	wcstombs(pResBuf, szResponse, nLen);
	pResBuf[nLen] = 0;

	Json::Value root;
	Json::Reader reader;
	std::string config_doc = pResBuf;
	bool parsingSuccessful = reader.parse(config_doc, root);

	if (parsingSuccessful)
	{
		if (root[textonce("error")] == Success)
		{
			mbstowcs(szNewVersion, root[textonce("version")].asString().c_str(), MAX_PATH);
			if (wcscmp(szNewVersion, szOrgVersion))
				return TRUE;
		}
	}

	return FALSE;
}
