add_contract(otcfeesplit otcfeesplit
   ${CMAKE_CURRENT_SOURCE_DIR}/src/otcfeesplit.cpp
)

target_include_directories(otcfeesplit
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(otcfeesplit
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/otcfeesplit.clauses.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/otcfeesplit.clauses.md @ONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/otcfeesplit.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/otcfeesplit.contracts.md @ONLY )

target_compile_options( otcfeesplit PUBLIC -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )