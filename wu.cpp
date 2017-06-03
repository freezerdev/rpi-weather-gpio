#include "wu.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <netdb.h>

#define PORT_HTTP	80

#define WU_SERVER	"weatherstation.wunderground.com"
#define WU_URL_BASE	"/weatherstation/updateweatherstation.php"
#define USER_AGENT	"Raspberry Pi"

/*****************************************************************************/
void Add(const unsigned int nIP, unsigned int *&pnIPs, unsigned short &nIPCount)
{
	bool bFound = false;
	unsigned short n;
	for(n = 0; n < nIPCount; ++n)
	{
		if(pnIPs[n] == nIP)
		{
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		unsigned int *pTmp = (unsigned int*)malloc((nIPCount + 1) * sizeof(unsigned int));
		if(pTmp)
		{
			for(n = 0; n < nIPCount; ++n)
				pTmp[n] = pnIPs[n];

			pTmp[nIPCount] = nIP;
			if(pnIPs)
				free(pnIPs);
			pnIPs = pTmp;
			++nIPCount;
		}
	}
}

/*****************************************************************************/
void GetServerIPAddresses(const char *szServer, unsigned int *&pnIPs, unsigned short &nIPCount)
{
	struct addrinfo aiHints = {0}, *paiResults = nullptr;
	if(getaddrinfo(szServer, nullptr, &aiHints, &paiResults) == 0)
	{
		struct addrinfo *paiScan = paiResults;
		while(paiScan)
		{
			Add(((struct sockaddr_in*)paiScan->ai_addr)->sin_addr.s_addr, pnIPs, nIPCount);
			paiScan = paiScan->ai_next;
		}

		freeaddrinfo(paiResults);
	}
}

/*****************************************************************************/
bool UpdateWU(const time_t nRaw, const CWeatherData &data)
{
	bool bSuccess = false;
	unsigned int *pnIPs = nullptr;
	unsigned short nIPCount = 0;

	GetServerIPAddresses(WU_SERVER, pnIPs, nIPCount);
	if(nIPCount)
	{
		struct sockaddr_in saiServer;
		saiServer.sin_family = AF_INET;
		saiServer.sin_port = htons(PORT_HTTP);

		// Build the HTTP string
		struct tm *pTime = gmtime(&nRaw);
		char szUrl[1024] = {0};
		char szQuery[1024] = {0};

		sprintf(szUrl, "%s?"\
			"ID=XXXXXXXXXX&"\
			"PASSWORD=XXXXXXXX&"\
			"dateutc=%04d-%02d-%02d+%02d%%3A%02d%%3A%02d&"\
			"tempf=%.2f&"\
			"baromin=%.2f&"\
			"humidity=%.2f&"\
			"dewptf=%.2f&"\
			"windspeedmph=%.2f&"\
			"windgustmph=%.2f&"\
			"rainin=%.2f&"\
			"dailyrainin=%.2f&",
			WU_URL_BASE,
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec,
			data.GetTemperature(),
			data.GetPressure(),
			data.GetHumidity(),
			data.GetDewPoint(),
			data.GetWindSpeed(),
			data.GetWindGust(),
			data.GetRainHour(),
			data.GetRainDay());

		if(data.GetWindSpeed() >= 3.0)
		{
			char szWindDir[64] = {0};
			sprintf(szWindDir, "winddir=%.2f&", data.GetWindDirection());
			strcat(szUrl, szWindDir);
		}

		strcat(szUrl, "action=updateraw");
		sprintf(szQuery, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", szUrl, WU_SERVER, USER_AGENT);

		for(unsigned short n = 0; n < nIPCount; ++n)
		{
			saiServer.sin_addr.s_addr = pnIPs[n];

			int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(nSocket != -1)
			{
				if(connect(nSocket, (struct sockaddr*)&saiServer, sizeof(saiServer)) != -1)
				{
					if(send(nSocket, szQuery, strlen(szQuery), 0))
					{
						bSuccess = true;
						break;
					}
				}

				shutdown(nSocket, SHUT_RDWR);
				close(nSocket);
			}
		}
	}

	if(pnIPs)
		free(pnIPs);
}
