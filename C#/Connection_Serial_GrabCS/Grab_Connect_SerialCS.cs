
using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_Connect_SerialCS
	{
		const int nCountOfImagesToGrab = 100;

		static void Main(string[] args)
		{
			try
			{
				using (CStApiAutoInit api = new CStApiAutoInit())

                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))
                {
                    // ==== カメラシリアルでターゲットカメラに接続 ================================================================
                    // ==== Connect to target camera with camera serial. ================================================================
                    // シリアル入力
                    // Input Serial.
                    System.Console.WriteLine("Please input serial number of target camera: ");
                    string sTgtCameraSerial = System.Console.ReadLine();

                    // インターフェースの数を取得
                    // Acquire interface counts.
                    uint uiInterfaceCnt = system.InterfaceCount;

                    // 見つかったカメラのデバイスIDを保存する変数を準備
                    // Prepare a variable to store the matched camera's device ID.
                    string strTgtDeviceID = "";

                    // 見つかったカメラのインターフェース番号を保存する変数を準備
                    // Prepare a variable to store the matched camera's interface index.
                    uint uiTgtInterfaceNo = 0;

                    // ヒットフラグ
                    // Camera found hit flag.
                    bool bHit = false;

                    // ターゲットカメラが存在するか全てのインターフェースをチェック
                    // Check all interface if target camera is exist.
                    for (uint i = 0; i < uiInterfaceCnt; i++ )
                    {
                        IStInterface tmpInterface = system.GetIStInterface(i);
                        uint uiCamCnt = tmpInterface.DeviceCount;
                        for (uint j = 0; j < uiCamCnt ; j++)
                        {
                            IStDeviceInfo tmpDevInfo = tmpInterface.GetIStDeviceInfo(j);
                            if(tmpDevInfo.SerialNumber == sTgtCameraSerial)
                            {
                                strTgtDeviceID = tmpDevInfo.ID;
                                uiTgtInterfaceNo = i;
                                bHit = true;
                                break;
                            }
                        }
                        if (bHit)
                            break;
                    }

                    if(!bHit)
                    {
                        // 見つからなければメッセージと共に終了
                        // Not found, exit program with message.
                        System.Console.WriteLine("Target camera not found.");
                        System.Console.WriteLine("Press Enter to exit.");
                        Console.ReadLine();
                        return;
                    }

                        // 見つけたデバイスIDを使うためIStDevice経由で生成
                        // Create IStDevice via using found device ID.
                        using (CStDevice device = system.GetIStInterface(uiTgtInterfaceNo).CreateStDevice(strTgtDeviceID))

                        using (CStDataStream dataStream = device.CreateStDataStream(0))
                        {
                            Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                            dataStream.StartAcquisition(nCountOfImagesToGrab);

                            device.AcquisitionStart();

                            while (dataStream.IsGrabbing)
                            {
                                using (CStStreamBuffer streamBuffer = dataStream.RetrieveBuffer(5000))
                                {
                                    if (streamBuffer.GetIStStreamBufferInfo().IsImagePresent)
                                    {
                                        IStImage stImage = streamBuffer.GetIStImage();
                                        // 取得した画像の情報を表示
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
