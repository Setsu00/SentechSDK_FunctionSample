/*
	BinningHV1: カメラの水平ビニング、垂直ビニングに1を設定
	BinningHV1: Set Camera Binning Horizontal / Binning Vertical to 1
*/

#define ENABLED_ST_GUI

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Parameter_BinningHV1
{
    class Grab_BinningHV1
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
                    // 水平、垂直Binningに1を設定するデモ
                    // Demostration of setting Binning Horizontal / Binning Veritcal to 1

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // BinningHorizontalのノードを取得
                    // Acquire Binning Horizontal node
                    IInteger intBinningHorizontal = nodeMap.GetNode<IInteger>("BinningHorizontal");
                    // BinningHorizontalに1を設定
                    // Set BinningHorizontal to 1
                    intBinningHorizontal.Value = 1;

                    // BinningVerticalのノードを取得
                    // Acquire Binning Vertical node
                    IInteger intBinningVertical = nodeMap.GetNode<IInteger>("BinningVertical");
                    // BinningVerticalに1を設定
                    // Set BinningVertical to 1
                    intBinningVertical.Value = 1;

                    // Binningに1を設定するためにHeight、Widthを手動で最大値に設定する必要がある。
                    // For setting Binning to 1, both height and width need to be set to max manually.

                    // Widthノードを取得
                    // Acquire Width node
                    IInteger intWidth = nodeMap.GetNode<IInteger>("Width");
                    // Widthに可能な限り最大値を設定
                    // Set Width to Max as it can
                    intWidth.Value = intWidth.Maximum;

                    // Heightノードを取得
                    // Acquire Height node
                    IInteger intHeight = nodeMap.GetNode<IInteger>("Height");
                    // Heightに可能な限り最大値を設定
                    // Set Height to Max as it can
                    intHeight.Value = intHeight.Maximum;
                    // ===========================================================================================================

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
