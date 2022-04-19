CMAKE_MINIMUM_REQUIRED( VERSION 3.16 )
PROJECT( StationMap )

# Compile Option #
ADD_COMPILE_OPTIONS( -W -Wall -g )
SET ( CMAKE_C_COMPILER "gcc" )

ADD_DEFINITIONS( -DDEBUG )

# SET VAR #
SET( ROOT       ${CMAKE_CURRENT_LIST_DIR}/.. )
SET( LIB_DIR    ${ROOT}/lib )
SET( INC_DIR    ${ROOT}/inc )
SET( MAIN_DIR   ${ROOT}/main )
SET( INS_DIR    ${ROOT}/out )
SET( OUTPUT     runFile )

# Main Source Files #
SET( SOURCE     ${MAIN_DIR}/main.c )

#전체 적용될 라이브러리 링크
LINK_LIBRARIES(
    pthread
)

# Library Files #
ADD_LIBRARY( Basic         ${LIB_DIR}/basic.c )
ADD_LIBRARY( LinkedList    ${LIB_DIR}/linked_list.c )
ADD_LIBRARY( Key           ${LIB_DIR}/key.c )
ADD_LIBRARY( Thread        ${LIB_DIR}/thread.c )
ADD_LIBRARY( IntegrityKey  ${LIB_DIR}/integrity_key.c )

# Set Library List # 
SET( LIB_NAME Basic Key LinkedList Thread IntegrityKey )

# Include #
INCLUDE_DIRECTORIES( ${INC_DIR} )

# Build Setting #
ADD_EXECUTABLE( ${OUTPUT} ${SOURCE} )

# Library Link #
TARGET_LINK_LIBRARIES( IntegrityKey PRIVATE  Key Thread  )
TARGET_LINK_LIBRARIES( ${OUTPUT}    PUBLIC   ${LIB_NAME} )

# Install #
INSTALL( TARGETS ${OUTPUT} DESTINATION ${INS_DIR} )