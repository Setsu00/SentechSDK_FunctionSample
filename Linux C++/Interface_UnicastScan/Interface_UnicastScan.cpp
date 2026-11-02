/*
	Unicast Scan: 指定したIPv4アドレスへユニキャスト検出を実行する。
	Unicast Scan: Run unicast discovery for the specified IPv4 address.
*/

#include <StApi_TL.h>

#include <arpa/inet.h> // inet_addr / ntohl



using namespace StApi;
using namespace std;

int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;

		// =============================================================================================
		// GigE Visionシステムを生成します。
		// Create a GigE Vision system.
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech, StInterfaceType_GigEVision));

		// スキャンするためのIPアドレスを入力
		// Input target IP address for scanning.
		string strAddress;
		cout << "Please input camera IP address for unicast camera scanning: " << endl;
		cin >> strAddress;

		// IPv4アドレスを整数値へ変換します。
		// Convert the IPv4 address to an integer value.
		const uint32_t nTgtCameraIP = ntohl(inet_addr(strAddress.c_str()));

		// インターフェース数を取得します。
		// Get the interface count.
		uint32_t uintInterfaceCnt = pIStSystem->GetInterfaceCount();

		for (uint32_t i = 0; i < uintInterfaceCnt; i++)
		{
			IStInterface* pInterface = pIStSystem->GetIStInterface(i);

			// 検出コマンドの宛先IPアドレスを設定して、ユニキャストカメラスキャンを有効にします。
			// *ブロードキャストデバイススキャンのデフォルトは255.255.255.255です。
			// Set the discovery-command destination IP address to enable unicast camera scanning.
			// *The default value is 255.255.255.255 for broadcast device scanning.
			GenApi::CNodeMapPtr pINodeMap(pInterface->GetIStPort()->GetINodeMap());
			GenApi::CIntegerPtr pIntegerDiscoveryDestination(pINodeMap->GetNode("GevDeviceDiscoveryCommandDestinationIPAddress"));
			pIntegerDiscoveryDestination->SetValue(nTgtCameraIP);

			// 接続先IPの設定後、実際のスキャンプロセスの更新を呼び出します。
			// ユニキャストスキャンパッケージは、この機能を介して送信されます。
			// After setting the destination IP address, update the device list to start the scan.
			// The unicast discovery packet is sent by this function.
			pInterface->UpdateDeviceList();
		}
		
		// 検出したカメラへ接続し、デバイス名を表示します。
		// Connect to the discovered camera and display its device name.
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
