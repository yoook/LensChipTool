LensChipTool
============

Configure the AF Confirm Chip for some camera systems from the computer

AF Confirm Chips are small chips that you can glue on an analog camera lens and that does some communication with the camera body and emulate an arbitrary lens. This unlocks some features in the camera that would be locked otherwise because the camera would assume there were no lens at acl (e.g. the AF confirm feature).
Some of this chips can be configured, mostly by taking a quite long series of quite special pictures.

This program shall help you to configure your AF Confirm Chip as it takes the required pictures (or what ever) for you. As it relies on libgphoto2, it could support all cameras and connection types that gphoto2 supports.

At the moment, this version is quite limited as it only:
- supports one type of AF Confirm Chips: the EMF chip, that is still sold on ebay and elsewhere
- is for Canon EOS cameras
- is command line only
- probably is Linux only (I haven't tried to compile elsewhere. Gphoto2 needs to be supported on the target platform)

So basic functionality is available for all that *I* need ;-) but feature requests are welcome and I will see what I can do
