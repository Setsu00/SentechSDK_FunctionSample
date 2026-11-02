"""
grab_Parameter_AcquisitionLineRate.py

Acquisition Line Rate: Set the camera line rate to 385 then acquire 20 images.
Note: A line-scan camera that provides the AcquisitionLineRate node is required.
"""

import stapipy as st

number_of_images_to_grab = 20

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # ラインレートを385に設定するデモ
    # Demonstration of setting the acquisition line rate to 385.

    # パラメータにアクセスするためのノードマップを取得
    # Get the NodeMap for accessing camera parameters.
    nodemap_remote = st_device.remote_port.nodemap

    # AcquisitionLineRateノードを取得し、ラインレートに385を設定
    # Get the AcquisitionLineRate node then set the line rate to 385.
    acquisition_line_rate = nodemap_remote.get_node("AcquisitionLineRate")
    acquisition_line_rate.value = 385

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
