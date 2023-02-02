//
// DataStream_CheckFilterDriver.cs : 
// 
// このサンプルはIStDataStreamのStreamDriverTypeを読み出す方法を示します。
// 読み出した情報よりフィルタドライバ使用しているかどうかの確認はできます。
// 
// This sample shows how to acquire the setting of IStDataStream for StreamDriverType.
// By checking the entry it can confirm if the filter driver is installed or not.
// 

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace DataStream_CheckFilterDriver
{
    class DataStream_CheckFilterDriver
    {
        const int nCountOfImagesToGrab = 100;
        static void Main(string[] args)
        {
            try
            {
                using (CStApiAutoInit api = new CStApiAutoInit())

                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

                using (CStDevice device = system.CreateFirstStDevice())

                using (CStDataStream dataStream = device.CreateStDataStream())
                {
                    Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // DataSteam対してのノードマップ生成
                    // Create NodeMap for DataStream
                    INodeMap nodeMap = dataStream.GetIStPort().GetINodeMap();

                    // StreamDriverTypeのノードを取得
                    // Acquire Node for "StreamDriverType" to check if current DataStream is using Filter Driver
                    IEnum EnumDriverType = nodeMap.GetNode<IEnum>("StreamDriverType");

                    // 今の設定を示す
                    // Display current setting
                    Console.WriteLine("Current stream driver type: " + EnumDriverType.CurrentEntryNode.Symbolic + Environment.NewLine);
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
                                Console.Write(" First byte =" + imageData[0] + Environment.NewLine);
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
