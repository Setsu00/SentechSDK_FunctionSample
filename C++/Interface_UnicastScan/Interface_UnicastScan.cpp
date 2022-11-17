/*
	Interface_UnicastScan.cpp
	指定されたIPアドレスを持つユニキャストを使用してデバイスをスキャンするようにGigEインターフェイスを設定する方法をデモします。
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
		// Note: システムを作成すると、ブロードキャストデバイススキャンが生成されます。
		// Note: Creating system will  generate a broadcast device scan.
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech, StInterfaceType_GigEVision));

		// スキャンするためのIPアドレスを入力
		// Input target IP address for scanning.
		string strAddress;
		cout << "Please input camera IP address for unicast camera scanning: " << endl;
		cin >> strAddress;

		// IPをstringから整数に変換
		// Convert IP string to integer value
		const uint32_t nTgtCameraIP = ntohl(inet_addr(strAddress.c_str()));

		// インターフェースの数を取得
		// Acquire interface counts
		uint32_t uintInterfaceCnt = pIStSystem->GetInterfaceCount();

		for (uint32_t i = 0; i < uintInterfaceCnt; i++)
		{
			IStInterface* pInterface = pIStSystem->GetIStInterface(i);

			// 検出コマンドの宛先IPアドレスを設定して、ユニキャストカメラスキャンを有効にします。
			// *ブロードキャストデバイススキャンのデフォルトは255.255.255.255です。
			// Set Discovery Command Distination IP Address to enable the unicast camera scanning.
			// *This is 255.255.255.255 in default for boradcast device scanning.
			GenApi::CNodeMapPtr pINodeMap(pInterface->GetIStPort()->GetINodeMap());
			GenApi::CIntegerPtr pInteger(pINodeMap->GetNode("GevDeviceDiscoveryCommandDestinationIPAddress"));
			pInteger->SetValue(nTgtCameraIP);

			// 接続先IPの設定後、実際のスキャンプロセスの更新を呼び出します。
			// ユニキャストスキャンパッケージは、この機能を介して送信されます。
			// After setting distination IP, call update for actual scanning process.
			// Unicast scanning package will be sent via this function.
			pInterface->UpdateDeviceList();
		}
		
		// 指定されたIPアドレスを持つカメラとの接続を試みる
		// もし見つかれば、デバイス名を表示
		// 見つからなければ、例外処理を行う
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
