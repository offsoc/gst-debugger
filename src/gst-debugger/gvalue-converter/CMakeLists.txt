add_library(gvalue-converter
	gvalue_base.cpp
	gvalue_boolean.cpp
	gvalue_caps.cpp
	gvalue_enum.cpp
	gvalue_numeric.cpp
	gvalue_string.cpp
)

target_link_libraries(gvalue-converter
	${GSTMM_LIBRARIES}
)