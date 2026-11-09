/*
	Interface Unicast Scan: ユニキャスト検出先のIPv4アドレスを設定してカメラを検出する。
	Interface Unicast Scan: Set the unicast discovery destination IPv4 address and discover the camera.
*/

#include <StApi_C.h>
#include <stdio.h>

void OutputErrorInfo(PApiFunctions psApiFunctions)
{
	char szText[1024];
	size_t nLen = sizeof(szText);
	EStApiCError_t eStApiCError = psApiFunctions->StApi->GetLastErrorDescriptionA(szText, &nLen);
	if (eStApiCError == StApiCError_NoError)
	{
		printf("Description:%s\r\n", szText);
	}
}


int main(int argc, char **argv)
{
	PApiFunctions psApiFunctions = NULL;
	EStApiCError_t eStApiCError = StApiCInitialize(STAPI_VERSION, &psApiFunctions);

	StApiHandle_t sIStSystemHandle = { NULL };
	StApiHandle_t sIStDeviceHandle = { NULL };

	if (eStApiCError != StApiCError_NoError)
	{
		printf("initializing StApiC was failed.\n");
		return(0);
	}
	for (;;)
	{
		// GigE Vision Interfaceを使用するSystemを生成します。
		// Create a System for GigE Vision interfaces.
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateIStSystem(StSystemVendor_Sentech, StInterfaceType_GigEVision, &sIStSystemHandle);
		if (eStApiCError != StApiCError_NoError) break;

		// ユニキャスト検出先のIPv4アドレスを入力します。
		// Enter the IPv4 address used as the unicast discovery destination.
		unsigned int nAddress0, nAddress1, nAddress2, nAddress3;
		char szAddress[64];
		printf("Camera IPv4 address: ");
		if (!fgets(szAddress, sizeof(szAddress), stdin)) break;
		if (sscanf(szAddress, "%u.%u.%u.%u", &nAddress0, &nAddress1, &nAddress2, &nAddress3) != 4) break;
		if (nAddress0 > 255 || nAddress1 > 255 || nAddress2 > 255 || nAddress3 > 255) break;
		uint32_t nIPAddress = (nAddress0 << 24) | (nAddress1 << 16) | (nAddress2 << 8) | nAddress3;

		uint32_t nInterfaceCount = 0;
		eStApiCError = psApiFunctions->StApi->IStSystem->GetInterfaceCount(&sIStSystemHandle, &nInterfaceCount);
		if (eStApiCError != StApiCError_NoError) break;

		// 各Interfaceへ検出先を設定してデバイスリストを更新します。
		// Set the discovery destination on each interface and update its device list.
		for (uint32_t i = 0; i < nInterfaceCount; ++i)
		{
			StApiHandle_t sIStInterfaceHandle = { NULL };
			StApiHandle_t sIStPortHandle = { NULL };
			StApiHandle_t sINodeMapHandle = { NULL };
			StApiHandle_t sIIntegerHandleDestinationIPAddress = { NULL };
			bool8_t isDeviceListUpdated = false;

			eStApiCError = psApiFunctions->StApi->IStSystem->GetIStInterface(&sIStSystemHandle, i, &sIStInterfaceHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStInterface->GetIStPort(&sIStInterfaceHandle, &sIStPortHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStPort->GetINodeMap(&sIStPortHandle, &sINodeMapHandle);
			if (eStApiCError != StApiCError_NoError) break;

			// GevDeviceDiscoveryCommandDestinationIPAddressへIPv4アドレスを設定します。
			// Set the IPv4 address on GevDeviceDiscoveryCommandDestinationIPAddress.
			eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapHandle, "GevDeviceDiscoveryCommandDestinationIPAddress", &sIIntegerHandleDestinationIPAddress);
			if (eStApiCError != StApiCError_NoError) break;

			// GevDeviceDiscoveryCommandDestinationIPAddressに入力値を設定します。
			// Set GevDeviceDiscoveryCommandDestinationIPAddress to the entered value.
			eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerHandleDestinationIPAddress, nIPAddress, false);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStInterface->UpdateDeviceList(&sIStInterfaceHandle, &isDeviceListUpdated);
			if (eStApiCError != StApiCError_NoError) break;
		}
		if (eStApiCError != StApiCError_NoError) break;

		eStApiCError = psApiFunctions->StApi->IStSystem->CreateFirstIStDevice(&sIStSystemHandle, DEVICE_ACCESS_CONTROL, &sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;

		StApiHandle_t sIStDeviceInfoHandle = { NULL };
		char szDisplayName[1024];
		size_t nDisplayNameLength = sizeof(szDisplayName);
		eStApiCError = psApiFunctions->StApi->IStDevice->GetIStDeviceInfo(&sIStDeviceHandle, &sIStDeviceInfoHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetDisplayNameA(&sIStDeviceInfoHandle, szDisplayName, &nDisplayNameLength);
		if (eStApiCError != StApiCError_NoError) break;
		printf("Device=%s\r\n", szDisplayName);

		break;
	}

	if (eStApiCError != StApiCError_NoError) OutputErrorInfo(psApiFunctions);
	if (sIStDeviceHandle.Handle) psApiFunctions->StApi->IStDevice->Release(&sIStDeviceHandle);
	if (sIStSystemHandle.Handle) psApiFunctions->StApi->IStSystem->Release(&sIStSystemHandle);
	psApiFunctions->StApi->StApiCTerminate();

	printf("Press Enter to exit.");
	getchar();
	return 0;
}
