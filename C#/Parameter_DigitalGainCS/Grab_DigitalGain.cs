/*
    Digital Gain Setting: デジタルゲインを2倍に設定する
	Digital Gain Setting: Set digital gain to 2 times.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_Digital
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
                    // デジタルゲインを2倍に設定するデモ
                    // Demostration of set digital Gain to 2 times.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // GainAutoをOffに切替。もしGainAutoがOnの場合、ゲイン値を正しく設定できません。
                    // Switch off gain auto. If gain auto is on, gain value cannot be set.
                    IEnum enumGainAuto = nodeMap.GetNode<IEnum>("GainAuto");
                    enumGainAuto.FromString("Off");

                    // デジタルゲイン設定にアクセスするため、GainSelectorにDigitalAllを設定する必要がある
                    // For setting digital gain, gain selector need to be set to DigitalAll to access digital gain.
                    IEnum enumGainSelector = nodeMap.GetNode<IEnum>("GainSelector");
                    enumGainSelector.FromString("DigitalAll");

                    // Gainノードを取得
                    // Get Node for Gain
                    IFloat floatGain = nodeMap.GetNode<IFloat>("Gain");
                    // デジタルゲインを2倍(128)に設定
                    // Set digital gain to 2 times(128).
                    floatGain.Value = 128;

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
