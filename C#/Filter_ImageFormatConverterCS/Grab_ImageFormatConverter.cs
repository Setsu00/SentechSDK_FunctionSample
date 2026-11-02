/*
    Image Format Converter: 取得画像をMono8へ変換して表示する。
    Image Format Converter: Convert the acquired image to Mono8 and display it.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.StApiDotNET;

namespace Grab
{
    class Grab_ImageFormatConverter
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
                    // ピクセルフォーマット変換器を生成し、変換先をMono8に設定するデモ
                    // Demonstration of creating a pixel-format converter and setting the destination to Mono8.

                    using (CStPixelFormatConverter converter = new CStPixelFormatConverter())
                    {
                        // 変換先（出力）のピクセルフォーマットをMono8に設定
                        // Set the destination (output) pixel format to Mono8.
                        converter.DestinationPixelFormat = eStPixelFormatNamingConvention.Mono8;

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

                                    // ==============================================================================================================
                                    // 変換後の画像を格納する別バッファを生成する。
                                    // Create another buffer for storing the converted image.
                                    using (CStImageBuffer imageBuffer = CStApiDotNet.CreateStImageBuffer())
                                    {
                                        // 元画像を変換し、別バッファへ出力する。
                                        // Convert the original image and output it to another buffer.
                                        converter.Convert(stImage, imageBuffer);

                                        // 変換後の画像を表示する。
                                        // Display the converted image.
                                        wnd.RegisterIStImage(imageBuffer.GetIStImage());
                                    }
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
