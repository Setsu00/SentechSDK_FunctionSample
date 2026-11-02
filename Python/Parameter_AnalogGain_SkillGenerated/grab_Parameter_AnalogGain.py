"""
    アナログゲインを2 dBに設定します。
    Set analog gain to 2 dB.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # アナログゲインを2 dBに設定します。
    # Set analog gain to 2 dB.

    # リモートNodeMapを取得します。 
    # Acquire the remote NodeMap.
    nodemap_remote = st_device.remote_port.nodemap

    # GainAutoノードを取得し、Offに設定します。
    # Get the GainAuto node and set it to Off.
    gain_auto = nodemap_remote.get_node("GainAuto")
    enum_gain_auto = st.PyIEnumeration(gain_auto)
    enum_gain_auto.from_string("Off")

    # GainSelectorノードを取得し、AnalogAllに設定します。
    # Get the GainSelector node and set it to AnalogAll.
    gain_selector = nodemap_remote.get_node("GainSelector")
    enum_gain_selector = st.PyIEnumeration(gain_selector)
    enum_gain_selector.from_string("AnalogAll")

    # Gainノードを取得してPyIFloatに変換します。
    # Get the Gain node and convert it to PyIFloat.
    gain = nodemap_remote.get_node("Gain")
    ifloat_gain = st.PyIFloat(gain)

    # Gainに2 dBを設定します。
    # Set Gain to 2 dB.
    ifloat_gain.value = 2.0

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
