#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                           Sean Gillen, Nick Nidzieko     
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
      )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m )
endif (${WIN32})


MACRO(ADD_BHV BHV_NAME)
  ADD_LIBRARY(${BHV_NAME} SHARED ${BHV_NAME}.cpp)
  TARGET_LINK_LIBRARIES(${BHV_NAME} 
    helmivp
    behaviors 
    ivpbuild 
    logic 
    ivpcore 
    bhvutil 
    mbutil 
    geometry 
    ${SYSTEM_LIBS} )
ENDMACRO(ADD_BHV)


#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(BHV_SimpleWaypoint SHARED 
   BHV_SimpleWaypoint.cpp AOF_SimpleWaypoint.cpp)
TARGET_LINK_LIBRARIES(BHV_SimpleWaypoint
   helmivp
   behaviors 
   ivpbuild 
   logic 
   ivpcore 
   bhvutil 
   mbutil 
   geometry 
   ${SYSTEM_LIBS} )
#--------------------------------------------------------
#                                      BHV_ConstantAltitude
#--------------------------------------------------------
ADD_LIBRARY(BHV_ConstantAltitude SHARED 
   BHV_ConstantAltitude.cpp)
 TARGET_LINK_LIBRARIES(BHV_ConstantAltitude
   helmivp
   behaviors 
   ivpbuild 
   logic 
   ivpcore 
   bhvutil 
   mbutil 
   geometry 
   ${SYSTEM_LIBS} )

#--------------------------------------------------------
#                                      BHV_FollowCurrent
#--------------------------------------------------------
ADD_LIBRARY(BHV_FollowCurrent SHARED 
   BHV_FollowCurrent.cpp)
TARGET_LINK_LIBRARIES(BHV_FollowCurrent
   helmivp
   behaviors 
   ivpbuild 
   logic 
   ivpcore 
   bhvutil 
   mbutil 
   geometry 
   ${SYSTEM_LIBS} )