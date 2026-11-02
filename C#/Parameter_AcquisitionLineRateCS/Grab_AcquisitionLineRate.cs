/*
    Acquisition Line Rate: カメラのラインレートを385に設定し、20枚の画像を取得する。
    Acquisition Line Rate: Set the camera line rate to 385 then acquire 20 images.

    注: AcquisitionLineRateノードを搭載したラインスキャンカメラが必要です。
    Note: A line-scan camera that provides the AcquisitionLineRate node is required.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
    class Grab_AcquisitionLineRateCS
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
                    // ラインレートを385に設定するデモ
                    // Demonstration of setting the acquisition line rate to 385.

                    // パラメータにアクセスするためのノードマップを取得
                    // Get the NodeMap for accessing camera parameters.
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // AcquisitionLineRateノードを取得し、ラインレートに385を設定
                    // Get the AcquisitionLineRate node then set the line rate to 385.
                    IFloat floatLineRate = nodeMap.GetNode<IFloat>("AcquisitionLineRate");
                    floatLineRate.SetValue(385, false);

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
