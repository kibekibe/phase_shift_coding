cmake_minimum_required (VERSION 2.8)
project (PhaseShift)


add_executable(
    PhaseShift
    src/main_decode_phase_shift.cpp
    src/imgUtil.cpp
    src/imgUtil.h
    src/PhaseShift.cpp
    src/PhaseShift.h
)

find_package(OpenCV REQUIRED)


if(OpenCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS})
  target_link_libraries(PhaseShift ${OpenCV_LIBS})
endif(OpenCV_FOUND)

