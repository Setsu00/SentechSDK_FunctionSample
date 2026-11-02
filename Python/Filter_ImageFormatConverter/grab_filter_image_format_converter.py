"""
grab_filter_image_format_converter.py

Image Format Converter: Convert acquired images to Mono8.
"""

import stapipy as st

number_of_images_to_grab = 100000

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    st_datastream = st_device.create_datastream()

    # ============================================================
    # ピクセルフォーマット変換器を生成し、変換先をMono8に設定するデモ
    # Demonstration of creating a pixel-format converter and setting the destination to Mono8.

    # PixelFormat変換器を生成
    # Create a PixelFormat converter.
    pixel_format_converter = st.create_converter(st.EStConverterType.PixelFormat)

    # 変換先（出力）のピクセルフォーマットをMono8に設定
    # Set the destination (output) pixel format to Mono8.
    pixel_format_converter.destination_pixel_format = st.EStPixelFormatNamingConvention.Mono8

    # ============================================================

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()

                # ============================================================
                # 元画像をMono8へ変換する。
                # Convert the original image to Mono8.
                converted_image = pixel_format_converter.convert(st_image)

                # 変換後の画像情報を表示する。
                # Display information about the converted image.
                print("BlockID={0} Size={1} x {2} First Byte={3} PixelFormat=Mono8".format(
                    st_buffer.info.frame_id,
                    converted_image.width, converted_image.height,
                    converted_image.get_image_data()[0]))
                # ============================================================
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

except Exception as exception:
    print(exception)
