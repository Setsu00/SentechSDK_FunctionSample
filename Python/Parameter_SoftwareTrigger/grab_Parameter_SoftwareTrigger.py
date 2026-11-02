"""
grab_Parameter_SoftwareTrigger.py

Software Trigger: Set the camera to software trigger mode and send trigger commands.
"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # ソフトウェアトリガーをOnに設定するデモ
    # Demonstration of setting software trigger on.

    # パラメータにアクセスするためのノードマップを取得
    # Get the NodeMap for accessing camera parameters.
    nodemap_remote = st_device.remote_port.nodemap

    # TriggerModeをOnに設定
    # Set TriggerMode to On.
    trigger_mode = nodemap_remote.get_node("TriggerMode")
    enum_trigger_mode = st.PyIEnumeration(trigger_mode)
    enum_trigger_mode.from_string("On")

    # TriggerSourceをSoftwareに設定
    # Set TriggerSource to Software.
    trigger_source = nodemap_remote.get_node("TriggerSource")
    enum_trigger_source = st.PyIEnumeration(trigger_source)
    enum_trigger_source.from_string("Software")

    # 後で呼び出すソフトウェアトリガーコマンドを取得
    # Get the software trigger command for use below.
    trigger_software = nodemap_remote.get_node("TriggerSoftware")
    command_trigger_software = st.PyICommand(trigger_software)

    # ============================================================

    st_datastream = st_device.create_datastream()

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        # ソフトウェアトリガーを送信してから、その直後のフレームを取得する。
        # Send a software trigger, then retrieve the resulting frame.
        command_trigger_software.execute()
        print("Software Trigger Sent.")

        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()
                print("BlockID={0} Size={1} x {2} First Byte={3}".format(
                    st_buffer.info.frame_id,
                    st_image.width, st_image.height,
                    st_image.get_image_data()[0]))
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

    # 使用後はTriggerModeをOffへ戻す。
    # Switch TriggerMode back to Off after use.
    enum_trigger_mode.from_string("Off")

except Exception as exception:
    print(exception)
