##------------------------------------------------------------
## Author:		Gonzalo Abella (abellagonzalo@gmail.com)
## Update:		25/01/2012 by Gonzalo Abella
##------------------------------------------------------------

### BICA generation code begin

set(BICA_ICEFILES_DIR ${CMAKE_CURRENT_LIST_DIR}/slice/bica)
set(BICA_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/cpp/bica)
set(SLICE2CPPE ${PROJECT_SOURCE_DIR}/bin/slice2cppe) 

file(MAKE_DIRECTORY ${BICA_OUTPUT_DIR})

set (BICA_FILES
	common
	containers
	datetime
	exceptions
	image
	schedulerManager
	localizationI
	teamInfoI
	componentsI
	motionManagerI
	multimediaI
	debug
	logI
	demencia
)

foreach (file ${BICA_FILES})
	set (bica_file ${file}.ice)
	set(args --output-dir=${BICA_OUTPUT_DIR} -I. -I${BICA_ICEFILES_DIR} ${BICA_ICEFILES_DIR}/${bica_file})
	execute_process(COMMAND ${SLICE2CPPE} ${args})	
endforeach()

### Bica generation code end



### Bica for jderobot begin

set(JDEROBOT_ICEFILES_DIR ${CMAKE_CURRENT_LIST_DIR}/slice)
set(JDEROBOT_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/cpp/jderobot)

file(MAKE_DIRECTORY ${JDEROBOT_OUTPUT_DIR})

set (JDEROBOT_FILES
	common_jderobot
	containers_jderobot
	datetime_jderobot
	exceptions_jderobot
	image_jderobot
	camera
	motors
	encoders
	pose3dmotors
	pose3dencoders
)

foreach (file ${JDEROBOT_FILES})
	set (jderobot_file jderobot/${file}.ice)
	set(args --output-dir=${JDEROBOT_OUTPUT_DIR} -I. -I${JDEROBOT_ICEFILES_DIR} ${JDEROBOT_ICEFILES_DIR}/${jderobot_file})
	execute_process(COMMAND ${SLICE2CPPE} ${args})	
endforeach()

### Bica for jderobot end

