/*
	Parameter_SaveToUserSet1CS.cs : Demostration of saving camera setting to UserSet1 as default.
	
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Parameter_SaveToUserSet1CS
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
                    // Saving current setting to UserSet1 of camera, with setting it as default when camera power on.
					// Please notice the UserSet saving can be only avaliable when camera is not in acquiring.

                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // Select which UserSet to save the setting (UserSet1)
                    IEnum enumUserSetSelector = nodeMap.GetNode<IEnum>("UserSetSelector");
					enumUserSetSelector.FromString("UserSet1");

					// Acquire and execute saving setting to UserSet
					ICommand cmdSaveToUserSet = nodeMap.GetNode<ICommand>("UserSetSave");
					cmdSaveToUserSet.Execute();

					Console.WriteLine("Save Current setting to UserSet1 succeed.");

					// Set UserSetDefault to UsetSet1 for using this setting when camera power on.
					IEnum enumUserSetDefault = nodeMap.GetNode<IEnum>("UserSetDefault");
					enumUserSetDefault.FromString("UserSet1");

					Console.WriteLine("Set UserSetDefault to UserSet1 succeed.");

                    // ==============================================================================================================
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
