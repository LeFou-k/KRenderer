cmake_minimum_required (VERSION 3.2)
project(nori)

FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_subdirectory(ext ext_build)

include_directories(
  # Nori include files
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  # tinyformat string formatting library
  ${TFM_INCLUDE_DIR}
  # Eigen linear algebra library
  SYSTEM ${EIGEN_INCLUDE_DIR}
  # OpenEXR high dynamic range bitmap library
  SYSTEM ${OPENEXR_INCLUDE_DIRS}
  # Intel Thread Building Blocks
  SYSTEM ${TBB_INCLUDE_DIR}
  # Pseudorandom number generator
  ${PCG32_INCLUDE_DIR}
  # PugiXML parser
  ${PUGIXML_INCLUDE_DIR}
  # Helper functions for statistical hypothesis tests
  ${HYPOTHESIS_INCLUDE_DIR}
  # GLFW library for OpenGL context creation
  SYSTEM ${GLFW_INCLUDE_DIR}
  # GLEW library for accessing OpenGL functions
  SYSTEM ${GLEW_INCLUDE_DIR}
  # NanoVG drawing library
  SYSTEM ${NANOVG_INCLUDE_DIR}
  # NanoGUI user interface library
  SYSTEM ${NANOGUI_INCLUDE_DIR}
  SYSTEM ${NANOGUI_EXTRA_INCS}
  # Portable filesystem API
  SYSTEM ${FILESYSTEM_INCLUDE_DIR}
  # STB Image Write
  SYSTEM ${STB_IMAGE_WRITE_INCLUDE_DIR}
)

# The following lines build the main executable. If you add a source
# code file to Nori, be sure to include it in this list.

add_executable(nori

        # Header files
        include/nori/bbox.h
        include/nori/bitmap.h
        include/nori/block.h
        include/nori/bsdf.h
        include/nori/accel.h
        include/nori/camera.h
        include/nori/color.h
        include/nori/common.h
        include/nori/dpdf.h
        include/nori/frame.h
        include/nori/integrator.h
        include/nori/emitter.h
        include/nori/mesh.h
        include/nori/object.h
        include/nori/parser.h
        include/nori/proplist.h
        include/nori/ray.h
        include/nori/rfilter.h
        include/nori/sampler.h
        include/nori/scene.h
        include/nori/timer.h
        include/nori/transform.h
        include/nori/vector.h
        include/nori/warp.h

        # Source code files
        src/bitmap.cpp
        src/block.cpp
        src/Accels/accel.cpp
        src/Tests/chi2test.cpp
        src/common.cpp
        src/gui.cpp
        src/Sampler/independent.cpp
        src/main.cpp
        src/mesh.cpp
        src/obj.cpp
        src/object.cpp
        src/parser.cpp
        src/perspective.cpp
        src/proplist.cpp
        src/rfilter.cpp
        src/scene.cpp
        src/Tests/ttest.cpp
        src/Sampler/warp.cpp
        src/Lights/AreaLight.cpp

        #BSDFs
        src/BSDFS/diffuse.cpp
        src/BSDFS/microfacet.cpp
        src/BSDFS/mirror.cpp
        src/BSDFS/dielectric.cpp

        #Integrators
        src/Integrators/AOIntegrator.cpp
        src/Integrators/NormalIntegrator.cpp
        src/Integrators/PathEms.cpp
        src/Integrators/PathMats.cpp
        src/Integrators/PathMis.cpp
        src/Integrators/SimpleIntegrator.cpp
        src/Integrators/WhittedIntegrator.cpp
  )


add_definitions(${NANOGUI_EXTRA_DEFS})

# The following lines build the warping test application
add_executable(warptest
  include/nori/warp.h
  src/Sampler/warp.cpp
  src/Tests/warptest.cpp
  src/BSDFS/microfacet.cpp
  src/object.cpp
  src/proplist.cpp
  src/common.cpp
 )

if (WIN32)
  target_link_libraries(nori tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS} zlibstatic)
else()
  target_link_libraries(nori tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS})
endif()

target_link_libraries(warptest tbb_static nanogui ${NANOGUI_EXTRA_LIBS})

# Force colored output for the ninja generator
if (CMAKE_GENERATOR STREQUAL "Ninja")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
  endif()
endif()

target_compile_features(warptest PRIVATE cxx_std_17)
target_compile_features(nori PRIVATE cxx_std_17)

# vim: set et ts=2 sw=2 ft=cmake nospell:
