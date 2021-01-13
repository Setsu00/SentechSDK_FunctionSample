"""
Output Exposure Active: Set camera Line1 as output for sensor exposure active signal(period of exposuring of sensor).

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostration of Setting Line1 as Exposure Active

    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Set Line1 to output
    line_selector = nodemap_remote.get_node("LineSelector")
    enum_line_selector = st.PyIEnumeration(line_selector)
    enum_line_selector.from_string("Line1")
    
    line_mode = nodemap_remote.get_node("LineMode")
    enum_line_mode = st.PyIEnumeration(line_mode)
    enum_line_mode.from_string("Output")
    
    # Set LineSource to Exposure Active
    line_source = nodemap_remote.get_node("LineSource")
    enum_line_source = st.PyIEnumeration(line_source)
    enum_line_source.from_string("ExposureActive")
    
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
