# ----- Find dependencies -----

# socket.io-cpp-client
if (MSVC OR 1)
    find_package(sioclient QUIET)
else()
    find_package(sioclient REQUIRED)
endif()
set(sioclinet_library ${SIOCLIENT_LIBRARY})
set(sioclient_includes ${SIOCLIENT_INCLUDE_DIR})
if (TARGET sioclient)
    set(sioclinet_library sioclient)
    set(sioclient_includes )
elseif(MSVC OR 1)
    #find_package(Boost COMPONENTS random REQUIRED)
    find_library(SIOCLIENT_LIBRARY_RELEASE sioclient PATH_SUFFIXES lib)
    find_library(SIOCLIENT_LIBRARY_DEBUG sioclient PATH_SUFFIXES debug/lib NO_DEFAULT_PATH)
    string(REPLACE "debug/" "" SIOCLIENT_LIBRARY_RELEASE ${SIOCLIENT_LIBRARY_RELEASE})
    find_path(SIOCLIENT_INCLUDE sio_client.h)
    message(STATUS "${SIOCLIENT_INCLUDE}  ${SIOCLIENT_LIBRARY_DEBUG} ${SIOCLIENT_LIBRARY_RELEASE}")
    set(sioclinet_library debug ${SIOCLIENT_LIBRARY_DEBUG} optimized ${SIOCLIENT_LIBRARY_RELEASE})
    set(sioclient_includes ${SIOCLIENT_INCLUDE})
endif()


# Protobuf
find_package(protobuf REQUIRED CONFIG)

if(NOT PROTOBUF_PROTOC_EXECUTABLE)
    #set(PROTOBUF_PROTOC_EXECUTABLE protobuf::libprotoc)
    #message(FATAL_ERROR "Could not find protoc executable (PROTOBUF_PROTOC_EXECUTABLE)")
endif()
message(STATUS "Found protoc executable: ${PROTOBUF_PROTOC_EXECUTABLE}")

# ----- Protobuf transpile -----

protobuf_generate(OUT_VAR MAP_PB_SOURCE MAP_PB_HEADER PROTOS protobuf/map_segment.proto)
if (NOT MSVC)
set_source_files_properties(${MAP_PB_HEADER} ${MAP_PB_SOURCE}
                            COMPILE_FLAGS -Wno-unused-variable)
endif()

# ----- Configure SocketPublisher library -----

add_library(socket_publisher
            SHARED
            data_serializer.h
            publisher.h
            socket_client.h
            data_serializer.cc
            publisher.cc
            socket_client.cc
            ${MAP_PB_SOURCE})
if (NOT MSVC)
set_source_files_properties(${MAP_PB_SOURCE} PROPERTIES
                            COMPILE_FLAGS -Wno-unused-parameter)
endif()

#set_target_properties(socket_publisher PROPERTIES
#                      OUTPUT_NAME socket_publisher
#                      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

target_include_directories(socket_publisher
                           PUBLIC
                           ${sioclient_includes}
                           ${PROTOBUF_INCLUDE_DIRS}
                           PRIVATE
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/protobuf)

target_link_libraries(socket_publisher
                      PUBLIC
                      ${PROJECT_NAME}
                      opencv_imgcodecs
                      ${sioclinet_library}
                      protobuf::libprotobuf
                      ${PROTOBUF_LIBRARIES})
if (MSVC)
    target_compile_options(socket_publisher PRIVATE /MP)
    set_target_properties(socket_publisher PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif()
# ----- Install configuration -----

#set(INSTALL_SOCKET_PUBLISHER OFF CACHE BOOL "Install SocketPublisher library")
#if(INSTALL_SOCKET_PUBLISHER)
    set(SOCKER_PUBLISHER_INCLUDE_INSTALL_DIR ${INCLUDES_DESTINATION}/socket_publisher)

    install(TARGETS socket_publisher
            EXPORT ${OPENVSLAM_TARGETS_EXPORT_NAME}
            RUNTIME DESTINATION ${RUNTIME_DESTINATION}
            LIBRARY DESTINATION ${LIBRARY_DESTINATION}
            ARCHIVE DESTINATION ${ARCHIVE_DESTINATION}
            INCLUDES DESTINATION ${SOCKER_PUBLISHER_INCLUDE_INSTALL_DIR})

    file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${HEADERS}
            DESTINATION ${SOCKER_PUBLISHER_INCLUDE_INSTALL_DIR})
#endif()
