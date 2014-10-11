
find_package( Qt4 )
set( QT_USE_QTCORE    True )
set( QT_USE_QTGUI     True )
#set( QT_USE_QTUITOOLS True )
#set( QT_USE_QTNETWORK True )
#set( QT_USE_QTWEBKIT  True )
if ( WIN32 )
    set( QT_USE_QTMAIN True )
endif ( WIN32 )
include( ${QT_USE_FILE} )

set( Boost_USE_STATIC_LIBS True )
set( Boost_USE_MULTITHREADED True )
find_package( Boost COMPONENTS thread filesystem REQUIRED )
find_package( Threads )

include( ${ROOT}/3dparty/cmake/FindLIBVLC.cmake )

include_directories( "./src" 
                     "./src/analogpad" 
                     "${ROOT}/peer_abst/src" 
                     "${ROOT}/peer_qxmpp/src" 
                     "${ROOT}/luajoyctrl/src"
                     "${ROOT}/3dparty/lua-5.1.5/src"
                     "${ROOT}/3dparty/lua-5.1.5/etc"  
                     "${ROOT}/3dparty/qtlua-1.4/src" 
                     "${ROOT}/3dparty/qwt-6.0.1/src" 
                     "${ROOT}/3dparty/widgets/src" 
                     ${Boost_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_BINARY_DIR} )
if ( WIN32 )
    include_directories( ${ROOT}/3dparty/vlc/sdk/include )
endif ( WIN32 )

file( GLOB src      "./src/*.cpp" )
file( GLOB headers  "./src/*.h" )
file( GLOB ui       "./src/*.ui" )
file( GLOB qrc      "./src/*.qrc" )
file( GLOB asrc     "./src/analogpad/*.cpp" )
set( src ${src} ${asrc} )
file( GLOB aheaders "./src/analogpad/*.h" )
set( headers ${headers} ${aheaders} )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

Qt4_WRAP_CPP( mocSrc ${headers} )
set( src ${src} ${mocSrc} )

QT4_WRAP_UI( uiSrc ${ui} )
set( src ${src} ${uiSrc} )

QT4_ADD_RESOURCES( qrcSrc ${qrc} )
set( src ${src} ${qrcSrc} )

if ( WIN32 )
    set( src ${src} "./src/icon.rc" )
    set( BUILD_TYPE WIN32 )
else ( WIN32 )
    set( BUILD_TYPE "" )
endif ( WIN32 )

add_executable( app_client ${BUILD_TYPE} ${src} )
set( libs peer_qxmpp liblua-5.1.5 luajoyctrl qtlua widgets )
if ( WIN32 )
    set( libs ${libs} libvlc.lib )
elseif ( UNIX )
    set( libs ${libs} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
endif ( WIN32 )
target_link_libraries( app_client ${libs}
                                  ${CMAKE_THREAD_LIBS_INIT}
                                  ${Boost_LIBRARIES}
                                  ${QT_LIBRARIES} )
install( TARGETS app_client DESTINATION "." )
install( DIRECTORY "./src_lua/" DESTINATION "." )
install( DIRECTORY "./help" DESTINATION "." )

