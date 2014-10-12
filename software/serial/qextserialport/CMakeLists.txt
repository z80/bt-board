# Поиск и присоединение заголовков Qt.
find_package( Qt4 )
include( ${QT_USE_FILE} )

# Собственные поддиректории заголовочных файлов.
include_directories( "src" )

# Файлы исходного кода.
list( APPEND com_src "./src/qextserialbase.cpp" )
list( APPEND com_src "./src/qextserialport.cpp" )
if( WIN32 )
    list( APPEND com_src "./src/win_qextserialport.cpp" )
    set( defs _TTY_WIN_ )
else( WIN32 )
    list( APPEND com_src "./src/posix_qextserialport.cpp" )
    set( defs _TTY_POSIX_ )
endif( WIN32 )

# Статическая, поскольку хотелось бы только в одном месте использовать.
add_library( serial STATIC ${com_src} )
target_link_libraries( serial ${QT_LIBRARIES} )
# Символы для импорта динамических библиотек.
set_property( TARGET serial PROPERTY DEFINE_SYMBOL ${defs} )
set_property( TARGET serial PROPERTY COMPILE_DEFINITIONS ${defs} )

message( STATUS "configuring serial" )
