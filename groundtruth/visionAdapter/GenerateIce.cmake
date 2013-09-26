##------------------------------------------------------------
## Author:		Gonzalo Abella (abellagonzalo@gmail.com)
## Update:		25/01/2012 by Gonzalo Abella
##------------------------------------------------------------

### Ice generation code begin
set (ICE_PATH ${PROJECT_SOURCE_DIR}/src)
set (ICE_FILES
	VisionAdapter
)
set (CPP_ICE)
set (H_ICE)

foreach (file ${ICE_FILES})
	set (cc_file ${file}.cpp)
	set (h_file ${file}.h)
	set (ice_file ${file}.ice)
	
	add_custom_command(OUTPUT ${cc_file} ${h_file}
			COMMAND slice2cpp --output-dir=${PROJECT_BINARY_DIR} -I${BICA_PATH} ${ICE_PATH}/${ice_file}
			DEPENDS ${ICE_PATH}/${ice_file}
			VERBATIM
	)
	
	list (APPEND CPP_ICE ${PROJECT_BINARY_DIR}/${cc_file})
	list (APPEND H_ICE ${PROJECT_BINARY_DIR}/${h_file})
endforeach()

add_custom_target(GenerateIce
	    DEPENDS ${CPP_ICE} ${H_ICE}
	    VERBATIM
	)
### Ice generation code end
