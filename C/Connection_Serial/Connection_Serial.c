/*
	Connection by Serial: 入力したシリアル番号のカメラへ接続して画像を取得する。
	Connection by Serial: Connect to the camera with the entered serial number and acquire images.
*/

#ifdef _MSC_VER
#pragma warning(disable : 4819)	
#endif
#include <StApi_C.h>
#include <string.h>
#ifdef _MSC_VER
#pragma warning(default : 4819)	
#endif

const uint64_t nCountOfImagesToGrab = 100;

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
	StApiHandle_t sIStDataStreamHandle = { NULL };

	if (eStApiCError != StApiCError_NoError)
	{
		printf("initializing StApiC was failed.\n");
		return(0);
	}
	for (;;)
	{
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateIStSystem(StSystemVendor_Default, StInterfaceType_All, &sIStSystemHandle);
		if (eStApiCError != StApiCError_NoError) break;

		// 接続するカメラのシリアル番号を入力します。
		// Enter the serial number of the camera to connect.
		char szTargetSerialNumber[1024];
		printf("Camera serial number: ");
		if (!fgets(szTargetSerialNumber, sizeof(szTargetSerialNumber), stdin)) break;
		szTargetSerialNumber[strcspn(szTargetSerialNumber, "\r\n")] = 0;

		uint32_t nInterfaceCount = 0;
		eStApiCError = psApiFunctions->StApi->IStSystem->GetInterfaceCount(&sIStSystemHandle, &nInterfaceCount);
		if (eStApiCError != StApiCError_NoError) break;

		// 各Interfaceのカメラを列挙し、一致するシリアル番号のカメラへ接続します。
		// Enumerate cameras on each interface and connect to the camera with the matching serial number.
		for (uint32_t i = 0; i < nInterfaceCount && !sIStDeviceHandle.Handle; ++i)
		{
			StApiHandle_t sIStInterfaceHandle = { NULL };
			uint32_t nDeviceCount = 0;
			bool8_t isDeviceListUpdated = false;

			eStApiCError = psApiFunctions->StApi->IStSystem->GetIStInterface(&sIStSystemHandle, i, &sIStInterfaceHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStInterface->UpdateDeviceList(&sIStInterfaceHandle, &isDeviceListUpdated);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStInterface->GetDeviceCount(&sIStInterfaceHandle, &nDeviceCount);
			if (eStApiCError != StApiCError_NoError) break;

			for (uint32_t j = 0; j < nDeviceCount; ++j)
			{
				StApiHandle_t sIStDeviceInfoHandle = { NULL };
				char szSerialNumber[1024];
				size_t nSerialNumberLength = sizeof(szSerialNumber);
				eStApiCError = psApiFunctions->StApi->IStInterface->GetIStDeviceInfo(&sIStInterfaceHandle, j, &sIStDeviceInfoHandle);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetSerialNumberA(&sIStDeviceInfoHandle, szSerialNumber, &nSerialNumberLength);
				if (eStApiCError != StApiCError_NoError) break;

				if (strcmp(szTargetSerialNumber, szSerialNumber) == 0)
				{
					eStApiCError = psApiFunctions->StApi->IStInterface->CreateIStDeviceByIndex(&sIStInterfaceHandle, j, DEVICE_ACCESS_CONTROL, &sIStDeviceHandle);
					break;
				}
			}
			if (eStApiCError != StApiCError_NoError) break;
		}
		if (eStApiCError != StApiCError_NoError) break;
		if (!sIStDeviceHandle.Handle)
		{
			printf("Target camera not found.\r\n");
			break;
		}

		{
			StApiHandle_t sIStDeviceInfoHandle = { NULL };
			char szDisplayName[1024];
			size_t nLen = sizeof(szDisplayName);
			eStApiCError = psApiFunctions->StApi->IStDevice->GetIStDeviceInfo(&sIStDeviceHandle, &sIStDeviceInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetDisplayNameA(&sIStDeviceInfoHandle, szDisplayName, &nLen);
			if (eStApiCError != StApiCError_NoError) break;

			printf("Device=%s\r\n", szDisplayName);
		}

		

		eStApiCError = psApiFunctions->StApi->IStDevice->CreateIStDataStream(&sIStDeviceHandle, 0, NULL, &sIStDataStreamHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStDataStream->StartAcquisition(&sIStDataStreamHandle, nCountOfImagesToGrab, ACQ_START_FLAGS_DEFAULT);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStDevice->AcquisitionStart(&sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;

		for (;;)
		{
			bool8_t isGrabbing;
			StApiHandle_t sIStStreamBufferHandle = { NULL };
			StApiHandle_t sIStStreamBufferInfoHandle = { NULL };
			bool8_t isImagePresent;

			eStApiCError = psApiFunctions->StApi->IStDataStream->IsGrabbing(&sIStDataStreamHandle, &isGrabbing);
			if (eStApiCError != StApiCError_NoError) break;
			if (!isGrabbing) break;

			eStApiCError = psApiFunctions->StApi->IStDataStream->RetrieveBuffer(&sIStDataStreamHandle, 5000, &sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStStreamBufferInfo(&sIStStreamBufferHandle, &sIStStreamBufferInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->IsImagePresent(&sIStStreamBufferInfoHandle, &isImagePresent);
			if (eStApiCError != StApiCError_NoError) break;

			if (isImagePresent)
			{
				StApiHandle_t sIStImageHandle = { NULL };
				size_t nImageWidth;
				size_t nImageHeight;

				eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStImage(&sIStStreamBufferHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;

				eStApiCError = psApiFunctions->StApi->IStImage->GetImageWidth(&sIStImageHandle, &nImageWidth);
				if (eStApiCError != StApiCError_NoError) break;

				eStApiCError = psApiFunctions->StApi->IStImage->GetImageHeight(&sIStImageHandle, &nImageHeight);
				if (eStApiCError != StApiCError_NoError) break;

				{

					StApiHandle_t sIStStreamBufferInfoHandle = { NULL };
					uint64_t iFrameID = 0;
					void *pBuffer = NULL;

					eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStStreamBufferInfo(&sIStStreamBufferHandle, &sIStStreamBufferInfoHandle);
					if (eStApiCError != StApiCError_NoError) break;

					eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->GetFrameID(&sIStStreamBufferInfoHandle, &iFrameID);
					if (eStApiCError != StApiCError_NoError) break;

					eStApiCError = psApiFunctions->StApi->IStImage->GetImageBuffer(&sIStImageHandle, &pBuffer);
					if (eStApiCError != StApiCError_NoError) break;

#ifdef _MSC_VER
					printf("BlockID=%llu Size: %Iu x %Iu First byte = %u\r\n", iFrameID, nImageWidth, nImageHeight, *(uint8_t*)pBuffer);
#else
					printf("BlockID=%llu Size: %zu x %zu First byte = %u\r\n", iFrameID, nImageWidth, nImageHeight, *(uint8_t*)pBuffer);
#endif
				}
			}
			else
			{
				printf("Image data does not exist\r\n");
			}
			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->Release(&sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;
		}

		psApiFunctions->StApi->IStDevice->AcquisitionStop(&sIStDeviceHandle);
		psApiFunctions->StApi->IStDataStream->StopAcquisition(&sIStDataStreamHandle, ACQ_STOP_FLAGS_DEFAULT);

		break;
	} 

	if (eStApiCError != StApiCError_NoError)
	{
		OutputErrorInfo(psApiFunctions);
	}

	if (sIStDataStreamHandle.Handle)
	{
		psApiFunctions->StApi->IStDataStream->Release(&sIStDataStreamHandle);
	}

	if (sIStDeviceHandle.Handle)
	{
		psApiFunctions->StApi->IStDevice->Release(&sIStDeviceHandle);
	}

	if (sIStSystemHandle.Handle)
	{
		psApiFunctions->StApi->IStSystem->Release(&sIStSystemHandle);
	}

	psApiFunctions->StApi->StApiCTerminate();

	printf("Press Enter to exit.");
	getchar();

	return 0;
}

