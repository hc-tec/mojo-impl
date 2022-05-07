
project(tit_base)

add_library(tit_base
        mutex.h
        noncopyable.h
        atomic_int.h
        types.h
        process.h
        )

target_link_libraries(tit_base pthread rt)


install(TARGETS tit_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/tit/base)
