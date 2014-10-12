
find_package( Qt4 REQUIRED )
set( QT_USE_QTGUI true )
set( QT_USE_QTSVG true )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${ROOT}/qextserialport/src )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
file( GLOB ui "./src/*.ui" )
message( INFO "SRC = ${src}" )
message( INFO "HEADERS = ${headers}" )

qt4_wrap_cpp( moc_src ${headers} )
set( src ${src} ${moc_src} )
qt4_wrap_ui( ui_src ${ui} )
set( src ${src} ${ui_src} )

add_executable( app ${src} )
if( WIN32 )
    set( defs _TTY_WIN_ )
else( WIN32 )
    set( defs _TTY_POSIX_ )
endif( WIN32 )
set_property( TARGET app PROPERTY COMPILE_DEFINITIONS ${defs} )

target_link_libraries( app ${QT_LIBRARIES} serial )









