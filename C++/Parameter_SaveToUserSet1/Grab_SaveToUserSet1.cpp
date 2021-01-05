/*
	Grab_SaveToUserSet1.cpp : Demostration of saving camera setting to UserSet1 as default.
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#include <iomanip>	//std::setprecision
#endif

using namespace StApi;
using namespace std;
const uint64_t nCountOfImagesToGrab = 100;

int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));
		CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());
		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// ==============================================================================================================
		// Saving current setting to UserSet1 of camera, with setting it as default when camera power on.
		// Please notice the UserSet saving can be only avaliable when camera is not in acquiring.

		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Select which UserSet to save the setting (UserSet1)
		GenApi::CEnumerationPtr pEnumUserSetSelector(pNodeMapCameraParam->GetNode("UserSetSelector"));
		*pEnumUserSetSelector = "UserSet1";

		// Acquire and execute saving setting to UserSet
		GenApi::CCommandPtr pCmdUsetSetSave(pNodeMapCameraParam->GetNode("UserSetSave"));
		pCmdUsetSetSave->Execute();

		cout << "Save Current setting to UserSet1 succeed." << endl;

		// Set UserSetDefault to UsetSet1 for using this setting when camera power on.
		GenApi::CEnumerationPtr pIEnumUserSetDefault(pNodeMapCameraParam->GetNode("UserSetDefault"));
		*pIEnumUserSetDefault = "UserSet1";

		cout << "Set UserSetDefault to UserSet1 succeed." << endl;

		// ==============================================================================================================

	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
