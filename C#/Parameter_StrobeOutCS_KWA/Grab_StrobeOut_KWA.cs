/*
    Strobe Out: Line 2を出力1000 us信号のStrobe Outに設定します。(カメラがExposure Activeのときに1000 us信号を出力する。)
	Strobe Out: Set Line2 as Strobe Out with output 1000us signal. (Output 1000us signal when camera exposure active.) 
    * For STC-MB/MCS Series only(STC-MB/MCSシリーズのみ対応)
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_StrobeOut_KWA
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
                    // Line2にStrobe Outを設定するデモ
                    // Demostration of Setting Line2 as Strobe Out

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // LineSelectorをLine2に設定し、出力に使用するラインを選択
                    // Set LineSelector to Line2 to select the output line.
                    IEnum enumLineSelector = nodeMap.GetNode<IEnum>("LineSelector");
                    enumLineSelector.FromString("Line2");

                    // 選択したLine2のLineModeをOutputに設定
                    // Set LineMode for the selected Line2 to Output.
                    IEnum enumLineMode = nodeMap.GetNode<IEnum>("LineMode");
                    enumLineMode.FromString("Output");

                    // Line2出力をTimer 0 Activeに切替
                    // Switch Line2 output source to Timer 0 Active
                    IEnum enumLineSource = nodeMap.GetNode<IEnum>("LineSource");
                    enumLineSource.FromString("Timer0Active");

                    // TimerSelectorをTimer0に設定し、制御するタイマーを選択
                    // Set TimerSelector to Timer0 to select the timer to configure.
                    IEnum enumTimerSelector = nodeMap.GetNode<IEnum>("TimerSelector");
                    enumTimerSelector.FromString("Timer0");

                    // Timer0のTriggerSourceとしてExposureStartを設定
                    // Set exposure start as the trigger source of Timer0
                    IEnum enumTimerTriggerSource = nodeMap.GetNode<IEnum>("TimerTriggerSource");
                    enumTimerTriggerSource.FromString("ExposureStart");

                    // Timer0出力期間に1000usを設定
                    // Set Timer0 output duration to 1000us
                    IFloat floatTimerDuration = nodeMap.GetNode<IFloat>("TimerDuration");
                    floatTimerDuration.Value = 1000;

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
