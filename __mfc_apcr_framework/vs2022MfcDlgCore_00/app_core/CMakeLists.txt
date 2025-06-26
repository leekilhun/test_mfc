cmake_minimum_required(VERSION 3.12)

set(PRJ_NAME apcr_app)

project(${PRJ_NAME}
  LANGUAGES ASM C CXX
)

set(EXECUTABLE ${PROJECT_NAME})

# 해당 폴더를 포함해서 하위 폴더까지의 파일도 찾는다.
file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  ap/*.cpp
  lib/*.cpp
  util/*.cpp
)

# Build the executable based on the source files
add_executable(${EXECUTABLE}  
  ${SRC_FILES_RECURSE}  
  core.cpp
  apcr_main.cpp
  )

 
target_compile_definitions(${EXECUTABLE} PRIVATE
-UNICODE
-DDEBUG
) 

# C++20 표준을 사용합니다.
target_compile_features(${EXECUTABLE} PUBLIC cxx_std_20)


#${CMAKE_SOURCE_DIR}/
# List of include directories
target_include_directories(${EXECUTABLE} PRIVATE   
  .
  ap
  ap/_inc
  ap/_inc/interface
  ap/_inc/hw
  util
  
  lib
  lib/MMC/MotionSDK_64bit
  mcc_lib

  )

#Compiler options
target_compile_options(${EXECUTABLE} PRIVATE  
  /EHsc
  /std:c++20
  /Od
  /MP
  #/W4  # 모든 경고 수준을 나타냅니다.
  #/WX  # 경고를 오류로 처리합니다.
  )

# Windows 플랫폼을 대상으로 할 때와 다른 플랫폼을 대상으로 할 때를 구분하여 컴파일러 옵션을 설정해야 할 수 있습니다.
# 예를 들어, GCC나 Clang을 사용하는 경우 아래와 같은 조건문을 사용할 수 있습니다.
if(NOT MSVC)
  target_compile_options(${EXECUTABLE} PRIVATE
    -Wall -Wextra -pedantic -Werror
  )
endif()

# Linker options
target_link_libraries(${EXECUTABLE} PRIVATE
  #ws2_32.lib  # 추가된 부분
  # 여기에 필요한 라이브러리 링크 옵션을 추가합니다.
  # 예: pthread, m (수학 라이브러리), 여기에 추가적인 라이브러리
)