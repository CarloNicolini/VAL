project(Val)
cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Display command line run by cmake
if (NOT CMAKE_VERBOSE_MAKEFILE)
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

SET(SUPPORT_EXTERNALS False)

# Installation configuration
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
if( APPLE )
  set(CMAKE_MACOSX_RPATH TRUE)
  set(CMAKE_INSTALL_RPATH "@executable_path")
else()
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)



set(VAL_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(VAL_INCLUDE_DIR ${VAL_SRC_DIR})

include_directories(${VAL_INCLUDE_DIR})
#add_subdirectory(${VAL_INCLUDE_DIR}/parser)

# Set output folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_COMPILER_IS_GNUCC)
  # needed for gcc 4.6+
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fpermissive")
endif()

# Enable/Disable external solver feature through dl library
if (NOT SUPPORT_EXTERNALS)
  set(SUPPORT_EXTERNALS False)
endif()

# VAL
set(LIB_VAL_SOURCE_FILES
    ${VAL_SRC_DIR}/parser/pddl+.cpp
    ${VAL_SRC_DIR}/AccumulatedDelta.cpp
    ${VAL_SRC_DIR}/Action.cpp
    ${VAL_SRC_DIR}/CausalGraph.cpp
    ${VAL_SRC_DIR}/DebugWriteController.cpp
    ${VAL_SRC_DIR}/Environment.cpp
    ${VAL_SRC_DIR}/Events.cpp
    ${VAL_SRC_DIR}/FastEnvironment.cpp
    ${VAL_SRC_DIR}/FuncAnalysis.cpp
    ${VAL_SRC_DIR}/FuncExp.cpp
    ${VAL_SRC_DIR}/HowAnalyser.cpp
    ${VAL_SRC_DIR}/instantiation.cpp
    ${VAL_SRC_DIR}/LaTeXSupport.cpp
    ${VAL_SRC_DIR}/LibLink.cpp
    ${VAL_SRC_DIR}/Ownership.cpp
    ${VAL_SRC_DIR}/PinguPlanGenerator.cpp
    ${VAL_SRC_DIR}/Plan.cpp
    ${VAL_SRC_DIR}/Polynomial.cpp
    ${VAL_SRC_DIR}/PrettyPrinter.cpp
    ${VAL_SRC_DIR}/Proposition.cpp
    ${VAL_SRC_DIR}/ptree.cpp
    ${VAL_SRC_DIR}/random.cpp
    ${VAL_SRC_DIR}/RelaxTranslator.cpp
    ${VAL_SRC_DIR}/RepairAdvice.cpp
    ${VAL_SRC_DIR}/RobustAnalyse.cpp
    ${VAL_SRC_DIR}/SASActions.cpp
    ${VAL_SRC_DIR}/SimpleEval.cpp
    ${VAL_SRC_DIR}/State.cpp
    ${VAL_SRC_DIR}/TIM.cpp
    ${VAL_SRC_DIR}/TimSupport.cpp
    ${VAL_SRC_DIR}/ToFunction.cpp
    ${VAL_SRC_DIR}/TrajectoryConstraints.cpp
    ${VAL_SRC_DIR}/typecheck.cpp
    ${VAL_SRC_DIR}/TypedAnalyser.cpp
    ${VAL_SRC_DIR}/Utils.cpp
    ${VAL_SRC_DIR}/Validator.cpp
    ${VAL_SRC_DIR}/valLib.cpp)

    add_library(VAL SHARED ${LIB_VAL_SOURCE_FILES})
if( CMAKE_COMPILER_IS_GNUCC )
    target_compile_options(VAL PUBLIC "-DVAL_EXPORTS")
endif()
# Add source files of parser in the Debug area for debugging.
if ( ${CMAKE_BUILD_TYPE} MATCHES Debug )
    add_custom_command(TARGET VAL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${VAL_SRC_DIR}/parser/pddl+.y ${VAL_SRC_DIR}/parser/pddl+.cpp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()

# Support of external solvers feature
if( ${SUPPORT_EXTERNALS} )
    target_compile_options(VAL PUBLIC "-DSUPPORTEXTERNALS")
    target_link_libraries(VAL dl)
endif()

# Analyse
add_executable(analyse ${VAL_SRC_DIR}/analyze.cpp)
target_link_libraries(analyse VAL)

# DomainView
add_executable(domainview ${VAL_SRC_DIR}/domainview.cpp)
target_link_libraries(domainview VAL)

# HowWhatWhen
add_executable(howwhatwhen ${VAL_SRC_DIR}/howwhatwhen.cpp)
target_link_libraries(howwhatwhen VAL)

# Instantiate
add_executable(instantiate ${VAL_SRC_DIR}/instantiate.cpp)
target_link_libraries(instantiate VAL)

# Instantiation IP # TODO: Fix this because not compiling
# add_executable(instantiationip ${VAL_SRC_DIR}/instantiationip.cpp)
# target_link_libraries(instantiationip VAL)

# parser
add_executable(parser ${VAL_SRC_DIR}/parser.cpp)
target_link_libraries(parser VAL)

# PinguPlan
add_executable(pinguplan ${VAL_SRC_DIR}/pinguplan.cpp)
target_link_libraries(pinguplan VAL)

# PlanRec
add_executable(planrec ${VAL_SRC_DIR}/planrec.cpp)
target_link_libraries(planrec VAL)

# PlanSeqStep
add_executable(planseqstep ${VAL_SRC_DIR}/planseqstep.cpp)
target_link_libraries(planseqstep VAL)

# PlanToValStep
add_executable(plantovalstep ${VAL_SRC_DIR}/plantovalstep.cpp)
target_link_libraries(plantovalstep VAL)

# Relax
add_executable(relax ${VAL_SRC_DIR}/relax.cpp)
target_link_libraries(relax VAL)

# TIM
add_executable(tim-main ${VAL_SRC_DIR}/tim-main.cpp)
target_link_libraries(tim-main VAL)

# ToFn
add_executable(tofn ${VAL_SRC_DIR}/tofn.cpp)
target_link_libraries(tofn VAL)

# TypeAnalysis
add_executable(typeanalysis ${VAL_SRC_DIR}/typeanalysis.cpp)
target_link_libraries(typeanalysis VAL)

# Validate
add_executable(validate ${VAL_SRC_DIR}/validate.cpp)
target_link_libraries(validate VAL)

# ValStep
add_executable(valstep ${VAL_SRC_DIR}/valstep.cpp)
target_link_libraries(valstep VAL)

# ValueSeq
add_executable(valueseq ${VAL_SRC_DIR}/valueseq.cpp)
target_link_libraries(valueseq VAL)

# Targets to be installed
install(
    TARGETS VAL analyse domainview howwhatwhen instantiate parser pinguplan planrec planseqstep plantovalstep relax tim-main tofn typeanalysis validate valstep valueseq
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)

install(
    FILES ${APPLICATIONS_DIR}/README.md
    DESTINATION .
)

# Configuration of CPack
include (InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "ZIP")
SET(CPACK_SOURCE_GENERATOR "ZIP")
set (CPACK_PACKAGE_VERSION "$ENV{BUILD_BUILDNUMBER}")
include (CPack)