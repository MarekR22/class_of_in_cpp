
find_package(Catch2 REQUIRED)

add_executable(poly_factory_test)

target_link_libraries(poly_factory_test
    PRIVATE
        Catch2::Catch2
        poly_factory)

target_sources(poly_factory_test
    PRIVATE
        main.cpp
        poly_factory_test.cpp
    )

