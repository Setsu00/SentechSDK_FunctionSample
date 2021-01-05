﻿/*
	Acquisition Frame Rate: Set camera FPS to 2 then take 20 pics.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_AcquisitionFrameRateCS
	{
		const int nCountOfImagesToGrab = 20;

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
					// Demostration of changing acquisition frame rate (FPS)

					// Create NodeMap pointer for accessing parameters
					INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

					// Switch off exposure auto. If exposure auto is on, FPS setting may not able to implemented.
					IEnum enumExposureAuto = nodeMap.GetNode<IEnum>("ExposureAuto");
                    enumExposureAuto.FromString("Off");

					// Also switch Exposure Mode to Off for not letting exposure time to influence the actual FPS.
					IEnum enumExposureMode = nodeMap.GetNode<IEnum>("ExposureMode");
					enumExposureMode.FromString("Off");

					// Get Node for Acquisition Frame Rate then set FPS to 2.
					IFloat floatFPS = nodeMap.GetNode<IFloat>("AcquisitionFrameRate");
					floatFPS.SetValue(2, false);

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
