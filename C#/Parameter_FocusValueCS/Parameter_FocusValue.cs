/*
	Parameter_FocusValue.cs
	Demostrates for those module that having focus value for controlling optical focus via camera setting.

*/

using System;
using Sentech.StApiDotNET;
using Sentech.GenApiDotNET;


// Main program
namespace GrabCallback
{
    class GrabCallback
    {
        static void OnCallback(IStCallbackParamBase paramBase, object[] param)
        {
            if (paramBase.CallbackType == eStCallbackType.TL_DataStreamNewBuffer)
            {
                IStCallbackParamGenTLEventNewBuffer callbackParam = paramBase as IStCallbackParamGenTLEventNewBuffer;

                if (callbackParam != null)
                {
                    try
                    {
                        IStDataStream dataStream = callbackParam.GetIStDataStream();
                        using (CStStreamBuffer streamBuffer = dataStream.RetrieveBuffer(0))
                        {
                            if (streamBuffer.GetIStStreamBufferInfo().IsImagePresent)
                            {
                                IStImage stImage = streamBuffer.GetIStImage();
#if ENABLED_ST_GUI
                                CStImageDisplayWnd wnd = (CStImageDisplayWnd)param[0];

                                if (!wnd.IsVisible)
                                {
                                    wnd.SetPosition(0, 0, (int)stImage.ImageWidth, (int)stImage.ImageHeight);
                                    wnd.Show(eStWindowMode.ModalessOnNewThread);
                                }

                                wnd.RegisterIStImage(stImage);
#else
                                Byte[] imageData = stImage.GetByteArray();
                                Console.Write("BlockId=" + streamBuffer.GetIStStreamBufferInfo().FrameID);
                                Console.Write(" Size:" + stImage.ImageWidth + " x " + stImage.ImageHeight);
                                Console.Write(" First byte =" + imageData[0] + Environment.NewLine);
#endif
                            }
                            else
                            {
                                Console.WriteLine("Image data does not exist.");
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
                    }
                }
            }
        }

        static void Main(string[] args)
        {
            try
            {
                using (CStApiAutoInit api = new CStApiAutoInit())

                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

                using (CStDevice device = system.CreateFirstStDevice())

#if ENABLED_ST_GUI
                using (CStImageDisplayWnd wnd = new CStImageDisplayWnd())
#endif
                using (CStDataStream dataStream = device.CreateStDataStream(0))
                {
                    Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // Demostration of Checking if Focus function is availavble, with showing image and providing value input
                    
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Confirm if OpticControllerSelector exist
                    IEnum pOpticControllerSelector = nodeMap.GetNode<IEnum>("OpticControllerSelector");
                    if (pOpticControllerSelector != null)
                    {
                        Console.WriteLine("OpticControllerSelector found.");

#if ENABLED_ST_GUI
                        object[] param = { wnd };
                        dataStream.RegisterCallbackMethod(OnCallback, param);
#else
					    dataStream.RegisterCallbackMethod(OnCallback);
#endif
                        dataStream.StartAcquisition();
                        device.AcquisitionStart();

                        // Acquire FocusValue node for control
                        IInteger pFocusValue = nodeMap.GetNode<IInteger>("FocusValue");

                        if (pFocusValue.IsReadable)
                        {
                            // Acquire min/max of FocusValue
                            long iMinFocus = pFocusValue.Minimum;
                            long iMaxFocus = pFocusValue.Maximum;

                            while (true)
                            {
                                long iCurrentFocus = pFocusValue.GetValue(true, true);
                                Console.WriteLine($"FocusValue range: Min={iMinFocus}, Max={iMaxFocus}");
                                Console.WriteLine($"Current Value: {iCurrentFocus}");
                                Console.Write("Enter a FocusValue within this range (99999 to exit): ");
                                long userFocus = Convert.ToInt64(Console.ReadLine());

                                // Related action by input value.
                                if (userFocus == 99999)
                                    break;

                                if (userFocus >= iMinFocus && userFocus <= iMaxFocus)
                                {
                                    // Setting focus value by input value
                                    pFocusValue.SetValue(userFocus, true);
                                    Console.WriteLine("FocusValue set to: " + userFocus);
                                }
                                else
                                {
                                    Console.WriteLine($"Invalid value. Please enter a value between {iMinFocus} and {iMaxFocus}.");
                                }
                            }
                        }
                        else
                        {
                            Console.WriteLine("FocusValue is not available.");
                        }

                        device.AcquisitionStop();
                        dataStream.StopAcquisition();
                    }
                    else
                    {
                        // If OpticControllerSelector not exist, showing message then exit.
                        Console.WriteLine("This camera does not support focus adjustment.");
                    }
                    // ==============================================================================================================
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception occurred: " + ex.Message);
            }
        }
    }
}
