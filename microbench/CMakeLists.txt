cmake_minimum_required(VERSION 3.16)
project(setbench)

set(CMAKE_CXX_STANDARD 20)
set(SOURCE_EXE main.cpp)
set(CMAKE_CXX_FLAGS "-lpthread -ldl -mrtm")

include_directories(../.)
include_directories(../common)
#include_directories(../dragonfly_table)

#include_directories(../common)

set(SOURCE_EXE main.cpp)

#add_subdirectory(../dragonfly_table)

file(GLOB ADAPTERS ../ds_old/*/adapter.h)
string(REGEX MATCHALL "\/[^\/]+\/adapter.h" RAW_DATA_STRUCTURES ${ADAPTERS})

foreach(DATA_STRUCTURE ${RAW_DATA_STRUCTURES})
#    message(ADAPTER="${ADAPTER}")
    string(REPLACE "\/adapter.h" "" DS ${DATA_STRUCTURE})
    string(REPLACE "\/" "" DS ${DS})
    list(APPEND DATA_STRUCTURES ${DS})
endforeach()


#string(REPLACE "\/adapter.h" "" DATA_STRUCTURES ${DATA_STRUCTURES})
#string(REPLACE "\/" "" DATA_STRUCTURES ${DATA_STRUCTURES})

foreach(DS ${DATA_STRUCTURES})
#    message(DATA_STRUCTURE="${DS}")
    include_directories(../ds/${DS})

    add_executable(${DS} ${SOURCE_EXE})
endforeach()

#message(ADAPTERS="${ADAPTERS}")
#message(DATA_STRUCTURES="${DATA_STRUCTURES}")
#message(DATA_STRUCTURES_NEW="${DATA_STRUCTURES}")


#add_executable(main ${SOURCE_EXE})

#target_link_libraries(main PUBLIC dragonfly_static)







