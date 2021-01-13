"""
    grab_Connection_DeviceID.py
    This sample shows how to connect to a camera with using its device ID.
"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    # ============================================================
    # Prepear PyStDevice for connection
    st_device = None
    
    # Acquire target device ID
    tgt_device_id = input("Input target device ID: ")

    # Check all interfaces with try to connect a device with indicated device ID
    # If not success it will go to exception handling
    for index in range(st_system.interface_count):
        st_interface = st_system.get_interface(index)
        try:
            st_device = st_interface.create_device_by_id(tgt_device_id)
            if st_device != None:
                break
        except:
            # We do nothing here if not success to connect to target device
            # This means no device has the same device ID
            pass

    # Finally check if camera found and connected
    # If not, force quit
    if st_device == None:
        print("Target camera not found.")
        exit()
    
    # ============================================================
    
    print('Device=', st_device.info.display_name)

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
