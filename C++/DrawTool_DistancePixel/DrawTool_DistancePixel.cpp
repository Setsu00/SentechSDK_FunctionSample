/*
	Draw Tool Selected Line Distance Calculation in Pixel.
	This sample code demostrate how to acquire the distances between 2 points as a Line that created by draw tool of Sentech SDK Image display window.
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#include <iomanip>	//std::setprecision
#endif

using namespace StApi;
using namespace std;
const uint64_t nCountOfImagesToGrab = INFINITE;

typedef IStImageDisplayWnd* UserParam_t;

// Drawing callback event type. Define as a structure for if more than one node need to be handled.
typedef enum _EDrawingEventType_t
{
	// DrawingEventType_ItemType,	// Sample of other type declearation.
	DrawingEventType_ObjectSelector
}EDrawingEventType_t;

// Declear Integer Pointer of INode for Drawing Object Selector
GenApi::CIntegerPtr		m_pIInteger_DrawingObjectSelector;

// Declear IStImageDisplyWindow for display
CIStImageDisplayWndPtr	m_pIStImageDisplayWnd;

//-----------------------------------------------------------------------------
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase* pIStCallbackParamBase, UserParam_t pvContext)
{
	// Check callback type. Only NewBuffer event is handled in here
	if (pIStCallbackParamBase->GetCallbackType() == StCallbackType_GenTLEvent_DataStreamNewBuffer)
	{
		// In case of receiving a NewBuffer events:
		// Convert received callback parameter into IStCallbackParamGenTLEventNewBuffer for acquiring additional information.
		IStCallbackParamGenTLEventNewBuffer* pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);

		try
		{
			// Get the IStDataStream interface pointer from the received callback parameter.
			IStDataStream* pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();

			// Retrieve the buffer pointer of image data for that callback indicated there is a buffer received.
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));

			// Check if the acquired data contains image data.
			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{

				// If yes, we create a IStImage object for further image handling.
				IStImage* pIStImage = pIStStreamBuffer->GetIStImage();

				// Check if display window is visible.
				if (!pvContext->IsVisible())
				{
					// Set the position and size of the window.
					pvContext->SetPosition(0, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());

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
				cout << "Image data does not exist." << endl;
			}
		}
		catch (const GenICam::GenericException& e)
		{
			// If any exception occurred, display the description of the error here.
			cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
		}
	}
}

//-----------------------------------------------------------------------------
// Callback function for registering
//-----------------------------------------------------------------------------
void __stdcall OnStCallbackCFunction(IStCallbackParamBase* pIStCallbackParamBase, UserParam_t pvContext)
{
	OnCallback(pIStCallbackParamBase, pvContext);
}


// Distance Pixel Calculation function
double CalculateDistancePixel(double aX, double aY, double bX, double bY)
{
	double difX = abs(aX - bX);
	double difY = abs(aY - bY);
	return sqrt( pow(difX,2) + pow(difY, 2));
}

//-----------------------------------------------------------------------------
// Drawing Callback for registering to handle Node actions of IStDisplayWindow.
//-----------------------------------------------------------------------------
void OnNodeCallbackDrawingEvent(GenApi::INode* pINode, EDrawingEventType_t eType)
{
	// Make sure of the type of callback is called. This is useful when trying to handle more than one kind of action.
	if (eType == DrawingEventType_ObjectSelector)
	{
		// Acquire DrawingObjectPointSelector for select between 2 points
		GenApi::CIntegerPtr pIInteger_Point = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointSelector");
		// Node for acquiring X value of point.
		GenApi::CFloatPtr pIFloat_X = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointX");
		// Node for acquiring Y value of point.
		GenApi::CFloatPtr pIFloat_Y = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointY");

		if (GenApi::IsWritable(pIInteger_Point))
		{
			// Acquire current image Size for later calculation
			StApi::IStImage* pIStImage = m_pIStImageDisplayWnd->GetRegisteredIStImage();

			// Acquire Point 1 Coordinate and conver its unit from proportion to pixel
			pIInteger_Point->SetValue(0);
			const double dblX1 = pIFloat_X->GetValue() * pIStImage->GetImageWidth();
			const double dblY1 = pIFloat_Y->GetValue() * pIStImage->GetImageHeight();

			// Acquire Point 2 Coordinate and conver its unit from proportion to pixel
			pIInteger_Point->SetValue(1);
			const double dblX2 = pIFloat_X->GetValue() * pIStImage->GetImageWidth();
			const double dblY2 = pIFloat_Y->GetValue() * pIStImage->GetImageHeight();

			// Calculate and output distance
			const double dDistance = CalculateDistancePixel(dblX1, dblY1, dblX2, dblY2);

			// Print out result on console
			// Note that when start drawing, this callback will also be trigger and will have a distance 0 result.
			// Here we ignore the result of 0 and output the result which is not 0.
			if (dDistance != 0)
			{
				cout << "Selected Item Point Distance: " << CalculateDistancePixel(dblX1, dblY1, dblX2, dblY2) << endl;
			}
		}
	}
}


// Main function
int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));
		CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());
		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// ==============================================================================================================
		// Demostrate registering callback to catch Node change for calculation distance between 2 points.
		// Here we use another callback for display, which referenced from SDK sample GrabCallback.

		// Create display window
		m_pIStImageDisplayWnd.Reset(StApi::CreateIStWnd(StApi::StWindowType_ImageDisplay));

		// Enable Draw tool of DisplayWindow
		m_pIStImageDisplayWnd->SetBuiltInDrawingToolbar(true);

		// Get DrawingObjectSelector Node of DisplayWindow for later use
		m_pIInteger_DrawingObjectSelector = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectSelector");

		// Register callback to be called whenever DrawingObjectSelector is changed
		// For DrawingObjectSelector changed, trigger callback with sending "DrawingEventType_ObjectSelector" to callback function.
		StApi::RegisterCallback(m_pIInteger_DrawingObjectSelector->GetNode(), OnNodeCallbackDrawingEvent, DrawingEventType_ObjectSelector, GenApi::cbPostInsideLock);

		// Register another callback for display.
		StApi::RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)m_pIStImageDisplayWnd);

		// ==============================================================================================================
		
		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		
		// Keep getting image until Enter key pressed.
		cout << "Press Enter to exit." << endl;
		cout << "Draw then click a Line the acquire the distance between two point in pixel unit." << endl;
		for (;;)
		{
			if (cin.get() == '\n') break;
			Sleep(500);
		}

		pIStDevice->AcquisitionStop();
		pIStDataStream->StopAcquisition();
	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
