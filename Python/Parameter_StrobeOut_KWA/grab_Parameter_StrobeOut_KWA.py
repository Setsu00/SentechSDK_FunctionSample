"""
grab_Parameter_StrobeOut_KWA.py

Strobe Out: Set Line2 as Strobe Out with a 1000 us signal.
The setting is for STC-MB/MCS (KWA) series cameras that provide these nodes.
"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Line2にStrobe Outを設定するデモ
    # Demonstration of setting Line2 as Strobe Out.

    # パラメータにアクセスするためのノードマップを取得
    # Get the NodeMap for accessing camera parameters.
    nodemap_remote = st_device.remote_port.nodemap

    # Line2を出力に設定
    # Set Line2 to output.
    line_selector = nodemap_remote.get_node("LineSelector")
    enum_line_selector = st.PyIEnumeration(line_selector)
    enum_line_selector.from_string("Line2")

    line_mode = nodemap_remote.get_node("LineMode")
    enum_line_mode = st.PyIEnumeration(line_mode)
    enum_line_mode.from_string("Output")

    # Line2の出力元をTimer0Activeに設定
    # Set Line2 output source to Timer0Active.
    line_source = nodemap_remote.get_node("LineSource")
    enum_line_source = st.PyIEnumeration(line_source)
    enum_line_source.from_string("Timer0Active")

    # Timer0をExposureStartで起動し、出力期間を1000 usに設定
    # Start Timer0 at ExposureStart and set its output duration to 1000 us.
    timer_selector = nodemap_remote.get_node("TimerSelector")
    enum_timer_selector = st.PyIEnumeration(timer_selector)
    enum_timer_selector.from_string("Timer0")

    timer_trigger_source = nodemap_remote.get_node("TimerTriggerSource")
    enum_timer_trigger_source = st.PyIEnumeration(timer_trigger_source)
    enum_timer_trigger_source.from_string("ExposureStart")

    timer_duration = nodemap_remote.get_node("TimerDuration")
    timer_duration.value = 1000

    # ============================================================

    st_datastream = st_device.create_datastream()

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
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

except Exception as exception:
    print(exception)
