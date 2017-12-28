# Colorchecker color calibration
This repository contains code to calibrate colors in images by finding
a colorchecker checkerboard and use it as a reference for what the
true colors should be. The color correction is modeled as a linear
transformation of linear intensities (images are decoded from sRGB).
The reference colors are read from a secondary image containing a
reference colorchecker. Two reference images are provided in the
resources folder, they come from http://babelcolor.com/.

This code was originally developed at Volumental, www.volumental.com.

# License
This software is in the public domain. Where that dedication is not
recognized, you are granted a perpetual, irrevocable license to copy
and modify these files as you see fit.

This does not apply to files in the thirdparty folder.

# Usage
Build the code by running `scripts/build.sh`.
