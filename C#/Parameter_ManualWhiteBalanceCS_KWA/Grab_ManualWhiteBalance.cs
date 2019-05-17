/*
	Manual White Balance: Manually set white balance ratio of all color to 10.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_ManualWhiteBalance
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

                    // Switch BalanceWhiteAuto to Preset0 for manual input value
                    IEnum enumBalanceWhiteAuto = nodeMap.GetNode<IEnum>("BalanceWhiteAuto");
                    enumBalanceWhiteAuto.FromString("Preset0");

                    // Switch balance ration selector to Red for access Red ratio value
                    IEnum enumBalanceRatioSelector = nodeMap.GetNode<IEnum>("BalanceRatioSelector");
                    enumBalanceRatioSelector.FromString("Red");

                    // Get Node for BalanceRatio
                    IFloat floatBalanceRatio = nodeMap.GetNode<IFloat>("BalanceRatio");
                    // Set BalanceRatio to 10
                    floatBalanceRatio.Value = 10;

                    // Switch balance ration selector to Blue for access Blue ratio value
                    enumBalanceRatioSelector.FromString("Blue");

                    // Set BalanceRatio to 10
                    floatBalanceRatio.Value = 10;

                    // Switch balance ration selector to Green for access Green ratio value
                    enumBalanceRatioSelector.FromString("Green");

                    // Set BalanceRatio to 10
                    floatBalanceRatio.Value = 10;

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
