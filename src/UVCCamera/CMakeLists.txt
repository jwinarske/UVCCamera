
set(SRC_FILES
        UVCCamera.cpp
        UVCPreview.cpp
        UVCButtonCallback.cpp
        UVCStatusCallback.cpp
        Parameters.cpp
        serenegiant_usb_UVCCamera.cpp
        )

find_package(PkgConfig REQUIRED)
pkg_check_modules(TURBO_JPEG REQUIRED libturbojpeg)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
pkg_check_modules(LIBUVC REQUIRED libuvc)
pkg_check_modules(RAPID_JSON REQUIRED RapidJSON)

add_library(UVCCamera SHARED ${SRC_FILES})

MESSAGE(STATUS "TURBO_JPEG_INCLUDE_DIRS: ${TURBO_JPEG_INCLUDEDIR}")
MESSAGE(STATUS "LIBUSB_INCLUDE_DIRS: ${LIBUSB_INCLUDEDIR}")
MESSAGE(STATUS "LIBUVC_INCLUDE_DIRS: ${LIBUVC_INCLUDEDIR}")
MESSAGE(STATUS "RAPID_JSON_INCLUDE_DIRS: ${RAPID_JSON_INCLUDEDIR}")

target_include_directories(UVCCamera PUBLIC
	..
	${TURBO_JPEG_INCLUDEDIR}
	${LIBUSB_INCLUDE_DIRS}
        ${LIBUVC_INCLUDEDIR}
        ${RAPID_JSON_INCLUDEDIR}
	)

target_compile_definitions(UVCCamera PUBLIC LOG_NDEBUG ACCESS_RAW_DESCRIPTORS)
target_compile_options(UVCCamera PUBLIC -Werror -fstrict-aliasing -fprefetch-loop-arrays)
target_link_libraries(UVCCamera ${DEPS_LINK_LIBRARIES} dl)
