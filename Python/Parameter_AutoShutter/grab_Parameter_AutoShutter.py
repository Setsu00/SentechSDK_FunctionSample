"""
grab_Parameter_AutoShutter.py
Set auto shutter on with range 1,000 us to 150,000 us.
(For M series GigEVision cameras)

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # 自動露光の範囲を設定するデモです。
    # Demostration of Setting Auto Shutter with dedicated range.

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap
    
    # ExposureModeをTimedに設定し、指定露光時間で動作させます。
    # Switch Exposure Mode to Timed
    exposure_mode = nodemap_remote.get_node("ExposureMode")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_exposure_mode = st.PyIEnumeration(exposure_mode)

    # ExposureModeをTimedに設定し、指定露光時間で動作させます。
    # Set Exposure Mode to Timed
    enum_exposure_mode.from_string("Timed")

    # ExposureAutoノードで自動露光の動作を設定します。
    # Configure automatic exposure with the ExposureAuto node.
    exposure_auto = nodemap_remote.get_node("ExposureAuto")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_exposure_auto = st.PyIEnumeration(exposure_auto)

    # ExposureAutoをContinuousに設定し、自動露光を継続します。
    # Set ExposureAuto to Continuous to adjust exposure automatically and continuously.
    enum_exposure_auto.from_string("Continuous")

    # AutoLuminanceTargetノードを取得します。
    # Get Node for Auto Luminance Target
    alc_tgt = nodemap_remote.get_node("AutoLuminanceTarget")

    # AutoLuminanceTargetを128に設定します。
    # Set Auto Luminance Target to 128
    alc_tgt.value = 128
        
    # ExposureAutoLimitMinノードを取得します。
    # Get Node for ExposureAutoLimitMin
    alc_limit_min = nodemap_remote.get_node("ExposureAutoLimitMin")

    # 自動露光時間の下限を1000 µsに設定します。
    # Set Auto Exposure time Min to 1,000 usec
    alc_limit_min.value = 1000
    
    # ExposureAutoLimitMaxノードを取得します。
    # Get Node for ExposureAutoLimitMax
    alc_limit_max = nodemap_remote.get_node("ExposureAutoLimitMax")

    # 自動露光時間の上限を150000 µsに設定します。
    # Set Auto Exposure time Max to 150,000 usec
    alc_limit_max.value = 150000
    
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
