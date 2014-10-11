
set( BOOST_LIBRARYDIR "C:/projects/Boost/boost-1.49.0/boost/bin.v2/libs" )
set( BOOST_INCLUDEDIR "C:/projects/Boost/boost_1_49_0" )
set( LIBUSB_INC_PATH "${ROOT}/3dparty/libusb-1.0.12/include/libusbx-1.0" )
set( LIBUSB_LIB_PATH "${ROOT}/3dparty/libusb-1.0.12/ms32/static" )

set( QT_PATH "c:/programs/qt/qt-4.8.3-mingw" )
set( MINGW_PATH "c:/programs/qt-4.8.3-mingw" )
set( ENV{PATH} "${QT_PATH}/bin;${QT_PATH}/lib;${QT_PATH}/include;$ENV{PATH}" )
set( ENV{PATH} "${MINGW_PATH}/bin;${MINGW_PATH}/lib/bin;${MINGW_PATH}/include;$ENV{PATH}" )
set( ENV{QMAKESPEC} win32-msvc2008 )
