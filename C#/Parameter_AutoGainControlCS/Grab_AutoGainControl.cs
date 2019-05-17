/*
	Auto Gain Control: Set auto gain control on with range 0 dB to 10 dB.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_AutoGainControl
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

                    // Switch on Gain Auto(IEnumeration).
                    IEnum enumGainAuto = nodeMap.GetNode<IEnum>("GainAuto");
                    enumGainAuto.FromString("Continuous");

                    // Get Node for Auto Luminance Target(IInteger)
		            IInteger intAutoLuminTgt = nodeMap.GetNode<IInteger>("AutoLuminanceTarget");
                    // Set Auto Luminance Target to 128
                    intAutoLuminTgt.Value = 128;

                    // For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
                    IEnum enumGainSelector = nodeMap.GetNode<IEnum>("GainSelector");
                    enumGainSelector.FromString("AnalogAll");

                    // Get Node for GainAutoLimitMin(IFloat).
                    IFloat floatGainAutoMin = nodeMap.GetNode<IFloat>("GainAutoLimitMin");
                    // Set Auto Gain Min to 0 dB (0).
                    floatGainAutoMin.Value = 20;

                    // Get Node for GainAutoLimitMax(IFloat).
                    IFloat floatGainAutoMax = nodeMap.GetNode<IFloat>("GainAutoLimitMax");
                    // Set Auto Gain Max to 10 dB (100).
                    floatGainAutoMax.Value = 100;

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
