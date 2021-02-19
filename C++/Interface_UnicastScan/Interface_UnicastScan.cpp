/*
	Interface_UnicastScan.cpp
	Demstrate the way of setting GigE interface to scan device with unicast with indicated IP address.
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>

#include <WinSock2.h> // For inet_addr
#pragma comment(lib, "Ws2_32.lib")


using namespace StApi;
using namespace std;

int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;

		// =============================================================================================
		// Note: Creating system will  generate a broadcast device scan.
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech, StInterfaceType_GigEVision));

		// Input target IP address for scanning.
		string strAddress;
		cout << "Please input camera IP address for unicast camera scanning: " << endl;
		cin >> strAddress;

		// Convert IP string to integer value
		const uint32_t nTgtCameraIP = ntohl(inet_addr(strAddress.c_str()));

		// Acquire interface counts
		uint32_t uintInterfaceCnt = pIStSystem->GetInterfaceCount();

		for (uint32_t i = 0; i < uintInterfaceCnt; i++)
		{
			IStInterface* pInterface = pIStSystem->GetIStInterface(i);

			// Set Discovery Command Distination IP Address to enable the unicast camera scanning.
			// *This is 255.255.255.255 in default for boradcast device scanning.
			GenApi::CNodeMapPtr pINodeMap(pInterface->GetIStPort()->GetINodeMap());
			GenApi::CIntegerPtr pInteger(pINodeMap->GetNode("GevDeviceDiscoveryCommandDestinationIPAddress"));
			pInteger->SetValue(nTgtCameraIP);

			// After setting distination IP, call update for actual scanning process.
			// Unicast scanning package will be sent via this function.
			pInterface->UpdateDeviceList();
		}
		
		// Try to connect to any camera with indicated IP address.
		// If found, show the device display name;
		// if not found it will goes to exception handling.
		CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());

		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

		// =============================================================================================
	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
