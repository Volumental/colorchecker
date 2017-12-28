find_package(OpenCV REQUIRED
	PATHS ${THIRDPARTY_INSTALL_PREFIX}
	"/opt/local/share/"      # Mac: port install opencv
	"/usr/local/opt/opencv@3" # Mac: brew install homebrew/science/opencv3
	NO_CMAKE_SYSTEM_PATH
	NO_CMAKE_PATH
	NO_DEFAULT_PATH REQUIRED
)
mark_as_advanced(OpenCV_DIR)
