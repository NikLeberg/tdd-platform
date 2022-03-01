add_library(mongoose STATIC
    mongoose/mongoose.c
)
target_include_directories(mongoose PUBLIC ./mongoose .)
target_compile_definitions(mongoose PUBLIC
    MG_ARCH=MG_ARCH_CUSTOM
    MG_ENABLE_SSI=0
    MG_ENABLE_FILE=0
)
target_link_libraries(mongoose PRIVATE platform)
target_compile_options(mongoose PRIVATE -Wno-format)
