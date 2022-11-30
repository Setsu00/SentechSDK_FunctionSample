/*
    Auto Gain Control: オートゲイン操作を0dBから10dBの範囲で設定
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
                    // オートゲイン操作を特定の範囲で設定するデモ
                    // Demostration of Setting Auto Gain Control with dedicated range.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // GainAuto(IEnumeration)に切替
                    // Switch on Gain Auto(IEnumeration).
                    IEnum enumGainAuto = nodeMap.GetNode<IEnum>("GainAuto");
                    enumGainAuto.FromString("Continuous");

                    // AutoLuminanceTarget(IInteger)ノードを取得
                    // Get Node for Auto Luminance Target(IInteger)
                    IInteger intAutoLuminTgt = nodeMap.GetNode<IInteger>("AutoLuminanceTarget");
                    // AutoLuminanceTargetを128に設定
                    // Set Auto Luminance Target to 128
                    intAutoLuminTgt.Value = 128;

                    // アナログゲインを設定するため、アナログゲインにアクセスするためにGainSelectorをAnalogAllに設定する必要がある
                    // For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
                    IEnum enumGainSelector = nodeMap.GetNode<IEnum>("GainSelector");
                    enumGainSelector.FromString("AnalogAll");

                    // GainAutoLimitMin(IFloat)ノードを取得
                    // Get Node for GainAutoLimitMin(IFloat).
                    IFloat floatGainAutoMin = nodeMap.GetNode<IFloat>("GainAutoLimitMin");
                    // AutoGainの最小値に0dB(0)を設定
                    // Set Auto Gain Min to 0 dB (0).
                    floatGainAutoMin.Value = 20;

                    // GainAutoLimitMax(IFloat)ノードを取得
                    // Get Node for GainAutoLimitMax(IFloat).
                    IFloat floatGainAutoMax = nodeMap.GetNode<IFloat>("GainAutoLimitMax");
                    // AutoGainの最大値に10dB(100)を設定
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
