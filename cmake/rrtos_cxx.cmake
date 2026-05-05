include(FetchContent)

set(RRTOS_ETL_SOURCE_DIR "" CACHE PATH "Optional local ETLCPP checkout")
set(RRTOS_ETL_GIT_REPOSITORY "https://github.com/mingshi2333/etl" CACHE STRING "ETLCPP repository")
set(RRTOS_ETL_GIT_TAG "293c7dfcfc9582cf955a4d72264f92db4bba3c8e" CACHE STRING "Pinned ETLCPP commit")

set(NO_STL ON CACHE BOOL "Build ETL without STL assumptions" FORCE)
set(NO_SYSTEM_INCLUDE ON CACHE BOOL "Keep ETL includes as normal includes for freestanding builds" FORCE)

if(RRTOS_ETL_SOURCE_DIR)
    add_subdirectory("${RRTOS_ETL_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/etl-build" EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(
        etl
        GIT_REPOSITORY ${RRTOS_ETL_GIT_REPOSITORY}
        GIT_TAG ${RRTOS_ETL_GIT_TAG}
    )
    FetchContent_MakeAvailable(etl)
endif()

add_subdirectory(cxx)
