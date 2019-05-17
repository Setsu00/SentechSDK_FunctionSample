/*
	Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_HardwareTriggerActiveHigh
	{
		const int nCountOfImagesToGrab = 100;

		static void Main(string[] args)
		{
			try
			{
				using (CStApiAutoInit api = new CStApiAutoInit())

				using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

				using (CStDevice device = system.CreateFirstStDevice())

				using (CStImageDisplayWnd wnd = new CStImageDisplayWnd())

				using (CStDataStream dataStream = device.CreateStDataStream(0))
				{
					Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // Demostration of Setting Hardware Trigger ON with active high

                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Set Line0 to input
                    IEnum enumLineSelector = nodeMap.GetNode<IEnum>("LineSelector");
                    enumLineSelector.FromString("Line0");
                    IEnum enumLineMode = nodeMap.GetNode<IEnum>("LineMode");
                    enumLineMode.FromString("Input");

                    // Switch on Trigger Mode(IEnumeration).
                    IEnum enumTriggerMode = nodeMap.GetNode<IEnum>("TriggerMode");
                    enumTriggerMode.FromString("On");

                    // Set Trigger Source to Line0 as Hardware input
                    IEnum enumTriggerSource = nodeMap.GetNode<IEnum>("TriggerSource");
                    enumTriggerSource.FromString("Line0");

                    // Set trigger activation to active high
                    IEnum enumTriggerActive = nodeMap.GetNode<IEnum>("TriggerActivation");
                    enumTriggerActive.FromString("RisingEdge");

                    // ==============================================================================================================

					dataStream.StartAcquisition(nCountOfImagesToGrab);

					device.AcquisitionStart();

					while (dataStream.IsGrabbing)
					{
						using (CStStreamBuffer streamBuffer = dataStream.RetrieveBuffer(5000))
						{
							if (streamBuffer.GetIStStreamBufferInfo().IsImagePresent)
							{
								IStImage stImage = streamBuffer.GetIStImage();
								string strText = device.GetIStDeviceInfo().DisplayName + " ";
								strText += stImage.ImageWidth + " x " + stImage.ImageHeight + " ";
								strText += string.Format("{0:F2}[fps]", dataStream.CurrentFPS);
								wnd.SetUserStatusBarText(strText);

								if (!wnd.IsVisible)
								{
									wnd.SetPosition(0, 0, (int)stImage.ImageWidth, (int)stImage.ImageHeight);

									wnd.Show(eStWindowMode.ModalessOnNewThread);
								}

								wnd.RegisterIStImage(stImage);
							}
							else
							{
								Console.WriteLine("Image data does not exist.");
							}
						}
					}

					device.AcquisitionStop();

					dataStream.StopAcquisition();

                    // ==============================================================================================================
                    // Set Software Trigger OFF after using

                    // Switch off Trigger Mode(IEnumeration) after acquiring.
                    enumTriggerMode.FromString("Off");

                    // ==============================================================================================================
				}
			}
			catch (Exception e)
			{
				Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
			}
			finally
			{
				Console.WriteLine("\r\nPress Enter to exit.");
				Console.ReadLine();
			}
		}
	}
}
