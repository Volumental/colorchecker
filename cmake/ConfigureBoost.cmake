option(USE_SYSTEM_BOOST "Whether to use the system installed boost" ON)

if (NOT USE_SYSTEM_BOOST)
	set(Boost_NO_SYSTEM_PATHS on)
	set(BOOST_ROOT ${THIRDPARTY_INSTALL_PREFIX})
endif()

find_package(Boost REQUIRED filesystem regex system thread timer unit_test_framework)
if (NOT APPLE)
	find_package(Boost REQUIRED python)
endif()

add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
