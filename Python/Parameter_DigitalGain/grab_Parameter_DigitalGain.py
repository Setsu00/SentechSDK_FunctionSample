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
    # Demostration of set digital Gain to 2 times.

    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Switch off gain auto.
    # If Gain auto is on, gain value cannot be set
    gain_auto = nodemap_remote.get_node("GainAuto")
    # Convert to IEnum type for operating
    enum_gain_auto = st.PyIEnumeration(gain_auto)
    # Set Gain Auto to Off
    enum_gain_auto.from_string("Off")

    # For setting analog gain, gain selector need to be set to Digital to access analog gain.
    gain_selector = nodemap_remote.get_node("GainSelector")
    # Convert to IEnum type for operating
    enum_gain_selector = st.PyIEnumeration(gain_selector)
    # Set selector to DigitalAll to control analog gain
    enum_gain_selector.from_string("DigitalAll")

    # Get Node for Gain
    gain_node = nodemap_remote.get_node("Gain")
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
