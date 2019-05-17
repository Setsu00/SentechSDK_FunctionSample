/*
	Software Trigger: Demostrate how to set camera to Software Trigger mode and how to send Trigger command.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_SoftwareTrigger
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
                    // Demostration of Setting Software Trigger ON

                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Switch on Trigger Mode(IEnumeration).
                    IEnum enumTriggerMode = nodeMap.GetNode<IEnum>("TriggerMode");
                    enumTriggerMode.FromString("On");

                    // Set Trigger Source to Software
                    IEnum enumTriggerSource = nodeMap.GetNode<IEnum>("TriggerSource");
                    enumTriggerSource.FromString("Software");


                    // Prepear Software Trigger Command for later calling
                    ICommand cmdTriggerSoftware = nodeMap.GetNode<ICommand>("TriggerSoftware");

                    // ==============================================================================================================

					dataStream.StartAcquisition(nCountOfImagesToGrab);

					device.AcquisitionStart();

					while (dataStream.IsGrabbing)
					{

                        // ===============================================================================
                        // Demostration sending software trigger
                        cmdTriggerSoftware.Execute();
                        Console.WriteLine("Software Trigger Sent.");

                        // ===============================================================================

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
