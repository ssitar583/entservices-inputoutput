# If not stated otherwise in this file or this component's license file the
# following copyright and licenses apply:
#
# Copyright 2020 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
set(PLUGIN_NAME HdmiCecSink)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})
set(PLUGIN_IMPLEMENTATION ${MODULE_NAME}Implementation)


set(PLUGIN_HDMICECSINK_STARTUPORDER "" CACHE STRING "To configure startup order of HdmiCecSink plugin")

find_package(${NAMESPACE}Plugins REQUIRED)

add_library(${MODULE_NAME} SHARED
        HdmiCecSink.cpp
        Module.cpp)

add_library(${PLUGIN_IMPLEMENTATION} SHARED
        HdmiCecSinkImplementation.cpp
        Module.cpp)

set_target_properties(${MODULE_NAME} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES)

set_target_properties(${PLUGIN_IMPLEMENTATION} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES)

target_compile_definitions(${MODULE_NAME} PRIVATE MODULE_NAME=Plugin_${PLUGIN_NAME})

find_package(DS)
find_package(IARMBus)
find_package(CEC)

target_include_directories(${MODULE_NAME} PRIVATE ${IARMBUS_INCLUDE_DIRS} ../helpers)
target_include_directories(${MODULE_NAME} PRIVATE ${CEC_INCLUDE_DIRS})
target_include_directories(${MODULE_NAME} PRIVATE ${DS_INCLUDE_DIRS})
set_source_files_properties(HdmiCecSink.cpp PROPERTIES COMPILE_FLAGS "-fexceptions")


target_include_directories(${PLUGIN_IMPLEMENTATION} PRIVATE ${IARMBUS_INCLUDE_DIRS} ../helpers)
target_include_directories(${PLUGIN_IMPLEMENTATION} PRIVATE ${CEC_INCLUDE_DIRS})
target_include_directories(${PLUGIN_IMPLEMENTATION} PRIVATE ${DS_INCLUDE_DIRS})
set_source_files_properties(HdmiCecSinkImplementation.cpp PROPERTIES COMPILE_FLAGS "-fexceptions")

target_link_libraries(${MODULE_NAME} PUBLIC ${NAMESPACE}Plugins::${NAMESPACE}Plugins ${IARMBUS_LIBRARIES} ${CEC_LIBRARIES} ${DS_LIBRARIES} )
target_link_libraries(${PLUGIN_IMPLEMENTATION} PUBLIC ${NAMESPACE}Plugins::${NAMESPACE}Plugins ${IARMBUS_LIBRARIES} ${CEC_LIBRARIES} ${DS_LIBRARIES} )

if (NOT RDK_SERVICES_L1_TEST)
    target_compile_options(${PLUGIN_IMPLEMENTATION} PRIVATE -Wno-error=deprecated)
endif ()

install(TARGETS ${MODULE_NAME}
        DESTINATION lib/${STORAGE_DIRECTORY}/plugins)

install(TARGETS ${PLUGIN_IMPLEMENTATION}
        DESTINATION lib/${STORAGE_DIRECTORY}/plugins)

write_config(${PLUGIN_NAME})
