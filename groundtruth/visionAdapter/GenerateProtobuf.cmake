##------------------------------------------------------------
## Author:		Gonzalo Abella (abellagonzalo@gmail.com)
## Update:		25/01/2012 by Gonzalo Abella
##------------------------------------------------------------

### protocol buffer generation code begin
set (PROTO_PATH ${SSLVISION_PATH}/src/shared/proto)
set (PROTO_FILES
	messages_robocup_ssl_detection
	messages_robocup_ssl_geometry
	messages_robocup_ssl_wrapper
	messages_robocup_ssl_refbox_log
)
set (CC_PROTO)
set (H_PROTO)

foreach (file ${PROTO_FILES})
	set (cc_file ${file}.pb.cc)
	set (h_file ${file}.pb.h)
	set (proto_file ${file}.proto)
	
	add_custom_command(OUTPUT ${cc_file} ${h_file}
			COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR} --proto_path=${PROTO_PATH} ${PROTO_PATH}/${proto_file}
			DEPENDS ${PROTO_PATH}/${proto_file}
			VERBATIM
	)
	
	list (APPEND CC_PROTO ${PROJECT_BINARY_DIR}/${cc_file})
	list (APPEND H_PROTO ${PROJECT_BINARY_DIR}/${h_file})
endforeach()

add_custom_target(GenerateProto
	    DEPENDS ${CC_PROTO} ${H_PROTO}
	    VERBATIM
	)
### protocol buffer generation code end
