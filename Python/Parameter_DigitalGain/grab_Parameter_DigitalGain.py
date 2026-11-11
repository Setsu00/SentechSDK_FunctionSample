"""
	Digital Gain Setting: Set digital gain to 2 times.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # デジタルゲインを2倍に設定するデモです。
    # Demostration of set digital Gain to 2 times.

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # ゲインを設定できるよう、自動ゲインを無効にします。
    # Switch off gain auto.
    # If Gain auto is on, gain value cannot be set
    gain_auto = nodemap_remote.get_node("GainAuto")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_gain_auto = st.PyIEnumeration(gain_auto)

    # GainAutoをOffに設定し、自動ゲイン調整を無効にします。
    # Set Gain Auto to Off
    enum_gain_auto.from_string("Off")

    # デジタルゲインを制御するため、GainSelectorをDigitalAllに設定します。
    # Set GainSelector to DigitalAll to control digital gain.
    gain_selector = nodemap_remote.get_node("GainSelector")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_gain_selector = st.PyIEnumeration(gain_selector)

    # デジタルゲインを制御するため、GainSelectorをDigitalAllに設定します。
    # Set GainSelector to DigitalAll to control digital gain.
    enum_gain_selector.from_string("DigitalAll")

    # Gainノードを取得します。
    # Get Node for Gain
    gain_node = nodemap_remote.get_node("Gain")

    # デジタルゲインを2倍（設定値128）に設定します。
    # Set digital gain to 2 times(128).
    gain_node.value = 128
    
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
