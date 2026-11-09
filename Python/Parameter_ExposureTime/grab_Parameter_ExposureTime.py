"""
	Exposure Time Setting: Set camera exposure time to 100,000 us.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostration of Setting Auto Shutter with dedicated range.

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap
    
    # ExposureAutoノードで自動露光の動作を設定します。
    # Configure automatic exposure with the ExposureAuto node.
    exposure_auto = nodemap_remote.get_node("ExposureAuto")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_exposure_auto = st.PyIEnumeration(exposure_auto)

    # ExposureAutoをOffに設定し、自動露光を無効にします。
    # Set Exposure Auto to Off
    enum_exposure_auto.from_string("Off")
    
    # ExposureModeをTimedに設定し、指定露光時間で動作させます。
    # Switch Exposure Mode to Timed
    exposure_mode = nodemap_remote.get_node("ExposureMode")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_exposure_mode = st.PyIEnumeration(exposure_mode)

    # ExposureModeをTimedに設定し、指定露光時間で動作させます。
    # Set Exposure Mode to Timed
    enum_exposure_mode.from_string("Timed")

    # Get Node for ExposureTime
    alc_tgt = nodemap_remote.get_node("ExposureTime")

    # 露光時間を100000 µsに設定します。
    # Set Exposure time to 100,000 usec
    alc_tgt.value = 100000

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
