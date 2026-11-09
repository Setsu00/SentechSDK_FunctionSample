"""
Rotate90cwFilter: 取得画像を時計回りに90度回転します。
Rotate90cwFilter: Rotate acquired images 90 degrees clockwise.

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
    # 変換器を使用して取得画像を時計回りに90度回転するデモ。
    # Demonstration of rotating acquired images 90 degrees clockwise with a converter.
    
    # Reverse変換器を生成します。
    # Create a Reverse converter.
    reverse_converter = st.create_converter(st.EStConverterType.Reverse)
    
    # 回転モードをClockwise90に設定し、時計回りに90度回転させます。
    # Set the rotation mode to Clockwise90 to rotate images 90 degrees clockwise.
    reverse_converter.rotation_mode = st.EStRotationMode.Clockwise90

    # 生成した変換器を画像取得後に使用します。
    # Use the converter after acquiring each image.

    # ============================================================
    
    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()

                # 取得画像を時計回りに90度回転します。
                # Rotate the acquired image 90 degrees clockwise.
                rotated_image = reverse_converter.convert(st_image)

                # 回転後の画像情報を表示します。
                # Display information about the rotated image.
                print("BlockID={0} Size={1} x {2} First Byte={3}".format(
                      st_buffer.info.frame_id,
                      rotated_image.width, rotated_image.height,
                      rotated_image.get_image_data()[0]))
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

except Exception as exception:
    print(exception)
