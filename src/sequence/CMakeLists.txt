add_library(sequence sequence.c arraylist.c bit.c element.c guid.c)

target_include_directories(sequence PUBLIC .)
set_target_properties(sequence PROPERTIES PUBLIC_HEADER "arraylist.h;element.h;guid.h;sequence.h")

set_target_properties(sequence PROPERTIES SOVERSION 0.1)

install(TARGETS sequence
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

