cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 실행 파일 이름 설정
project(mfcApp
  LANGUAGES ASM C CXX
)

set(EXECUTABLE ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
#C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.40.33807\atlmfc\include\afxwin.h
# MFC를 사용하도록 설정
set(CMAKE_MFC_FLAG 2)

# 소스 파일 추가
add_executable(${EXECUTABLE} WIN32 main.cpp stdafx.cpp mfcApp.rc resource.h)

# List of include directories
target_include_directories(${EXECUTABLE} PRIVATE
  src/ 
  C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.30.30705/atlmfc/include
)
# Visual Studio에서 MFC 사용 설정
if (MSVC)
    target_compile_options(${EXECUTABLE} PRIVATE /utf-8 /Yustdafx.h)
    set_target_properties(${EXECUTABLE} PROPERTIES
        COMPILE_FLAGS "/Yustdafx.h"
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
    target_link_libraries(${EXECUTABLE} PRIVATE afxwin)
endif()

# 미리 컴파일된 헤더 설정
target_sources(${EXECUTABLE} PRIVATE stdafx.h)
