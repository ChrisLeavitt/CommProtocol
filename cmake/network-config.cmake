# Tools config files, which are added into a set for compilation

set(NETWORK_HEADERS
  ${NETWORK_HEADER_DIR}UDP.h
  ${NETWORK_HEADER_DIR}Serial.h
  ${NETWORK_HEADER_DIR}CommSocket.h
  ${NETWORK_HEADER_DIR}CommsLink.h)

set(NETWORK_SOURCES
  ${NETWORK_SOURCE_DIR}UDP.cpp
  ${NETWORK_SOURCE_DIR}Serial.cpp
  ${NETWORK_SOURCE_DIR}TcpSocket.cpp)