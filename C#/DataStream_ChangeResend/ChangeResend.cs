// ChangeResend.cs : 
//
// このサンプルはIStDataStreamのMaximum Resend Requests Per Blockを50(Default)から0に変更する方法を示します。
//

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace DataStream_ChangeResend
{
    class ChangeResend
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
                    // ==============================================================================================================
                    // DataSteam対してのノードマップ生成
                    // Create NodeMap for DataStream
                    INodeMap nodeMap = dataStream.GetIStPort().GetINodeMap();

                    // Maximum Resend Requests Per Blockのノードを取得
                    IInteger IntegerResendRequests = nodeMap.GetNode<IInteger>("MaximumResendRequestsPerBlock");
                    // Maximum Resend Requests Per Blockに0を設定
                    IntegerResendRequests.Value = 0;

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
            catch(Exception e)
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
