"""
Rotate90cwFilter: using Filter to rotate image clock wise 90 degree.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    st_datastream = st_device.create_datastream()

    # ============================================================
    # Demostration of using Filter to rotate image clock wise 90 degree.
    
    # Create an ReverseConverter filter
    reverse_converter = st.create_converter(st.EStConverterType.Reverse)
    
    # Set the converter to rotate image close wise 90 degree
    reverse_converter.retation_mode = st.EStRotationMode.Clockwise90)

    # We will use the converter later when image is acquired

    # ============================================================
    
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
