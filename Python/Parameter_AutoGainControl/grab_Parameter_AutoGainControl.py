"""
grab_Parameter_AutoGainControl.py

Auto Gain Control: Set auto gain control on with a range of 0 dB to 10 dB.
"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # オートゲイン操作を特定の範囲で設定するデモ
    # Demonstration of setting auto gain control with a dedicated range.

    # パラメータにアクセスするためのノードマップを取得
    # Get the NodeMap for accessing camera parameters.
    nodemap_remote = st_device.remote_port.nodemap

    # GainAutoをContinuousに設定
    # Set GainAuto to Continuous.
    gain_auto = nodemap_remote.get_node("GainAuto")
    enum_gain_auto = st.PyIEnumeration(gain_auto)
    enum_gain_auto.from_string("Continuous")

    # AutoLuminanceTargetを128に設定
    # Set AutoLuminanceTarget to 128.
    auto_luminance_target = nodemap_remote.get_node("AutoLuminanceTarget")
    auto_luminance_target.value = 128

    # アナログゲイン制御のためGainSelectorをAnalogAllに設定
    # Set GainSelector to AnalogAll for analog gain control.
    gain_selector = nodemap_remote.get_node("GainSelector")
    enum_gain_selector = st.PyIEnumeration(gain_selector)
    enum_gain_selector.from_string("AnalogAll")

    # AutoGainの最小値を0 dB、最大値を10 dBに設定
    # Set the auto-gain minimum to 0 dB and maximum to 10 dB.
    gain_auto_limit_min = nodemap_remote.get_node("GainAutoLimitMin")
    gain_auto_limit_min.value = 0

    gain_auto_limit_max = nodemap_remote.get_node("GainAutoLimitMax")
    gain_auto_limit_max.value = 100

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
