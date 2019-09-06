/*
    CameraAutoFuncSample.cpp
*/

// Include files for using StApi.
#include <StApi_TL.h>
#include <StApi_GUI.h>

// Include for input/output related functions.
#include <iomanip>

#include <thread>

#include <unistd.h>   //getch
#include <termios.h>  //getch

// Helper function to detect key pressed
#include "kbhit.h" 

// Namespace for using StApi.
using namespace StApi;

// Namespace for using cout.
using namespace std;

// Namespace for using GenApi.
using namespace GenApi;

typedef IStImageDisplayWnd* UserParam_t;

// Define for ESC key input.
#define ESC_KEY        0x1B
#define BACKSPACE_KEY   0x7F

// Declearation of input helper function.
bool ValueInputChk(string * result);

// Declearation of input helper of getch().
char getch();

//-----------------------------------------------------------------------------
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
    // Check callback type. Only NewBuffer event is handled in here.
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
                    pvContext->Show(NULL, StWindowMode_ModalessOnNewThread);
                }

                // Register the image to be displayed.
                // This will have a copy of the image data and original buffer can be released if necessary.
                pvContext->RegisterIStImage(pIStImage);

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
// Auto Exposure Setting Handling function
//-----------------------------------------------------------------------------
void funcAutoExposureSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Acquire Node of ExposureAuto for later usage.
        GenApi::CEnumerationPtr pIEnumExpoAuto(nodeMap->GetNode("ExposureAuto"));

        for (;;)
        {
            // Display current Auto Exposure Mode and wait for selection.
            cout << "Current Exposure Mode: " << pIEnumExpoAuto->GetCurrentEntry()->GetSymbolic()<< endl;
            cout << "1. Switch On Auto Exposure. (Continuous)" << endl;
            cout << "2. Switch Off Auto Exposure. (Off)" << endl;
            cout << "3. Exit Function." << endl;
            cout << "Please select: ";

            int iExpoAutoSel;
            cin >> iExpoAutoSel;

            switch (iExpoAutoSel)
            {
            case 1:
                // Set Auto Exposure Mode to Continuous.
                *pIEnumExpoAuto = "Continuous";
                break;
            case 2:
                // Set Auto Exposure Mode Off.
                *pIEnumExpoAuto = "Off";
                break;
            case 3:
                return;
            default:
                break;
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
void funcAutoGainSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Get Node for GainAuto for later usage
        GenApi::CEnumerationPtr pIEnumGainAuto(nodeMap->GetNode("GainAuto"));

        for (;;)
        {
            // Display Current Auto Gain Setting and wait for selection.
            cout << "Current Gain Mode: " << pIEnumGainAuto->GetCurrentEntry()->GetSymbolic() << endl;
            cout << "1. Switch On Auto Gain. (Continuous)" << endl;
            cout << "2. Switch Off Auto Gain. (Off)" << endl;
            cout << "3. Exit Function." << endl;
            cout << "Please select: ";

            int iGainAutoSel;
            cin >> iGainAutoSel;

            switch (iGainAutoSel)
            {
            case 1:
                // Set Auto Gain to continuous.
                *pIEnumGainAuto = "Continuous";
                break;
            case 2:
                // Set Auto Gain to Off.
                *pIEnumGainAuto = "Off";
                break;
            case 3:
                return;
            default:
                break;
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
// Auto Luminance Target
//-----------------------------------------------------------------------------
void funcAutoLuminanceTarget(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Acquire Auto Luminance Target node, which as the target brightness for AE/AGC.
        GenApi::CIntegerPtr pIIntLuminTgt(nodeMap->GetNode("AutoLuminanceTarget"));

        // Check if this node exist.
        if(pIIntLuminTgt == 0)
        {
            cerr << endl << "Auto Luminance setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }

        // Check if the node is writable.
        // There should be at least auto exposure or auto gain need to be switch to "Contiunous" or this value cannot be set.
        for (;;)
        {
            bool bWritable = GenApi::IsWritable(pIIntLuminTgt);
            if (!bWritable)
            {
                cout << "At least one of AE or AGC need to be switch on for setting this parameter!" << endl;
                cout << "Press any key to exit." << endl;
                getch();
                return;
            }

            // Acquire the range of AUto Luminance and read the input value.
            int64_t almMin, almMax;
            string sInput = "";
            almMin = pIIntLuminTgt->GetMin();
            almMax = pIIntLuminTgt->GetMax();
            cout << "Current Auto Luminance Target: " << pIIntLuminTgt->GetValue() << endl;
            cout << "Input New Auto Luminance Target (" << almMin  << " ~ "  << almMax << "), or press ESC to exit: " ;
            if (!ValueInputChk(&sInput))
                return;

            int64_t iInputValue = ::atoi(sInput.c_str());

            // Set target value. The value not in the range will be ignored here.
            if (iInputValue >= almMin && iInputValue <= almMax)
            {
                pIIntLuminTgt->SetValue(iInputValue);
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
// AWB Setting function
//-----------------------------------------------------------------------------
void funcAutoWBSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Acquire Node for Balance White Auto Setting.
        GenApi::CEnumerationPtr pIEnumBalanceWhite(nodeMap->GetNode("BalanceWhiteAuto"));
        
        // If Node acquire failed, the camera should be monochrome one or have to AWB function.
        // Exit current setting.
        if (pIEnumBalanceWhite == 0)
        {
            cerr << endl << "Manual white balance setting is not available for this camera." << endl;
            cout << "Press any key to quit manual white balance setting." << endl;
            getch();
            return;
        }

        for (;;)
        {
            // Select AWB mode.
            cout << "Current AWB setting: " << pIEnumBalanceWhite->GetCurrentEntry()->GetSymbolic() << endl;
            cout << "Please select target AWB mode: " << endl;
            cout << "1. Off" << endl;
            cout << "2. Once (Will adjust WB then switch back to Off)" << endl;
            cout << "3. Continuous " << endl;
            cout << "4. Exit" << endl;

            int iAWBSel;
            cin >> iAWBSel;

            switch (iAWBSel)
            {
            case 1:
                // Switch off AWB and use current WB value of camera.
                *pIEnumBalanceWhite = "Off";
                break;
            case 2:
                // Switch to AWB Once. 
                // The camera will takes several frames for WB calculation then switch back to "Off".
                *pIEnumBalanceWhite = "Once";
                break;
            case 3:
                // Switch to AWB Continuous. The camera will continuously check and adjust WB value.
                *pIEnumBalanceWhite = "Continuous";
                break;
            default:
                return;
                break;
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
// Exposure / Gain Value checking function
//-----------------------------------------------------------------------------
void funcExposureGainMonitor(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // Get Node for ExposureTime
        GenApi::CNodePtr pNodeExposureTime = nodeMap->GetNode("ExposureTime");
        // If ExposureTime node is not exist, the exposure time setting method is not supported by the camera thus we end current setting.
        if (pNodeExposureTime == 0)
        {
            cerr << endl << "Exposure Time setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatExpoTime(pNodeExposureTime);

        // Get Node for Gain
        GenApi::CNodePtr pNodeGain = nodeMap->GetNode("Gain");
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatGain(pNodeGain);

        cout << "Monitoring current Exposure Time and Gain Value...Press anykey to Exit." << endl;
        for(;;)
        {
            cout << "Current Exposure Time: " << pFloatExpoTime->GetValue() << " ; Gain: " << pFloatGain->GetValue() << "        \r";
            // Set NodeMap Poll value to make it acquire latest data from camera.
            nodeMap->Poll(500);
            usleep(10000);

            // Wait ESC for exit
            if (_kbhit())
            {
                getch();
                break;
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
// Display Choice Worker
//-----------------------------------------------------------------------------
void displayChoiceWorker(CIStDevicePtr *pIStDevice, CIStDataStreamPtr *pIStDataStream, CNodeMapPtr *pNodeMapCameraParam, bool *isCompleted)
{
    // Start the image acquisition of the host (local machine) side.
    (*pIStDataStream)->StartAcquisition();

    // Start the image acquisition of the camera side.
    (*pIStDevice)->AcquisitionStart();
        
    // Keep getting image until Enter key pressed.
    bool endLoop = false;

    // Loop for entering selected function for demostration.
    do
    {
        system("clear");
        cout << "========= Camera Auto Function Sample =============" << endl;
        cout << "1. Auto Exposure" << endl;
        cout << "2. Auto Gain" << endl;
        cout << "3. Auto Luminance Target" << endl;
        cout << "4. Balance White Auto" << endl;
        cout << "5. Monitor Current Exposure / Gain Value" << endl;
        cout << "6. Exit" << endl;
        cout << "===================================================" << endl;
        cout << "Please Select Function Number: ";

        int input;
        cin >> input;
            
        switch (input)
        {
        case 1:
            funcAutoExposureSetting(*pNodeMapCameraParam);
            break;
        case 2:
            funcAutoGainSetting(*pNodeMapCameraParam);
            break;
        case 3:
            funcAutoLuminanceTarget(*pNodeMapCameraParam);
            break;
        case 4:
            funcAutoWBSetting(*pNodeMapCameraParam);
            break;
        case 5:
            funcExposureGainMonitor(*pNodeMapCameraParam);
            break;
        default:
            endLoop = true;
            break;
        }
    } while (!endLoop);

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

        // If using GUI for display, create a display window here.
        CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));

        // Create a DataStream object for handling image stream data.
        CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

        // Register callback function. Note that by different usage, we pass different kinds/numbers of parameters in.
        RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)pIStImageDisplayWnd);

        // Create NodeMap pointer for accessing parameters
        GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

        // ==== Camera Setting Initial Part ====
        // Switch off Trigger Mode(IEnumeration).
        GenApi::CEnumerationPtr pIEnumTrigMode(pNodeMapCameraParam->GetNode("TriggerMode"));
        *pIEnumTrigMode = "Off";
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

