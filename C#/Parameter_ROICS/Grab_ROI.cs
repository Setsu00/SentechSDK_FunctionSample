/*
    ROI: Offset(100,200)、640x480でROI設定をする
	ROI: Set ROI to Offset (100, 200) and width/height (640, 480)
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_ROI
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
                    // オフセット(100,200)、640x480でROI設定を行うデモ
                    // Demostration of setting ROI to 640 x 480 with offset (100, 200).

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Widthノードを取得
                    // Get Node for Width
                    IInteger intWidth = nodeMap.GetNode<IInteger>("Width");
                    // 幅を640に設定
                    // Set Width to 640
                    intWidth.Value = 640;

                    // Heightノードを取得
                    // Get Node for Height
                    IInteger intHeight = nodeMap.GetNode<IInteger>("Height");
                    // 高さを480に設定
                    // Set Height to 480
                    intHeight.Value = 480;

                    // OffsetXのノードを取得
                    // Get Node for Offset X
                    IInteger intOffsetX = nodeMap.GetNode<IInteger>("OffsetX");
                    // Xオフセットに100を設定
                    // Set Offset X to 100
                    intOffsetX.Value = 100;

                    // OffsetYのノードを取得
                    // Get Node for Offset Y
                    IInteger intOffsetY = nodeMap.GetNode<IInteger>("OffsetY");
                    // Yオフセットに200を設定
                    // Set Offset Y to 200
                    intOffsetY.Value = 200;

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
