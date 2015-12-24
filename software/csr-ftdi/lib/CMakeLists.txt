
set( target "usbspi" )

add_definitions( -DCOMPAT_TIMER_MACROS )
add_definitions( -DENABLE_LEDS )

include_directories( "./src"
                     "${ROOT}/3dparty/libftdi-0.20/src")
                     
set( src "./src/basics.cpp" 
         "./src/compat.c" 
         "./src/dllmain.c" 
         "./src/logging.c"
         "./src/spi.c" 
         "./src/usbspi.dll.def" )
         
add_library( ${target} SHARED ${src} )
target_link_libraries( ${target} ftdi-static )



