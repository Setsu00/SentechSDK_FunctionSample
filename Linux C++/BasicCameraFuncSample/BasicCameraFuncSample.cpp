/*
    BasicCameraFuncSample.cpp
*/

// Include files for using StApi.
#include <StApi_TL.h>
#include <StApi_GUI.h>

// Include for input/output related functions.
//#include <curses.h>
#include <iomanip>

#include <thread>

#include <unistd.h>   //_getch
#include <termios.h>  //_getch

//Namespace for using StApi.
using namespace StApi;

//Namespace for using cout.
using namespace std;

//Namespace for using GenApi.
using namespace GenApi;

typedef IStImageDisplayWnd* UserParam_t;

// Define for ESC key input.
#define ESC_KEY        0x1B
#define BACKSPACE_KEY  0x7F

// Declearation of input helper function.
bool ValueInputChk(string * result);

// Declearation of input helper of getch().
char getch();

// Hardware Triggering checking flag
bool g_hwtrg_active = false;

//-----------------------------------------------------------------------------
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
    // Check callback type. Only NewBuffer event is handled in here
    if(pIStCallbackParamBase->GetCallbackType() == StCallbackType_GenTLEvent_DataStreamNewBuffer)
    {
        // In case of receiving a NewBuffer events:
        // Convert received callback parameter into IStCallbackParamGenTLEventNewBuffer for acquiring additional information.
        IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
        
        try
        {
            // Get the IStDataStream interface pointer from the received callback parameter.
            IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();

            // Retrieve the buffer pointer of image data for that callback indicated there is a buffer received.
            CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));

            // Check if the acquired data contains image data.
            if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
            {

                // If yes, we create a IStImage object for further image handling.
                IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

                // Check if display window is visible.
                if (!pvContext->IsVisible())
                {
                    // Set the position and size of the window.
                    pvContext->SetPosition(600, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());

                    // Create a new thread to display the window.
                    pvContext->Show(NULL, StWindowMode_Modaless);
                }

                // Register the image to be displayed.
                // This will have a copy of the image data and original buffer can be released if necessary.
                pvContext->RegisterIStImage(pIStImage);

                // [Hardware Trigger related message]
                if (g_hwtrg_active)
                {
                    cout << "\rHardware trigger received and executed.";
                 }

            }
            else
            {
                // If the acquired data contains no image data...
                //cout << "Image data does not exist." << endl;
            }
        }
        catch (const GenICam::GenericException &e)
        {
            // If any exception occurred, display the description of the error here.
            cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Callback function for registering
//-----------------------------------------------------------------------------
void OnStCallbackCFunction(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
    OnCallback(pIStCallbackParamBase, pvContext);
}

//-----------------------------------------------------------------------------
// SW Trigger Handling function
//-----------------------------------------------------------------------------
void funcSWTrigger(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Initial Software Trigger related parameter
        cout << "Software Trigger Operation." << endl;

        // TriggerModeをOnに設定し、トリガーを有効にします。
        // Set TriggerMode to On to enable triggering.
        GenApi::CEnumerationPtr pIEnumTrigMode(nodeMap->GetNode("TriggerMode"));
        *pIEnumTrigMode = "On";

        // TriggerSourceをSoftwareに設定します。
        // Set Trigger Source to Software
        GenApi::CEnumerationPtr pIEnumTrigSource(nodeMap->GetNode("TriggerSource"));
        *pIEnumTrigSource = "Software";

        // Prepear Software Trigger Command for later calling
        GenApi::CCommandPtr pcICommandTriggerSoftTrg(nodeMap->GetNode("TriggerSoftware"));

        cout << "Press any key to send software trigger command; press ESC to exit and switch back to freerun mode." << endl;
        // Waiting Trigger Loop
        for (;;)
        {
            if (getch() == ESC_KEY)
                break;

            // SoftwareTriggerコマンドを実行し、トリガーを送信します。
            // Send software trigger
            pcICommandTriggerSoftTrg->Execute();

            cout << "\rSoftware Trigger Sent.";
        }

    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }

    // 実行後はTriggerModeをOffに設定します。
    // Switch off trigger mode after demostration.
    GenApi::CEnumerationPtr pIEnumTrigMode(nodeMap->GetNode("TriggerMode"));
    *pIEnumTrigMode = "Off";
}

//-----------------------------------------------------------------------------
// HW Trigger Handling function (Line 0)
//-----------------------------------------------------------------------------
void funcHWTrigger(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Initial Software Trigger related parameter
        cout << "Hardware Trigger Operation." << endl;

        // TriggerModeをOnに設定し、トリガーを有効にします。
        // Set TriggerMode to On to enable triggering.
        GenApi::CEnumerationPtr pIEnumTrigMode(nodeMap->GetNode("TriggerMode"));
        *pIEnumTrigMode = "On";

        // ハードウェア入力のLine0をTriggerSourceに設定します。
        // Set TriggerSource to Line0 for hardware triggering.
        GenApi::CEnumerationPtr pIEnumTrigSource(nodeMap->GetNode("TriggerSource"));
        *pIEnumTrigSource = "Line0";

        // Set on hardware trigger handling flag
        g_hwtrg_active = true;

        cout << "Camera waiting for Hareware triggering. press ESC to exit and switch back to freerun mode." << endl;
        // Waiting Trigger Loop
        for (;;)
        {
            if (getch() == ESC_KEY)
                break;
        }

    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }

    // Set off hardware trigger handling flag
    g_hwtrg_active = false;

    // 実行後はTriggerModeをOffに設定します。
    // Switch off trigger mode after demostration.
    GenApi::CEnumerationPtr pIEnumTrigMode(nodeMap->GetNode("TriggerMode"));
    *pIEnumTrigMode = "Off";
}


//-----------------------------------------------------------------------------
// Exposure Setting Handling function
//-----------------------------------------------------------------------------
void funcExposureSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // 露光時間を設定できるよう、ExposureAutoをOffに設定します。
        // Switch off exposure auto. If exposure auto is on, exposure time cannot be set.
        GenApi::CEnumerationPtr pIEnumExpoAuto(nodeMap->GetNode("ExposureAuto"));
        *pIEnumExpoAuto = "Off";

        // 露光時間を設定できるよう、ExposureModeをTimedに設定します。
        // For setting camera exposure time, exposure mode must set to Timed to enable value input
        GenApi::CEnumerationPtr pIEnumExpoMode(nodeMap->GetNode("ExposureMode"));
        *pIEnumExpoMode = "Timed";

        // Get Node for ExposureTime
        GenApi::CNodePtr pNodeExposureTime = nodeMap->GetNode("ExposureTime");
        
        // If ExposureTime node is not exist, the exposure time setting method is not supported by the camera thus we end current setting.
        if (!pNodeExposureTime.IsValid())
        {
            cerr << endl << "Exposure Time setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }
        // 値を設定するため、ノードをCFloatPtrに変換します。
        // Convert Node to CFloatPtr for setting value

        GenApi::CFloatPtr pFloatExpoTime(pNodeExposureTime);

        for (;;)
        {
            cout << "Exposure Time min: " << pFloatExpoTime->GetMin() << "; max: " << fixed << setprecision(1) << pFloatExpoTime->GetMax() << " Current: " << pFloatExpoTime->GetValue() << endl;
            cout << "Input new exposure time value, or press ESC for exit: ";
            string sExpoTime;
            if (!ValueInputChk(&sExpoTime))
                return;

            double fExpoTime = ::atof(sExpoTime.c_str());

            // 入力値が範囲内の場合はカメラへ設定し、範囲外の場合は無視します。
            // If input value is between min and max, set it to camera; otherwise ignore it.
            if (fExpoTime >= pFloatExpoTime->GetMin() && fExpoTime <= pFloatExpoTime->GetMax())
            {
                pFloatExpoTime->SetValue(fExpoTime);
            }
        }

    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// Gain Setting Handling function
//-----------------------------------------------------------------------------
void funcGainSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // ゲインを設定できるよう、GainAutoをOffに設定します。
        // Switch off gain auto. If gain auto is on, gain value cannot be set.
        GenApi::CEnumerationPtr pIEnumGainAuto(nodeMap->GetNode("GainAuto"));
        *pIEnumGainAuto = "Off";

        // アナログゲインを制御するため、GainSelectorをAnalogAllに設定します。
        // For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
        // If GainSelector node is not exist, the gain setting method is not supported and we end current setting.
        try
        {
            GenApi::CEnumerationPtr pIEnumGainSelector(nodeMap->GetNode("GainSelector"));
            *pIEnumGainSelector = "AnalogAll";
        }
        catch (...)
        {
            cerr << endl << "Analog Gain setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }

        // Get Node for Gain
        GenApi::CNodePtr pNodeGain = nodeMap->GetNode("Gain");
        // 値を設定するため、ノードをCFloatPtrに変換します。
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatGain(pNodeGain);

        for (;;)
        {
            // Acquire and display min/max of analog gain value for reference and wait for input.
            cout << "Analog Gain min: " << pFloatGain->GetMin() << "; max: " << pFloatGain->GetMax() << " Current: " << pFloatGain->GetValue() << " (" << pFloatGain->GetValue() / 10 << " dB)"  << endl;
            cout << "Input new Gain value, or press ESC for exit: ";

            
            // Check input value. If ESC pressed, function will return false and we force exit.
            string sGain;
            if(!ValueInputChk(&sGain))
                return;

            // Convert input to float
            double fGain = ::atof(sGain.c_str());

            // 入力値が範囲内の場合はカメラへ設定し、範囲外の場合は無視します。
            // If the value is in the range, set the value to camera; otherwise ignore it.
            if (fGain >= pFloatGain->GetMin() && fGain <= pFloatGain->GetMax())
            {
                pFloatGain->SetValue(fGain);
            }
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// WB value Setting Handling function
//-----------------------------------------------------------------------------
void funcManualWBSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // 手動設定のため、BalanceWhiteAutoをOffに設定します。
        // Switch BalanceWhiteAuto to off for manual input
        // If BalanceWhiteAuto node is not exist, the camera does not support white balance adjustment and function will comes to end.
        try
        {
            GenApi::CEnumerationPtr pIEnumBalanceWhite(nodeMap->GetNode("BalanceWhiteAuto"));
            *pIEnumBalanceWhite = "Off";
        }
        catch (const GenICam::GenericException &e)
        {
            cerr << endl << "Manual white balance setting is not available for this camera." << endl;
            cout << "Press any key to quit manual white balance setting." << endl;
            getch();
            return;
        }

        double fRedRatio, fGreenRatio, fBlueRatio;
        double fNewRedRatio, fNewGreenRatio, fNewBlueRatio;
        
        // Get Node for BalanceRatio
        GenApi::CNodePtr pNodeBalanceRatio = nodeMap->GetNode("BalanceRatio");
        // 値を設定するため、ノードをCFloatPtrに変換します。
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatBalanceRatio(pNodeBalanceRatio);

        for (;;)
        {
            string sNewRedRatio, sNewGreenRatio, sNewBlueRatio;

            // Acquire current red ratio value
            // 赤の比率にアクセスするため、BalanceRatioSelectorをRedに設定します。
            // Switch balance ration selector to Red for access Red ratio value
            GenApi::CEnumerationPtr pIEnumBalanceRatioSelect(nodeMap->GetNode("BalanceRatioSelector"));
            *pIEnumBalanceRatioSelect = "Red";
            fRedRatio = pFloatBalanceRatio->GetValue();

            // 緑の比率にアクセスするため、BalanceRatioSelectorをGreenに設定します。
            // Switch balance ration selector to Green for access Green ratio value
            *pIEnumBalanceRatioSelect = "Green";
            // Acquire current green ratio value
            fGreenRatio = pFloatBalanceRatio->GetValue();

            // 青の比率にアクセスするため、BalanceRatioSelectorをBlueに設定します。
            // Switch balance ration selector to Blue for access Blue ratio value
            *pIEnumBalanceRatioSelect = "Blue";
            // Acquire current blue ratio value
            fBlueRatio = pFloatBalanceRatio->GetValue();

            // Ratio select.
            cout << "Current manual white balance ratio: R: " << fRedRatio << " / G: " << fGreenRatio << " / B: " << fBlueRatio << " " << endl;
            cout << "Please select the ratio for change: " << endl;
            cout << "1. Red Ratio " << endl;
            cout << "2. Green Ration " << endl;
            cout << "3. Blue Ratio " << endl;
            cout << "4. Exit Function" << endl;
            int iRatioSelector;
            cin >> iRatioSelector;

            switch (iRatioSelector)
            {
            case 1:
                cout << "Input new Red ratio, or press ESC to about: ";
                if (!ValueInputChk(&sNewRedRatio))
                    break;

                // Convert input to float
                fNewRedRatio = ::atof(sNewRedRatio.c_str());

                // 対象の色を選択し、入力したBalanceRatioを設定します。範囲外の値は無視します。
                // Select the color and set the entered BalanceRatio. Values outside the range are ignored.
                *pIEnumBalanceRatioSelect = "Red";
                if (fNewRedRatio >= pFloatBalanceRatio->GetMin() && fNewRedRatio <= pFloatBalanceRatio->GetMax())
                {
                    pFloatBalanceRatio->SetValue(fNewRedRatio);
                }
                break;
            case 2:
                cout << "Input new Green ratio, or press ESC for about: ";
                if (!ValueInputChk(&sNewGreenRatio))
                    break;

                // Convert input to float
                fNewGreenRatio = ::atof(sNewGreenRatio.c_str());

                // 対象の色を選択し、入力したBalanceRatioを設定します。範囲外の値は無視します。
                // Select the color and set the entered BalanceRatio. Values outside the range are ignored.
                *pIEnumBalanceRatioSelect = "Green";
                if (fNewGreenRatio >= pFloatBalanceRatio->GetMin() && fNewGreenRatio <= pFloatBalanceRatio->GetMax())
                {
                    pFloatBalanceRatio->SetValue(fNewGreenRatio);
                }
                break;
            case 3:
                cout << "Input new Blue ratio, or press ESC for about: ";
                if (!ValueInputChk(&sNewBlueRatio))
                    break;

                // Convert input to float
                fNewBlueRatio = ::atof(sNewBlueRatio.c_str());

                // 対象の色を選択し、入力したBalanceRatioを設定します。範囲外の値は無視します。
                // Select the color and set the entered BalanceRatio. Values outside the range are ignored.
                *pIEnumBalanceRatioSelect = "Blue";
                if (fNewBlueRatio >= pFloatBalanceRatio->GetMin() && fNewBlueRatio <= pFloatBalanceRatio->GetMax())
                {
                    pFloatBalanceRatio->SetValue(fNewBlueRatio);
                }
                break; 
            case 4:
                return;
            default:
                break;
            }
            cout << endl;
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// Save current camera setting to UserSet0 and set as default (Setting when power on)
//-----------------------------------------------------------------------------
void funcSaveCameraSettingAsDefault(IStDevice * pDevice, IStDataStream * pDataStream, GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // For access UserSet, stop camera acquisition first.
        // Also stop data stream to prevent influence by changed setting.
        cout << "Stopping Camera Acquisition...";
        pDevice->AcquisitionStop();
        pDataStream->StopAcquisition();
        cout << "Done." << endl;

        // 設定の保存先としてUserSetSelectorをUserSet0に設定します。
        // Switch UserSet to UserSet0 for saving camera setting inside.
        try
        {
            cout << "Switching UserSet Selector to UserSet0." << endl;
            GenApi::CEnumerationPtr pIEnumUserSetSelector(nodeMap->GetNode("UserSetSelector"));
            *pIEnumUserSetSelector = "UserSet0";
        }
        catch (...)
        {
            cerr << endl << "Current UserSet setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }

        // 現在の設定をUserSet0に保存します。
        // Save current setting to UserSet0.
        // IPアドレス関連の設定は保存されません。
        // *Be noted the IP address related setting will not be saved.
        cout << "Executing UserSet Save command...";
        GenApi::CCommandPtr pcICommandUserSetSave(nodeMap->GetNode("UserSetSave"));
        pcICommandUserSetSave->Execute();
        cout << "Done." << endl;

        // 起動時にUserSet0を使うため、UserSetDefaultをUserSet0に設定します。
        // Set UserSet Default to "UserSet0" for indicating the UserSet for camera when start up.
        cout << "Set default UserSet to UserSet0." << endl;
        GenApi::CEnumerationPtr pIEnumUserSetDefault(nodeMap->GetNode("UserSetDefault"));
        *pIEnumUserSetDefault = "UserSet0";

        // After Saving, restart acquisition
        cout << "Restart Camera Acquisition...";
        pDataStream->StartAcquisition();
        pDevice->AcquisitionStart();
        cout << "Done." << endl;

        cout << "Press any key to exit." << endl;
        getch();
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// Load default setting of the camera
//-----------------------------------------------------------------------------
void funcLoadDefaultSetting(IStDevice * pDevice, IStDataStream * pDataStream, GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // For access UserSet, stop camera acquisition first.
        // Also stop data stream to prevent influence by changed setting.
        cout << "Stopping Camera Acquisition...";
        pDevice->AcquisitionStop();
        pDataStream->StopAcquisition();
        cout << "Done." << endl;

        // 既定設定を読み込むため、UserSetSelectorをDefaultに設定します。
        // Switch User Set to Default for reading the default setting.
        cout << "Switching UserSet Selector to Default." << endl;
        GenApi::CEnumerationPtr pIEnumUserSetSelector(nodeMap->GetNode("UserSetSelector"));
        *pIEnumUserSetSelector = "Default";

        // UserSetLoadコマンドを実行し、選択した設定を読み込みます。
        // Execute Load command
        cout << "Executing UserSet Load command...";
        GenApi::CCommandPtr pcICommandUserSetLoad(nodeMap->GetNode("UserSetLoad"));
        pcICommandUserSetLoad->Execute();
        cout << "Done." << endl;

        // After loading, restart camera acquisition
        cout << "Restart Camera Acquisition...";
        pDataStream->StartAcquisition();
        pDevice->AcquisitionStart();
        cout << "Done." << endl;

        cout << "Press any key to exit." << endl;
        getch();
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}


//-----------------------------------------------------------------------------
// Display Choice Worker
//-----------------------------------------------------------------------------
void displayChoiceWorker(CIStDevicePtr *pIStDevice, CIStDataStreamPtr *pIStDataStream, CNodeMapPtr *pNodeMapCameraParam, bool *isCompleted)
{
    // Start the image acquisition of the host (local machine) side.
    (*pIStDataStream)->StartAcquisition();

    // Start the image acquisition of the camera side.
    (*pIStDevice)->AcquisitionStart();
        
    bool endLoop = false;

    // Loop for entering selected function for demostration.
    do
    {
        system("clear");
        cout << "========= Basic Camera Function Sample ============" << endl;
        cout << "1. Software Trigger Function" << endl;
        cout << "2. Hardware Trigger Function (Line0 Input)" << endl;
        cout << "3. Exposure Time Setting" << endl;
        cout << "4. Analog Gain Setting" << endl;
        cout << "5. Manual White Balance Setting" << endl;
        cout << "6. Save Current Setting As Default" << endl;
        cout << "7. Load Factory Setting" << endl;
        cout << "8. Exit" << endl;
        cout << "===================================================" << endl;
        cout << "Please Select Function Number: ";

        int input;
        cin >> input;
            
        switch (input)
        {
            case 1:
                funcSWTrigger(*pNodeMapCameraParam);
                break;
            case 2:
                funcHWTrigger(*pNodeMapCameraParam);
                break;
            case 3:
                funcExposureSetting(*pNodeMapCameraParam);
                break;
            case 4:
                funcGainSetting(*pNodeMapCameraParam);
                break;
            case 5:
                funcManualWBSetting(*pNodeMapCameraParam);
                break;
            case 6:
                funcSaveCameraSettingAsDefault(*pIStDevice, *pIStDataStream, *pNodeMapCameraParam);
                break;
            case 7:
                funcLoadDefaultSetting(*pIStDevice, *pIStDataStream, *pNodeMapCameraParam);
                break;
            case 8:
            default:
                endLoop = true;
                break;
        }
    }while (!endLoop);

    // Stop the image acquisition of the camera side.
    (*pIStDevice)->AcquisitionStop();

    // Stop the image acquisition of the host side.
    (*pIStDataStream)->StopAcquisition();

    *isCompleted = true;
}

//-----------------------------------------------------------------------------
// Main thread
//-----------------------------------------------------------------------------
int main(int /* argc */, char ** /* argv */)
{
    try
    {
        // Initialize StApi before using.
        CStApiAutoInit objStApiAutoInit;

        // Create a system object for device scan and connection.
        CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));

        // Create a camera device object and connect to first detected device.
        CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());

        // Displays the DisplayName of the device.
        cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

        // If using GUI for display, create a display window here.
        CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));

        // Create a DataStream object for handling image stream data.
        CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

        // Register callback function. Note that by different usage, we pass different kinds/numbers of parameters in.
        RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)pIStImageDisplayWnd);

        // パラメーター操作用のNodeMapを取得します。
        // Create NodeMap pointer for accessing parameters
        GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

        // ==== Camera Setting Initial Part ====
        // TriggerModeをOffに設定し、トリガーを無効にします。
        // Set TriggerMode to Off to disable triggering.
        GenApi::CEnumerationPtr pIEnumTrigMode(pNodeMapCameraParam->GetNode("TriggerMode"));
        *pIEnumTrigMode = "Off";

        // 機能デモのため、ExposureModeをTimedに設定します。
        // Set camera exposure mode to timed here for function demo
        GenApi::CEnumerationPtr pIEnumExpoMode(pNodeMapCameraParam->GetNode("ExposureMode"));
        *pIEnumExpoMode = "Timed";

        // 露光時間を設定できるよう、ExposureAutoをOffに設定します。
        // Switch off exposure auto. If exposure auto is on, exposure time cannot be set.
        GenApi::CEnumerationPtr pIEnumExpoAuto(pNodeMapCameraParam->GetNode("ExposureAuto"));
        *pIEnumExpoAuto = "Off";

        // ゲインを設定できるよう、GainAutoをOffに設定します。
        // Switch off gain auto. If gain auto is on, gain value cannot be set.
        GenApi::CEnumerationPtr pIEnumGainAuto(pNodeMapCameraParam->GetNode("GainAuto"));
        *pIEnumGainAuto = "Off";

        // 利用可能な場合はBalanceWhiteAutoをOffに設定します。
        // Also set white balance auto to off if avaliable
        try
        {
            GenApi::CEnumerationPtr pIEnumBalanceWhite(pNodeMapCameraParam->GetNode("BalanceWhiteAuto"));
            *pIEnumBalanceWhite = "Off";
        }
        catch (const GenICam::GenericException &e)
        {
            // Do nothing when BalanceWhiteAuto is not avaliable.
        }
        // =====================================

        bool isCompleted = false;
        thread worker(displayChoiceWorker, &pIStDevice, &pIStDataStream, &pNodeMapCameraParam, &isCompleted);
        
        // Process the GUI event while waiting for thread completion        
        while(!isCompleted)
        {
            if(!pIStImageDisplayWnd->IsVisible())
            {
                // Create a new thread to display the window.
                pIStImageDisplayWnd->Show(NULL, StWindowMode_Modaless);
            }
            processEventGUI();
        }        
        if (worker.joinable()) worker.join();
    }
        catch (const GenICam::GenericException &e)
    {
        // If any exception occurred, display the description of the error here.
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
    }

    return(0);
}

//-----------------------------------------------------------------------------
// Helper function for handling value input
//-----------------------------------------------------------------------------
bool ValueInputChk(string * result)
{
    // Value input checking loop
    char chInput;
    bool bChkLoop = true;
    while (bChkLoop)
    {
        chInput = getch();
        switch (chInput)
        {
        case ESC_KEY:
            // ESC : Force quit
            return false;
            break;
        case '\n': // Enter
            // Enter : End input
            bChkLoop = false;
            cout << endl;
            break;
        case BACKSPACE_KEY: // Backspace
            // Backspace : Erease one character
            //cout << "BS";
            if (result->size() > 0)
            {
                cout << "\b"; cout << " "; cout << "\b";    // Clean up one character of display
                *result = result->substr(0, result->size() - 1);    // Remove one character of input data
            }
            break;
        // Only hanlde number input. Otherwise ignore.
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            cout << chInput;
            *result += chInput;
            break;
        default:
            break;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Helper function for getch() in Linux
//-----------------------------------------------------------------------------
char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    return buf;
 }

