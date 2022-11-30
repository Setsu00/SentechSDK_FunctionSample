// StreamLostFrameCount.cs : 
// 
// このサンプルは、IStDataStreamよりLost Frame Countを取得、表示する方法を示します
// This sample demonstrate the way of acquiring Lost Frame Count from IStDataStream.

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace StreamLostFrameCount
{
    class StreamLostFrameCount
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
                    // DataSteam対してのノードマップ生成
                    // Create NodeMap for DataStream
                    INodeMap nodeMap = dataStream.GetIStPort().GetINodeMap();

                    // StreamLostFrameCountのノードを取得
                    // Acquire StreamLostFrameCount Node from NodeMap of DataStream for showing lost frame count.
                    IInteger IntegerLostFrameCount = nodeMap.GetNode<IInteger>("StreamLostFrameCount");

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
                                // Display the information of the acquired image data.
                                Byte[] imageData = stImage.GetByteArray();
                                Console.Write("BlockId=" + streamBuffer.GetIStStreamBufferInfo().FrameID);
                                Console.Write(" Size:" + stImage.ImageWidth + " x " + stImage.ImageHeight);
                                Console.Write(" First byte =" + imageData[0]);
                                // ==============================================================================================================
                                // StreamLostFrameCountの出力
                                // Output StreamLostFrameCount every frame.
                                Console.Write(" LostFrameCount =" + IntegerLostFrameCount.Value + Environment.NewLine);
                                // ==============================================================================================================
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
