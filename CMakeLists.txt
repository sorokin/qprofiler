cmake_minimum_required(VERSION 2.8)
project(qprofiler)

add_subdirectory(linux-3.14.29)

set(QT_USE_QTCORE TRUE)
set(QT_USE_QTGUI TRUE)
find_package(Qt4 4.6 REQUIRED)
include(${QT_USE_FILE})

qt4_wrap_ui(UI_FILES mainwindow.ui)
qt4_wrap_cpp(MOC_FILES mainwindow.h myitemdelegate.h)

set(CMAKE_CXX_FLAGS         "-std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

include_directories(. ${perf_INCLUDE_DIR})
add_definitions(${perf_DEFINES})

add_executable(
    qprofiler
    ${UI_FILES}
    ${MOC_FILES}
    benchmark_mode.h
    main.cpp
    mainwindow.h
    my_context.h
    myitemdelegate.cpp
    my_item.h
    profile.h
    call_tree_column.h
    mainwindow.cpp
    my_context.cpp
    my_item.cpp
    myitemdelegate.h
    profile.cpp
    profile_perfdata.cpp
    string_pool.h
    string_pool.cpp
    string_ref.h
    string_ref.cpp
    object_pool.h
    object_pool.cpp
    tuple_hash.h
    tuple_equals.h
    type_list.h
    transformation.h
    transformation.cpp
    demangle/demangle.h
    demangle/demangle.cpp
)

add_executable(
    demangle_test
    string_ref.h
    string_ref.cpp
    demangle/demangle.h
    demangle/demangle.cpp
    demangle/demangle_test.cpp
)

add_executable(
    demangle_app
    string_ref.h
    string_ref.cpp
    demangle/demangle.h
    demangle/demangle.cpp
    demangle/demangle_app.cpp
)

target_link_libraries(
    qprofiler
    ${QT_LIBRARIES}
    perf-util
    -ldw -lslang -liberty -lunwind -lunwind-x86_64 -lpthread -ldl -lm
)
