/*
    Hardware Trigger (Active High): Line 0を入力、アクティブハイ (Trigger activation as rising edge) としてカメラをハードウェアトリガーモードに設定します。
	Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_HardwareTriggerActiveHigh
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
                    // アクティブハイでハードウェアトリガーをONに設定するデモ
                    // Demostration of Setting Hardware Trigger ON with active high

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Line0に入力を設定
                    // Set Line0 to input
                    IEnum enumLineSelector = nodeMap.GetNode<IEnum>("LineSelector");
                    enumLineSelector.FromString("Line0");
                    IEnum enumLineMode = nodeMap.GetNode<IEnum>("LineMode");
                    enumLineMode.FromString("Input");

                    // TriggerMode(IEnumeration)をOnに切替
                    // Switch on Trigger Mode(IEnumeration).
                    IEnum enumTriggerMode = nodeMap.GetNode<IEnum>("TriggerMode");
                    enumTriggerMode.FromString("On");

                    // TriggerSourceにハードウェア入力としてLine0を設定
                    // Set Trigger Source to Line0 as Hardware input
                    IEnum enumTriggerSource = nodeMap.GetNode<IEnum>("TriggerSource");
                    enumTriggerSource.FromString("Line0");

                    // TriggerActivationにアクティブハイを設定
                    // Set trigger activation to active high
                    IEnum enumTriggerActive = nodeMap.GetNode<IEnum>("TriggerActivation");
                    enumTriggerActive.FromString("RisingEdge");

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

                    // ==============================================================================================================
                    // Set Software Trigger OFF after using

                    // 取得後はTriggerMode(IEnumeration)をOffに切替
                    // Switch off Trigger Mode(IEnumeration) after acquiring.
                    enumTriggerMode.FromString("Off");

                    // ==============================================================================================================
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
