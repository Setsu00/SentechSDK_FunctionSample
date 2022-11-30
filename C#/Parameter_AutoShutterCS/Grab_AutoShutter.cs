/*
    Auto Shutter: オートシャッターを1000us-150000usの範囲で設定
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
                    // オートシャッターを特定の範囲で設定するデモ
                    // Demostration of Setting Auto Gain Control with dedicated range.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    //ExposureAuto(IEnumeration)に切替
                    // Switch on Exposure Auto(IEnumeration).
                    IEnum enumExposureAuto = nodeMap.GetNode<IEnum>("ExposureAuto");
                    enumExposureAuto.FromString("Continuous");

                    // AutoLuminanceTarget(IInteger)ノードを取得
                    // Get Node for Auto Luminance Target(IInteger)
                    IInteger intAutoLuminTgt = nodeMap.GetNode<IInteger>("AutoLuminanceTarget");
                    // AutoLuminanceTargetに128を設定
                    // Set Auto Luminance Target to 128
                    intAutoLuminTgt.Value = 128;

                    // ExposureAutoLimitMin(IFloat)ノードを取得
                    // Get Node for ExposureAutoLimitMin(IFloat).
                    IFloat floatExpoAutoMin = nodeMap.GetNode<IFloat>("ExposureAutoLimitMin");
                    // AutoExposure timeの最小値に1000usを設定
                    // Set Auto Exposure time Min to 1,000 usec
                    floatExpoAutoMin.Value = 1000;

                    // ExposureAutoLimitMax(IFloat)ノードを取得
                    // Get Node for ExposureAutoLimitMax(IFloat).
                    IFloat floatExpoAutoMax = nodeMap.GetNode<IFloat>("ExposureAutoLimitMax");
                    // AutoExposure timeの最大値に150000usを設定
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
