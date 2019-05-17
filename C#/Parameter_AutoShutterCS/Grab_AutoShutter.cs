/*
	Auto Shutter: Set auto shutter on with range 1,000 us to 150,000 us.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_AutoShutter
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
                    // Demostration of Setting Auto Gain Control with dedicated range.

                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Switch on Exposure Auto(IEnumeration).
                    IEnum enumExposureAuto = nodeMap.GetNode<IEnum>("ExposureAuto");
                    enumExposureAuto.FromString("Continuous");

                    // Get Node for Auto Luminance Target(IInteger)
		            IInteger intAutoLuminTgt = nodeMap.GetNode<IInteger>("AutoLuminanceTarget");
                    // Set Auto Luminance Target to 128
                    intAutoLuminTgt.Value = 128;

                    // Get Node for ExposureAutoLimitMin(IFloat).
                    IFloat floatExpoAutoMin = nodeMap.GetNode<IFloat>("ExposureAutoLimitMin");
                    // Set Auto Exposure time Min to 1,000 usec
                    floatExpoAutoMin.Value = 1000;

                    // Get Node for ExposureAutoLimitMax(IFloat).
                    IFloat floatExpoAutoMax = nodeMap.GetNode<IFloat>("ExposureAutoLimitMax");
                    // Set Auto Exposure time Max to 150,000 usec
                    floatExpoAutoMax.Value = 150000;

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
