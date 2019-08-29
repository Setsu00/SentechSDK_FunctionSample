# Sentech SDK Single Function Samples
These sample codes are for Sentech SDK, the SDK from OMRON SENTECH for GigE Vision, USB3 Vision, and CoaXPress cameras of OMRON SENTECH.

Each sample code is for dedicated function of camera for a quick understanding of adjusting parameters obeying SFNC from GenICam.

For more detail of Sentech SDK, please visit https://sentech.co.jp/ for further information.

Note: For all the samples provide fixed value of parameter, they may not able to work with all modules of cameras and may need to modifiy the value of setting.


* GigE Vision Cameras of OMRON SENTECH

There are 2 versions of GigE cameras made by OMRON SENTECH:

 IEA series: 35mm x 35mm GigE cameras with module name initialed by "S"
 
  ex: STC-SCS241POE
  
 KWA series: 28mm x 28mm GigE cameras with module name initialed by "M"
 
  ex: STC-MCS500POE

Sentech SDK works with these 2 modules of cameras, but some naming of the IEA series cameras may differ from KWA series and need to be handled in program.

* USB3 Vision Cameras of OMRON SENTECH

USB3 Vision (U3V) cameras does not indcluded MB/MCA5MUSB3, which are not U3V standard based USB 3.0 cameras.

* CoaXPress Cameras of OMRON SENTECH

For CoaXPress cameras, it related to the grabber of CoaXPress with its GenTL producer.
