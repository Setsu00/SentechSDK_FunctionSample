/*!
	Parameter_FocusValue.cpp
	FocusValue対応カメラで光学フォーカスを制御します。
	Demostrates for those module that having focus value for controlling optical focus via camera setting.
*/

#define ENABLED_ST_GUI

// StApi用ヘッダーをインクルードします。
// Include files for using StApi.
#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#endif

#include <conio.h>
#include <iomanip>

// StApiの名前空間
// Namespace for using StApi.
using namespace StApi;

// GenApiの名前空間
// Namespace for GenAPI
using namespace GenApi;

// cout用の名前空間
// Namespace for using cout
using namespace std;

#ifdef ENABLED_ST_GUI
typedef IStImageDisplayWnd* UserParam_t;
#else
typedef void*	UserParam_t;
#endif

static uint64_t g_timeStampRec = 0;

//-----------------------------------------------------------------------------
// コールバック処理用の関数
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
	if(pIStCallbackParamBase->GetCallbackType() == StCallbackType_GenTLEvent_DataStreamNewBuffer)
	{
		IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		
		try
		{
			IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();

			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));

			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

#ifdef ENABLED_ST_GUI
				if (!pvContext->IsVisible())
				{
					pvContext->SetPosition(0, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());
					pvContext->Show(NULL, StWindowMode_ModalessOnNewThread);
				}

				pvContext->RegisterIStImage(pIStImage);
#else
				uint64_t iTimeStamp = pIStStreamBuffer->GetIStStreamBufferInfo()->GetTimestamp();

				UNREFERENCED_PARAMETER(pvContext);
				cout << "BlockId=" << pIStStreamBuffer->GetIStStreamBufferInfo()->GetFrameID()
					<< " Size:" << pIStImage->GetImageWidth() << " x " << pIStImage->GetImageHeight()
					<< " First byte =" << (uint32_t)*(uint8_t*)pIStImage->GetImageBuffer() << endl;
				cout << "TimeStamp =" << iTimeStamp << endl;
				if (g_timeStampRec == 0)
				{
					g_timeStampRec = iTimeStamp;
				}
				else
				{
					cout << "TimeStamp diff(ms): " << (iTimeStamp - g_timeStampRec) / 1000000 << endl;
					g_timeStampRec = iTimeStamp;
				}
#endif
			}
			else
			{
				cout << "Image data does not exist." << endl;
			}
		}
		catch (const GenICam::GenericException &e)
		{
			// 例外の説明を表示します。
			// If any exception occurred, display the description of the error here.
			cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
		}
	}
}

//-----------------------------------------------------------------------------
// 登録用のコールバック関数
// Callback function for registering
//-----------------------------------------------------------------------------
void __stdcall OnStCallbackCFunction(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
	OnCallback(pIStCallbackParamBase, pvContext);
}

//-----------------------------------------------------------------------------
// メインスレッド
// Main thread
//-----------------------------------------------------------------------------
int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;
		CIStSystemPtr pIStSystem(CreateIStSystem());
		CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());
		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

#ifdef ENABLED_ST_GUI

		RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)pIStImageDisplayWnd);
#else
		RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)NULL);
#endif

		// ==============================================================================================================
		// フォーカス機能を確認し、画像表示と入力値によるフォーカス制御を行うデモです。
		// Demostration of Checking if Focus function is availavble, with showing image and providing value input.

		// パラメーター操作用のNodeMapを取得します。
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// OpticControllerSelectorノードの有無を確認します。
		// Confirm if OpticControllerSelector exist
		CEnumerationPtr pOpticControllerSelector = pNodeMapCameraParam->GetNode("OpticControllerSelector");
		if (IsAvailable(pOpticControllerSelector) && IsReadable(pOpticControllerSelector))
		{
			std::cout << "OpticControllerSelector found." << std::endl;

			pIStDataStream->StartAcquisition();
			pIStDevice->AcquisitionStart();

			// 制御対象のFocusValueノードを取得します。
			// Acquire FocusValue node for control
			CIntegerPtr pFocusValue = pNodeMapCameraParam->GetNode("FocusValue");
			if (IsAvailable(pFocusValue) && IsReadable(pFocusValue))
			{
				// FocusValueの最小値・最大値を取得します。
				// Acquire min/max of FocusValue
				int iMinFocus = pFocusValue->GetMin();
				int iMaxFocus = pFocusValue->GetMax();
				
				for (;;)
				{
					int iCurrentFocus = pFocusValue->GetValue();
					
					std::cout << "FocusValue range: Min = " << iMinFocus << ", Max = " << iMaxFocus << std::endl;
					std::cout << "Current Value: " << iCurrentFocus << endl;
					std::cout << "Enter a FocusValue within this range, 99999 for exit: ";
					int userFocus;
					std::cin >> userFocus;

					// 入力値に応じて処理します。
					// Related action by input value.
					if (userFocus == 99999)
					{
						break;
					}
					else if (userFocus >= iMinFocus && userFocus <= iMaxFocus)
					{
						// 入力した値をFocusValueに設定します。
						// Setting focus value by input value
						pFocusValue->SetValue(userFocus);
						std::cout << "FocusValue set to: " << userFocus << std::endl;
					}
					
					else
					{
						std::cout << "Invalid value. Please enter a value between " << iMinFocus << " and " << iMinFocus << "." << std::endl;
					}
				}
			}
			else
			{
				std::cout << "FocusValue is not available." << std::endl;
			}

			pIStDevice->AcquisitionStop();
			pIStDataStream->StopAcquisition();
		}
		else
		{
			// OpticControllerSelectorがない場合はメッセージを表示して終了します。
			// If OpticControllerSelector not exist, showing message then exit.
			std::cout << "This camera does not support focus adjustment." << std::endl;
		}

		// ==============================================================================================================

	}
    catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	return(0);
}
