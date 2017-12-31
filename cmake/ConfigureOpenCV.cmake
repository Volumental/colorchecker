find_package(OpenCV REQUIRED
	PATHS ${THIRDPARTY_INSTALL_PREFIX}
	"/opt/local/share/"      # Mac: port install opencv
	"/usr/local/opt/opencv@3" # Mac: brew install homebrew/science/opencv3
)
mark_as_advanced(OpenCV_DIR)
