/*
	BinningHV2: カメラの水平ビニング、垂直ビニングに2を設定
	BinningHV2: Set Camera Binning Horizontal / Binning Vertical to 2
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Parameter_BinningHV2
{
    class Parameter_BinningHV2
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

                    // ===========================================================================================================
                    // 水平、垂直ビニングに2を設定するデモ
                    // Demostration of setting Binning Horizontal / Binning Veritcal to 2

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // BinningHorizontalノードを取得
                    // Acquire Binning Horizontal node
                    IInteger intBinningHorizontal = nodeMap.GetNode<IInteger>("BinningHorizontal");
                    // BinningHorizontalに2を設定
                    // Set BinningHorizontal to 2
                    intBinningHorizontal.Value = 2;

                    // BinningVerticalノードを取得
                    // Acquire Binning Vertical node
                    IInteger intBinningVertical = nodeMap.GetNode<IInteger>("BinningVertical");
                    // BinningVerticalに2を設定
                    // Set BinningVertical to 2
                    intBinningVertical.Value = 2;

                    // ============================================================================================================

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
