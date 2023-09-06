cmake_minimum_required(VERSION 3.18)

project(TradingAggregator)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/Dependencies/websocketpp-0.8.2/cmake")
find_package(websocketpp CONFIG REQUIRED)
find_package(Threads REQUIRED)

##########################################Trading Common#######################################
add_library(TradingCommonLib
    TradingCommon/Symbol.cpp
    TradingCommon/Price.cpp
    TradingCommon/DataProviderMessage.cpp
    TradingCommon/StorageHttpClient.cpp
    TradingCommon/Utils.cpp
)

target_include_directories(TradingCommonLib PUBLIC "${CMAKE_SOURCE_DIR}/TradingCommon")

target_compile_features(TradingCommonLib PUBLIC cxx_std_14)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(TradingCommonLib PUBLIC /W3 /MP)
else()
    target_compile_options(TradingCommonLib PUBLIC -Wall)
endif()

target_link_libraries(TradingCommonLib PUBLIC Threads::Threads)

target_include_directories(TradingCommonLib PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/asio-1.28.0/include")
target_include_directories(TradingCommonLib PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/json/include")

###############################################################################################

##########################################Storage API##########################################
add_executable(StorageApiApp
    StorageApi/StorageHttpServer.cpp
    StorageApi/main.cpp
)

target_compile_features(StorageApiApp PUBLIC cxx_std_14)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(StorageApiApp PUBLIC /W3 /MP)
else()
    target_compile_options(StorageApiApp PUBLIC -Wall)
endif()

target_link_libraries(StorageApiApp PRIVATE TradingCommonLib websocketpp::websocketpp)

target_include_directories(StorageApiApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/asio-1.28.0/include")
target_include_directories(StorageApiApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/json/include")

set_target_properties(StorageApiApp PROPERTIES RUNTIME_OUTPUT_NAME "StorageApi")
###############################################################################################

##########################################Data Provider########################################
add_executable(DataProviderApp
    DataProvider/Ai.cpp
    DataProvider/DataProvider.cpp
    DataProvider/main.cpp
)

target_compile_features(DataProviderApp PUBLIC cxx_std_14)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(DataProviderApp PUBLIC /W3 /MP)
else()
    target_compile_options(DataProviderApp PUBLIC -Wall)
endif()

target_link_libraries(DataProviderApp PRIVATE TradingCommonLib websocketpp::websocketpp)

target_include_directories(DataProviderApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/asio-1.28.0/include")
target_include_directories(DataProviderApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/json/include")
target_compile_definitions(DataProviderApp PUBLIC "PROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\"")

set_target_properties(DataProviderApp PROPERTIES RUNTIME_OUTPUT_NAME "DataProvider")
###############################################################################################

#######################################Trading Aggregator######################################
add_executable(TradingAggregatorApp
    TradingAggregator/TradingAggregatorServer.cpp
    TradingAggregator/main.cpp
    TradingAggregator/ClientHandler.cpp
    TradingAggregator/EndPoint.cpp
)

target_compile_features(TradingAggregatorApp PUBLIC cxx_std_14)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(TradingAggregatorApp PUBLIC /W3 /MP)
else()
    target_compile_options(TradingAggregatorApp PUBLIC -Wall)
endif()

target_link_libraries(TradingAggregatorApp PRIVATE TradingCommonLib websocketpp::websocketpp)

target_include_directories(TradingAggregatorApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/asio-1.28.0/include")
target_include_directories(TradingAggregatorApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/json/include")

set_target_properties(TradingAggregatorApp PROPERTIES RUNTIME_OUTPUT_NAME "TradingAggregator")
###############################################################################################

#######################################Trading Broadcaster######################################
add_executable(TradingBroadcasterApp
    TradingBroadcaster/TradingBroadcasterServer.cpp
    TradingBroadcaster/main.cpp
    TradingBroadcaster/ClientHandler.cpp
    TradingBroadcaster/EndPoint.cpp
)

target_compile_features(TradingBroadcasterApp PUBLIC cxx_std_14)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(TradingBroadcasterApp PUBLIC /W3 /MP)
else()
    target_compile_options(TradingBroadcasterApp PUBLIC -Wall)
endif()

target_link_libraries(TradingBroadcasterApp PRIVATE TradingCommonLib websocketpp::websocketpp)

target_include_directories(TradingBroadcasterApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/asio-1.28.0/include")
target_include_directories(TradingBroadcasterApp PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/json/include")

set_target_properties(TradingBroadcasterApp PROPERTIES RUNTIME_OUTPUT_NAME "TradingBroadcaster")
###############################################################################################
