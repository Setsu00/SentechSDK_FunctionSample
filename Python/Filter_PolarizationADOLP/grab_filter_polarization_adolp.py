"""
    PixelFormatConverterで取得画像をADOLP画像へ変換して表示します。
    Convert acquired images to ADOLP images with PixelFormatConverter and display them.

    偏光画像を出力するカメラ、stapipy、numpy、opencv-pythonが必要です。
    A camera that outputs polarization images, stapipy, numpy, and opencv-python are required.
"""

import cv2
import numpy as np
import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    st_datastream = st_device.create_datastream()

    # ============================================================
    # PixelFormatConverterでADOLP変換を設定します。
    # Configure ADOLP conversion with PixelFormatConverter.

    # PixelFormat変換器を生成します。
    # Create a PixelFormat converter.
    pixel_format_converter = st.create_converter(st.EStConverterType.PixelFormat)

    # OpenCV表示用に変換先をBGR8に設定します。
    # Set the destination to BGR8 for OpenCV display.
    pixel_format_converter.destination_pixel_format = st.EStPixelFormatNamingConvention.BGR8

    # 変換器のNodeMapを取得します。
    # Acquire the converter's NodeMap.
    nodemap_converter = pixel_format_converter.nodemap

    # 偏光画像変換方式を直線偏光の角度と偏光度（ADOLP）に設定します。
    # Set the polarization image transformation method to angle and degree of linear polarization (ADOLP).
    polarization_method = nodemap_converter.get_node("PolarizationImageTransformationMethod")
    enum_polarization_method = st.PyIEnumeration(polarization_method)
    enum_polarization_method.from_string("AngleAndDegreeOfLinearPolarization")

    # ============================================================

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()

                # ============================================================
                # 表示前に、取得した偏光画像をADOLP画像へ変換します。
                # Convert the acquired polarization image to an ADOLP image before display.
                converted_image = pixel_format_converter.convert(st_image)

                # 変換後のBGR8画像をOpenCVで表示します。
                # Display the converted BGR8 image with OpenCV.
                data = converted_image.get_image_data()
                nparr = np.frombuffer(data, np.uint8)
                nparr = nparr.reshape(converted_image.height, converted_image.width, 3)
                cv2.imshow('ADOLP', nparr)

                # Escキーで表示を終了します。
                # Press Esc to end the display.
                if cv2.waitKey(1) == 27:
                    break
                # ============================================================
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

    cv2.destroyAllWindows()

except Exception as exception:
    print(exception)
