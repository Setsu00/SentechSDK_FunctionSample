/*
    Analog Gain Setting: アナログゲインを2 dBに設定する。
	Analog Gain Setting: Set analog gain to 2 dB.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Parameter_AnalogGainCS
{
	class Grab_AnalogGain
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
                    // アナログゲインを2 dBに設定する
                    // Set analog gain to 2 dB.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // GainAutoをOffに設定
                    // Set GainAuto to Off.
                    IEnum enumGainAuto = nodeMap.GetNode<IEnum>("GainAuto");
                    enumGainAuto.FromString("Off");

                    // GainSelectorをAnalogAllに設定
                    // Set GainSelector to AnalogAll.
                    IEnum enumGainSelector = nodeMap.GetNode<IEnum>("GainSelector");
                    enumGainSelector.FromString("AnalogAll");

                    // Gainノードを取得
                    // Get Node for Gain.
                    IFloat floatGain = nodeMap.GetNode<IFloat>("Gain");

                    // Gainに2 dBを設定
                    // Set Gain to 2 dB.
                    floatGain.Value = 2.0;

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
