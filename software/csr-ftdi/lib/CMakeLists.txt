
set( target "usbspi" )

add_definitions( -DCOMPAT_TIMER_MACROS )
add_definitions( -DENABLE_LEDS )
add_definitions( -DVERSION="0.5.0" )
add_definitions( -DGIT_REVISION="abcde" )

include_directories( "./src"
                     "${ROOT}/3dparty/libftdi-0.20/src" 
                     "${ROOT}/3dparty/libusb-win32-bin-1.2.6.0/include" )
                     
set( src "./src/basics.cpp" 
         "./src/compat.c" 
         "./src/dllmain.c" 
         "./src/logging.c"
         "./src/spi.c" 
         "./src/usbspi.dll.def" )
         
add_library( ${target} SHARED ${src} )
target_link_libraries( ${target} ftdi )



