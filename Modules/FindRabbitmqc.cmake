find_package(rabbitmq-c CONFIG REQUIRED)
# needed because currently rabbitmq target doesn;t have include path in it
get_filename_component(_Rabbitmqc_PARENT_DIR ${rabbitmq-c_DIR} DIRECTORY)
set(_Rabbitmqc_INCLUDE_DIR "${_Rabbitmqc_PARENT_DIR}/include")
while(NOT EXISTS ${_Rabbitmqc_INCLUDE_DIR})
  get_filename_component(_Rabbitmqc_PARENT_DIR ${_Rabbitmqc_PARENT_DIR} DIRECTORY)
  set(_Rabbitmqc_INCLUDE_DIR "${_Rabbitmqc_PARENT_DIR}/include")
endwhile()

set(Rabbitmqc_INCLUDE_DIR ${_Rabbitmqc_INCLUDE_DIR})
message(STATUS "asdasd ${Rabbitmqc_INCLUDE_DIR}")
find_file(_Rabbitmqc_SSL_HEADER
  NAMES amqp_ssl_socket.h
  PATHS ${Rabbitmqc_INCLUDE_DIR}
  NO_DEFAULT_PATH
)

string(COMPARE NOTEQUAL "${_Rabbitmqc_SSL_HEADER}"
  "_Rabbitmqc_SSL_HEADER-NOTFOUND" _rmqc_ssl_enabled)

set(Rabbitmqc_SSL_ENABLED ${_rmqc_ssl_enabled} CACHE BOOL
  "Rabbitmqc is SSL Enabled" FORCE)

if(TARGET rabbitmq::rabbitmq)
  set_target_properties(rabbitmq::rabbitmq PROPERTIES IMPORTED_GLOBAL TRUE)
  add_library(Rabbitmqc::rabbitmq ALIAS rabbitmq::rabbitmq)
else()
  set_target_properties(rabbitmq::rabbitmq-static PROPERTIES IMPORTED_GLOBAL TRUE)
  add_library(Rabbitmqc::rabbitmq ALIAS rabbitmq::rabbitmq-static)
endif()

mark_as_advanced(_Rabbitmqc_INCLUDE_DIR Rabbitmqc_INCLUDE_DIR)
mark_as_advanced(_Rabbitmqc_SSL_HEADER Rabbitmqc_SSL_ENABLED)