"""
grab_device_reset.ph
"""

import stapipy as st
import time

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostrate how to reset device
    # (This sample code work within 1 camera environment)

    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Acquire Device Reset command
    device_reset = nodemap_remote.get_node("DeviceReset")
    cmd_device_reset = st.PyICommand(device_reset)
    print('Resetting Device...')
    cmd_device_reset.execute()

    # Wait for 30 sec. (This may varies depeneds on environment)
    print('Reset command send. Wait for 30 sec. for re-connect...')
    time.sleep(30)

    # After waiting, re-scan for camera then try to connect again
    # Be noticed the IP address of the camera must fixed to a value that is able to be connected again without setting
    print('Updating device list.')
    for index in range(st_system.interface_count):
        st_system.get_interface(index).update_device_list()

    # Release previous component before re-connect
    st_device.release()

    # Re-connect to a device
    st_device = st_system.create_first_device()
    print('Device=', st_device.info.display_name)

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
