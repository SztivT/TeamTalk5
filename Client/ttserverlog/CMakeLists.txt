cmake_minimum_required(VERSION 3.1)
project (ttserverlog)

include_directories (../../Library/TeamTalk_DLL)
link_directories (../../Library/TeamTalk_DLL)

option (BUILD_TEAMTALK_CLIENT_LOG_EXAMPLE "Build ttserverlog for TeamTalk 5 Standard Edition" ON)
option (BUILD_TEAMTALKPRO_CLIENT_LOG_EXAMPLE "Build ttproserverlog for TeamTalk 5 Professional Edition" ON)

if (MSVC)
  add_definitions (-DUNICODE -D_UNICODE)
endif()

if (BUILD_TEAMTALK_CLIENT_LOG_EXAMPLE)
  add_executable (ttserverlog Main.cpp)
  set_property(TARGET ttserverlog PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../Library/TeamTalk_DLL")
  target_link_libraries (ttserverlog TeamTalk5)
endif()

if (BUILD_TEAMTALKPRO_CLIENT_LOG_EXAMPLE)
  add_executable (ttproserverlog Main.cpp)
  set_property(TARGET ttproserverlog PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../Library/TeamTalk_DLL")
  target_link_libraries (ttproserverlog TeamTalk5Pro)
endif()
