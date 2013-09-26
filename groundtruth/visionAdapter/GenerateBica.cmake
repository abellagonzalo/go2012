##------------------------------------------------------------
## Author:		Gonzalo Abella (abellagonzalo@gmail.com)
## Update:		25/01/2012 by Gonzalo Abella
##------------------------------------------------------------

### BICA generation code begin
set (BICA_PATH ${PLAYER_PATH}/src/interfaces/slice/bica)
set (BICA_FILES
	common
	containers
	datetime
	exceptions
	debug
	logI
	localizationI
	componentsI
	image
)
set (CPP_BICA)
set (H_BICA)

foreach (file ${BICA_FILES})
	set (cc_file ${file}.cpp)
	set (h_file ${file}.h)
	set (bica_file ${file}.ice)
	
	add_custom_command(OUTPUT ${cc_file} ${h_file}
			COMMAND slice2cpp --output-dir=${PROJECT_BINARY_DIR} -I${BICA_PATH} ${BICA_PATH}/${bica_file}
			DEPENDS ${BICA_PATH}/${bica_file}
			VERBATIM
	)
	
	list (APPEND CPP_BICA ${PROJECT_BINARY_DIR}/${cc_file})
	list (APPEND H_BICA ${PROJECT_BINARY_DIR}/${h_file})
endforeach()

add_custom_target(GenerateBica
	    DEPENDS ${CPP_BICA} ${H_BICA}
	    VERBATIM
	)

### Bica generation code end
