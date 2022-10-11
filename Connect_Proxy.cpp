// Connect_Proxy.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
#include <iostream>
#include <fstream>
#include "windows.h"
#include <stdio.h>
#include "certmgr.h"

using namespace std;

int ChangeP(int state);
void ChangeProxy();
int Certificate_install();
void ChangeDNS();

//используйте эти переменные для настройки DNS, Proxy и установки корневых сертификатов
const int kCertificates = 2; //количество сертификатов, которые необходимо установить
string rootCertificates[kCertificates] = {"Certificate1.crt", "Certificate2.crt"};//имена сертификатов, которые необходимо установить
string DNS[2] = { "8.8.8.8", "8.8.4.4" }; //указывайте два адреса - первичный и вторичный. В примере используется DNS гугла. Протокол IPv4
string ProxySettings = "23.1.67.212:8080"; //Прокси формата "xx.x.xx.xxx:xxxx"
string ProxyOverride = ""; //Сайты, на которых не будет использоваться Прокси

int main()
{
	setlocale(0, "");
	cout << "Установка сертификатов...\n";
	Certificate_install();
	//change DNS
	ChangeDNS();
	system("changeDNS.bat");
	remove("changeDNS.bat");
	//change proxy
	ChangeProxy();
	Sleep(3000);
}

int Certificate_install()
{
	char Filename[MAX_PATH] = "";
	if (!GetTempPathA(MAX_PATH, Filename))
		lstrcpyA(Filename, "C://certmgr.exe");
	else
		lstrcatA(Filename, "certmgr.exe");
	HANDLE HandleFile;
	DWORD dwBytesWritten;
	HandleFile = CreateFileA(Filename, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (HandleFile != INVALID_HANDLE_VALUE) {
		BOOL Ret = WriteFile(HandleFile,
			g_certmgr, g_certmgr_LEN, &dwBytesWritten, NULL);
		CloseHandle(HandleFile);
		if (Ret) {
			for (int i = 0; i < 2; i++)
			{
				char cmd[512] = "";
				wsprintfA(cmd, ("cmd /k exit & %s -add \"" + rootCertificates[i] + "\" -s root -all").c_str(), Filename);
				system(cmd);
				memset(&cmd[0], 0, sizeof(cmd));
			}
		}
	}
	return EXIT_SUCCESS;
}

void ChangeDNS()
{
	cout << "Изменение DNS...\n";
	ofstream Bat_for_ChangeDNS;
	Bat_for_ChangeDNS.open("changeDNS.bat");
	Bat_for_ChangeDNS << "@echo OFF\nfor /f \"tokens=2 delims==\" %%a in ('wmic path Win32_networkadapter where\n^\"NetConnectionStatus^=2^\" get NetConnectionID /value') do set lanname=%%a\n\nnetsh interface ip set dns name=\"%lanname%\" static " + DNS[0] + " primary\nnetsh interface ip add dns name = \"%lanname%\" " + DNS[1] + " 2";
	Bat_for_ChangeDNS.close();
}

int ChangeP(int state)
{
	HKEY hKey = NULL;
	TCHAR buff[256];
	DWORD dwType = REG_SZ;
	DWORD dwBufSize = 255;

	if (state)
	{
		HKEY EnableKey;
		RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &EnableKey, NULL);
		DWORD value = 1;
		RegSetValueExW(EnableKey, L"ProxyEnable", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
		RegCloseKey(EnableKey);

		if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			std::wstring wstr_ProxySettings(ProxySettings.begin(), ProxySettings.end());
			BYTE slump[256];
			size_t dwSize = wcslen(wstr_ProxySettings.c_str()) * sizeof(TCHAR);
			memset((void*)slump, 0, 256 * sizeof(BYTE));
			memcpy((void*)slump, (const void*)wstr_ProxySettings.c_str(), dwSize * sizeof(BYTE));

			RegSetValueEx(hKey, L"ProxyServer", 0, REG_SZ,
				(const byte*)slump, dwSize);
			RegCloseKey(hKey);
		}
		else {
			RegCloseKey(hKey);
			return -1;
		};

		HKEY POverride;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_ALL_ACCESS, &POverride) == ERROR_SUCCESS)
		{
			std::wstring wstr_CPUname(ProxyOverride.begin(), ProxyOverride.end());
			BYTE slump[256];
			size_t dwSize = wcslen(wstr_CPUname.c_str()) * sizeof(TCHAR);
			memset((void*)slump, 0, 256 * sizeof(BYTE));
			memcpy((void*)slump, (const void*)wstr_CPUname.c_str(), dwSize * sizeof(BYTE));

			RegSetValueEx(hKey, L"ProxyOverride", 0, REG_SZ,
				(const byte*)slump, dwSize);
		}
		else {
			RegCloseKey(hKey);
			return -1;
		};
	}
	else
	{
		HKEY EnableKey;
		RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &EnableKey, NULL);
		DWORD value = 0;
		RegSetValueExW(EnableKey, L"ProxyEnable", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
		RegCloseKey(EnableKey);
	}
}

void ChangeProxy() {
	HKEY hKey = NULL;
	DWORD buff;
	DWORD dwType = REG_DWORD;
	DWORD dwBufSize = 255;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, L"ProxyEnable", 0, &dwType, (LPBYTE)&buff, &dwBufSize) == ERROR_SUCCESS)
		{
			if (buff == 0)
			{
				cout << "Прокси выключен\n";
				cout << "Включаю...\n";
				ChangeP(1);
				cout << "Ok!";
			}
			else
			{
				cout << "Прокси включен\n";
				cout << "Выключаю..\n";
				ChangeP(0);
				cout << "Ok!";
			};
		}
		else
		{
			cout << "Не удалось проверить включен ли прокси.";
			cout << "Включить? 0 - нет, 1 - да\n";
			int select; cin >> select;
			(select) ? ChangeP(1) : ChangeP(0);
		}
		RegCloseKey(hKey);
	};
}