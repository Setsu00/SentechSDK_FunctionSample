"""
grab_Parameter_AcquisitionFrameRate.py

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostration of changing acquisition frame rate (FPS)

    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Switch off exposure auto.
    # If exposure auto is on, FPS setting may not able to implemented.
    exposure_auto = nodemap_remote.get_node("ExposureAuto")
    # Convert to IEnum type for operating
    enum_exposure_auto = st.PyIEnumeration(exposure_auto)
    # Set Exposure Auto to Off
    enum_exposure_auto.from_string("Off")

    # Also switch Exposure Mode to Off for not letting exposure time
    # to influence the actual FPS.
    exposure_mode = nodemap_remote.get_node("ExposureMode")
    # Convert to IEnum type for operating
    enum_exposure_mode = st.PyIEnumeration(exposure_mode)
    # Set Exposure Mode to Off
    enum_exposure_mode.from_string("Off")

    # Get Node for Acquisition Frame Rate (FPS)
    acq_frame_rate = nodemap_remote.get_node("AcquisitionFrameRate")
    # Set FPS to 2
    acq_frame_rate.value = 2
    
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
