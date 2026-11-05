/*
    PixelFormatConverterで取得した偏光画像をADOLP画像へ変換して表示します。
    Convert acquired polarization images to ADOLP images with PixelFormatConverter and display them.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
    class Grab_PolarizationADOLP
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
                    // PixelFormatConverterでADOLP変換を設定します。
                    // Configure ADOLP conversion with PixelFormatConverter.

                    using (CStPixelFormatConverter converter = new CStPixelFormatConverter())
                    {
                        // 変換先（出力）のピクセルフォーマットをBGR8に設定
                        // Set the destination (output) pixel format to BGR8.
                        converter.DestinationPixelFormat = eStPixelFormatNamingConvention.BGR8;

                        // 変換器のNodeMapを取得します。
                        // Acquire the converter's NodeMap.
                        INodeMap nodeMapConverter = converter.GetINodeMap();

                        // 偏光画像変換方式のノードを取得します。
                        // Get the polarization image transformation method node.
                        IEnum enumPolarizationMethod = nodeMapConverter.GetNode<IEnum>("PolarizationImageTransformationMethod");

                        // 直線偏光の角度と偏光度（ADOLP）への変換を設定します。
                        // Set conversion to angle and degree of linear polarization (ADOLP).
                        enumPolarizationMethod.FromString("AngleAndDegreeOfLinearPolarization");

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
                                        // 表示前に、取得した偏光画像をADOLP画像へ変換します。
                                        // Convert the acquired polarization image to an ADOLP image before display.
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
