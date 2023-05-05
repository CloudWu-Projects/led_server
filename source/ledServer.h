#pragma once
#include <string>
#include <tuple>

class CLedDll;
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createAProgram(std::wstring& showText);

private:
	CLedDll *g_Dll;
	std::tuple<int, std::string> createAProgram(std::wstring networkID, std::wstring& showText);

	int m_ledType = 3;
	int m_ledWidth = 320, m_ledHeight = 40, m_ledColor = 3, m_ledGraylevel = 1;

};
