/*
	Parameter_FocusValue.cs
	FocusValueを持つカメラで、カメラ設定により光学フォーカスを制御する例です。
	Demostrates for those module that having focus value for controlling optical focus via camera setting.

*/

using System;
using Sentech.StApiDotNET;
using Sentech.GenApiDotNET;


// メインプログラム
// Main program
namespace GrabCallback
{
    class GrabCallback
    {
        static void OnCallback(IStCallbackParamBase paramBase, object[] param)
        {
            if (paramBase.CallbackType == eStCallbackType.TL_DataStreamNewBuffer)
            {
                IStCallbackParamGenTLEventNewBuffer callbackParam = paramBase as IStCallbackParamGenTLEventNewBuffer;

                if (callbackParam != null)
                {
                    try
                    {
                        IStDataStream dataStream = callbackParam.GetIStDataStream();
                        using (CStStreamBuffer streamBuffer = dataStream.RetrieveBuffer(0))
                        {
                            if (streamBuffer.GetIStStreamBufferInfo().IsImagePresent)
                            {
                                IStImage stImage = streamBuffer.GetIStImage();
#if ENABLED_ST_GUI
                                CStImageDisplayWnd wnd = (CStImageDisplayWnd)param[0];

                                if (!wnd.IsVisible)
                                {
                                    wnd.SetPosition(0, 0, (int)stImage.ImageWidth, (int)stImage.ImageHeight);
                                    wnd.Show(eStWindowMode.ModalessOnNewThread);
                                }

                                wnd.RegisterIStImage(stImage);
#else
                                Byte[] imageData = stImage.GetByteArray();
                                Console.Write("BlockId=" + streamBuffer.GetIStStreamBufferInfo().FrameID);
                                Console.Write(" Size:" + stImage.ImageWidth + " x " + stImage.ImageHeight);
                                Console.Write(" First byte =" + imageData[0] + Environment.NewLine);
#endif
                            }
                            else
                            {
                                Console.WriteLine("Image data does not exist.");
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
                    }
                }
            }
        }

        static void Main(string[] args)
        {
            try
            {
                using (CStApiAutoInit api = new CStApiAutoInit())

                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

                using (CStDevice device = system.CreateFirstStDevice())

#if ENABLED_ST_GUI
                using (CStImageDisplayWnd wnd = new CStImageDisplayWnd())
#endif
                using (CStDataStream dataStream = device.CreateStDataStream(0))
                {
                    Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // フォーカス機能を確認し、画像表示と入力値によるフォーカス制御を行うデモ
                    // Demonstration of checking focus support, displaying images, and controlling focus with entered values.
                    
                    // パラメーター操作用のNodeMapを取得
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // OpticControllerSelectorノードの有無を確認
                    // Confirm if OpticControllerSelector exist
                    IEnum pOpticControllerSelector = nodeMap.GetNode<IEnum>("OpticControllerSelector");
                    if (pOpticControllerSelector != null)
                    {
                        Console.WriteLine("OpticControllerSelector found.");

#if ENABLED_ST_GUI
                        // 画像表示用のDataStreamコールバックを登録
                        // Register a DataStream callback for displaying images.
                        object[] param = { wnd };
                        dataStream.RegisterCallbackMethod(OnCallback, param);
#else
						// 画像情報表示用のDataStreamコールバックを登録
						// Register a DataStream callback for displaying image information.
					    dataStream.RegisterCallbackMethod(OnCallback);
#endif
                        dataStream.StartAcquisition();
                        device.AcquisitionStart();

                        // 制御対象のFocusValueノードを取得
                        // Acquire FocusValue node for control
                        IInteger pFocusValue = nodeMap.GetNode<IInteger>("FocusValue");

                        if (pFocusValue.IsReadable)
                        {
                            // FocusValueの最小値と最大値を取得
                            // Acquire min/max of FocusValue
                            long iMinFocus = pFocusValue.Minimum;
                            long iMaxFocus = pFocusValue.Maximum;

                            while (true)
                            {
                                // 現在のFocusValueと範囲を表示し、設定する値を入力
                                // Display the current FocusValue and its range, then enter the value to set.
                                long iCurrentFocus = pFocusValue.GetValue(true, true);
                                Console.WriteLine($"FocusValue range: Min={iMinFocus}, Max={iMaxFocus}");
                                Console.WriteLine($"Current Value: {iCurrentFocus}");
                                Console.Write("Enter a FocusValue within this range (99999 to exit): ");
                                long userFocus = Convert.ToInt64(Console.ReadLine());

                                // 99999が入力された場合はフォーカス調整を終了
                                // End focus adjustment if the entered value is 99999.
                                if (userFocus == 99999)
                                    break;

                                if (userFocus >= iMinFocus && userFocus <= iMaxFocus)
                                {
                                    // 入力した値をFocusValueに設定
                                    // Setting focus value by input value
                                    pFocusValue.SetValue(userFocus, true);
                                    Console.WriteLine("FocusValue set to: " + userFocus);
                                }
                                else
                                {
                                    Console.WriteLine($"Invalid value. Please enter a value between {iMinFocus} and {iMaxFocus}.");
                                }
                            }
                        }
                        else
                        {
                            Console.WriteLine("FocusValue is not available.");
                        }

                        device.AcquisitionStop();
                        dataStream.StopAcquisition();
                    }
                    else
                    {
                        // OpticControllerSelectorがない場合はメッセージを表示して終了
                        // If OpticControllerSelector not exist, showing message then exit.
                        Console.WriteLine("This camera does not support focus adjustment.");
                    }
                    // ==============================================================================================================
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception occurred: " + ex.Message);
            }
        }
    }
}
