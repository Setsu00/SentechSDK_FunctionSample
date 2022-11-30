/*
    Exposure Time Setting: 露光時間に100000usを設定する
	Exposure Time Setting: Set camera exposure time to 100,000 us.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_ExposureTime
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
                    // カメラの露光時間(us)を変更するデモ
                    // Demostration of changing exposure time(us) of camera.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // ExposureAutoをOffに切替。もしExposureAutoがOnの場合、露光時間を設定できない
                    // Switch off exposure auto. If exposure auto is on, exposure time cannot be set.
                    IEnum enumExposureAuto = nodeMap.GetNode<IEnum>("ExposureAuto");
                    enumExposureAuto.FromString("Off");

                    // 値を入力出来るようにするためExposureModeをTimedに設定する必要がある
                    // For setting camera exposure time, exposure mode must set to Timed to enable value input
                    IEnum enumExpoMode = nodeMap.GetNode<IEnum>("ExposureMode");
                    enumExpoMode.FromString("Timed");

                    // ExposureTimeのノードを取得
                    // Get Node for ExposureTime
                    IFloat floatExpoTime = nodeMap.GetNode<IFloat>("ExposureTime");
                    // 露光時間に100000usを設定
                    // Set Exposure time to 100,000 usec
                    floatExpoTime.Value = 100000;

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
