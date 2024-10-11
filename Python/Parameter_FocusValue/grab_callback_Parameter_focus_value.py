"""
grab_callback_Parameter_focus_value.py

Demostrates for those module that having focus value for controlling optical focus via camera setting.

"""

import cv2
import threading
import numpy as np
import stapipy as st

# Global timestamp variable
g_timeStampRec = 0

# Image scale when displaying using OpenCV.
DISPLAY_RESIZE_FACTOR = 0.3

class CMyCallback:
    """
    Class that contains a callback function.
    """
    def __init__(self):
        self._image = None
        self._lock = threading.Lock()

    @property
    def image(self):
        duplicate = None
        self._lock.acquire()
        if self._image is not None:
            duplicate = self._image.copy()
        self._lock.release()
        return duplicate

    def datastream_callback(self, handle=None, context=None):
        """
        Callback to handle events from DataStream.

        :param handle: handle that trigger the callback.
        :param context: user data passed on during callback registration.
        """
        st_datastream = handle.module
        if st_datastream:
            with st_datastream.retrieve_buffer() as st_buffer:
                # Check if the acquired data contains image data.
                if st_buffer.info.is_image_present:
                    # Create an image object.
                    st_image = st_buffer.get_image()

                    # Check the pixelformat of the input image.
                    pixel_format = st_image.pixel_format
                    pixel_format_info = st.get_pixel_format_info(pixel_format)

                    # Only mono or bayer is processed.
                    if not(pixel_format_info.is_mono or
                           pixel_format_info.is_bayer):
                        return

                    # Get raw image data.
                    data = st_image.get_image_data()

                    # Perform pixel value scaling if each pixel component is
                    # larger than 8bit. Example: 10bit Bayer/Mono, 12bit, etc.
                    if pixel_format_info.each_component_total_bit_count > 8:
                        nparr = np.frombuffer(data, np.uint16)
                        division = pow(2, pixel_format_info
                                       .each_component_valid_bit_count - 8)
                        nparr = (nparr / division).astype('uint8')
                    else:
                        nparr = np.frombuffer(data, np.uint8)

                    # Process image for display.
                    nparr = nparr.reshape(st_image.height, st_image.width, 1)

                    # Perform color conversion for Bayer.
                    if pixel_format_info.is_bayer:
                        bayer_type = pixel_format_info.get_pixel_color_filter()
                        if bayer_type == st.EStPixelColorFilter.BayerRG:
                            nparr = cv2.cvtColor(nparr, cv2.COLOR_BAYER_RG2RGB)
                        elif bayer_type == st.EStPixelColorFilter.BayerGR:
                            nparr = cv2.cvtColor(nparr, cv2.COLOR_BAYER_GR2RGB)
                        elif bayer_type == st.EStPixelColorFilter.BayerGB:
                            nparr = cv2.cvtColor(nparr, cv2.COLOR_BAYER_GB2RGB)
                        elif bayer_type == st.EStPixelColorFilter.BayerBG:
                            nparr = cv2.cvtColor(nparr, cv2.COLOR_BAYER_BG2RGB)

                    # Resize image and store to self._image.
                    nparr = cv2.resize(nparr, None,
                                       fx=DISPLAY_RESIZE_FACTOR,
                                       fy=DISPLAY_RESIZE_FACTOR)
                    self._lock.acquire()
                    self._image = nparr
                    self._lock.release()

# For FocusValue inputing thread
def do_focus_adjust(nodemap, stop_event):
    focus_value = nodemap.get_node("FocusValue")
    if focus_value.is_readable:
        focus_value_int = st.PyIInteger(focus_value)
        min_focus = focus_value_int.min
        max_focus = focus_value_int.max

        while not stop_event.is_set():  # Check if stop event is set
           current_focus = focus_value.value
           print(f"FocusValue range: Min={min_focus}, Max={max_focus}")
           print(f"Current Value: {current_focus}")
           user_focus = int(input("Enter a FocusValue (99999 to exit): "))

           if user_focus == 99999:
               stop_event.set()  # Signal to stop the main loop
               break

           if min_focus <= user_focus <= max_focus:
               focus_value.value = user_focus
               print(f"FocusValue set to: {user_focus}")
           else:
               print(f"Invalid value. Please enter a value between {min_focus} and {max_focus}.")
    else:
        print("FocusValue is not available.")


# Main function
def main():
    my_callback = CMyCallback()
    cb_func = my_callback.datastream_callback
    stop_event = threading.Event()  # Create a stop event to signal stopping the loop

    try:
        st.initialize()
        st_system = st.create_system()
        st_device = st_system.create_first_device()

        print("Device: ", st_device.info.display_name)

        st_datastream = st_device.create_datastream()
        callback = st_datastream.register_callback(cb_func)

        # Access the NodeMap for parameters
        nodemap = st_device.remote_port.nodemap

        # Check if OpticControllerSelector exists
        optic_selector = nodemap.get_node("OpticControllerSelector")
        if optic_selector is not None:
            print("OpticControllerSelector found.")

            st_datastream.start_acquisition()
            st_device.acquisition_start()

            focus_adj_thread = threading.Thread(target=do_focus_adjust, args=(nodemap, stop_event))
            focus_adj_thread.start()

            # Display image using OpenCV.
            while not stop_event.is_set():  # Break the loop when stop_event is set
                output_image = my_callback.image
                if output_image is not None:
                    cv2.imshow('image', output_image)
                key_input = cv2.waitKey(1)
                if key_input == 27:  # Exit when 'ESC' key is pressed
                    stop_event.set()  # Also set stop_event if ESC is pressed
                    break

            focus_adj_thread.join()

            st_device.acquisition_stop()
            st_datastream.stop_acquisition()
            cv2.destroyAllWindows()

        else:
            print("This camera does not support focus adjustment.")

    except Exception as ex:
        print("Exception: ", str(ex))


if __name__ == "__main__":
    main()
