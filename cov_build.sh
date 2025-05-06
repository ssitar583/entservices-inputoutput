#!/bin/bash
set -x
set -e
##############################
GITHUB_WORKSPACE="${PWD}"
ls -la ${GITHUB_WORKSPACE}
############################
# Build entservices-inputoutput
echo "======================================================================================"
echo "buliding entservices-inputoutput"
<<<<<<< HEAD

cd ${GITHUB_WORKSPACE}
cmake -G Ninja -S "$GITHUB_WORKSPACE" -B build/entservices-inputoutput \
-DUSE_THUNDER_R4=ON \
-DCMAKE_INSTALL_PREFIX="$GITHUB_WORKSPACE/install/usr" \
-DCMAKE_MODULE_PATH="$GITHUB_WORKSPACE/install/tools/cmake" \
-DCMAKE_VERBOSE_MAKEFILE=ON \
-DCMAKE_DISABLE_FIND_PACKAGE_IARMBus=ON \
-DCMAKE_DISABLE_FIND_PACKAGE_RFC=ON \
-DCMAKE_DISABLE_FIND_PACKAGE_DS=ON \
-DCMAKE_DISABLE_FIND_PACKAGE_CEC=ON \
-DCOMCAST_CONFIG=OFF \
-DRDK_SERVICES_COVERITY=ON \
-DRDK_SERVICES_L1_TEST=ON \
-DDS_FOUND=ON \
-DPLUGIN_HDMICECSOURCE=ON \
-DPLUGIN_HDCPPROFILE=ON \
-DCMAKE_CXX_FLAGS="-DEXCEPTIONS_ENABLE=ON \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/audiocapturemgr \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/rdk/ds \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/rdk/iarmbus \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/rdk/iarmmgrs-hal \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/ccec/drivers \
-I ${GITHUB_WORKSPACE}/entservices-testframework/Tests/headers/network \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/devicesettings.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/Iarm.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/Rfc.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/RBus.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/Telemetry.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/Udev.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/pkg.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/maintenanceMGR.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/gdialservice.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/wpa_ctrl_mock.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/secure_wrappermock.h \
-include ${GITHUB_WORKSPACE}/entservices-testframework/Tests/mocks/HdmiCec.h \
--coverage -Wall -Werror -Wno-error=format \
-Wl,-wrap,system -Wl,-wrap,popen -Wl,-wrap,syslog \
-DENABLE_TELEMETRY_LOGGING -DUSE_IARMBUS \
-DENABLE_SYSTEM_GET_STORE_DEMO_LINK -DENABLE_DEEP_SLEEP \
-DENABLE_SET_WAKEUP_SRC_CONFIG -DENABLE_THERMAL_PROTECTION \
-DUSE_DRM_SCREENCAPTURE -DHAS_API_SYSTEM -DHAS_API_POWERSTATE \
-DHAS_RBUS -DDISABLE_SECURITY_TOKEN -DENABLE_DEVICE_MANUFACTURER_INFO -DUSE_THUNDER_R4 -DTHUNDER_VERSION=4 -DTHUNDER_VERSION_MAJOR=4 -DTHUNDER_VERSION_MINOR=4" \

cmake --build build/entservices-inputoutput --target install
echo "======================================================================================"
exit 0
=======
cd $GITHUB_WORKSPACE
cmake -G Ninja -S entservices-inputoutput -B build/entservices-inputoutput \
  -DUSE_THUNDER_R4=ON \
  -DCMAKE_INSTALL_PREFIX="$GITHUB_WORKSPACE/install/usr" \
  -DCMAKE_MODULE_PATH="$GITHUB_WORKSPACE/install/tools/cmake" \
  -DCMAKE_VERBOSE_MAKEFILE=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_IARMBus=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_RFC=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_DS=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_Udev=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_RBus=ON \
  -DCMAKE_DISABLE_FIND_PACKAGE_CEC=ON \
  -DCMAKE_CXX_FLAGS="-DEXCEPTIONS_ENABLE=ON \
                      -I ${PWD}/entservices-testframework/Tests/headers \
                      -I ${PWD}/entservices-testframework/Tests/headers/audiocapturemgr \
                      -I ${PWD}/entservices-testframework/Tests/headers/rdk/ds \
                      -I ${PWD}/entservices-testframework/Tests/headers/rdk/iarmbus \
                      -I ${PWD}/entservices-testframework/Tests/headers/rdk/iarmmgrs-hal \
                      -I ${PWD}/entservices-testframework/Tests/headers/ccec/drivers \
                      -I ${PWD}/entservices-testframework/Tests/headers/network \
                      -I ${PWD}/entservices-testframework/Tests \
                      -I ${PWD}/Thunder/Source \
                      -I ${PWD}/Thunder/Source/core \
                      -I/usr/include/gstreamer-1.0 \
                      -I/usr/include/glib-2.0 \
                      -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
                      -I/usr/local/include/trower-base64 \
                      -include ${PWD}/entservices-testframework/Tests/mocks/devicesettings.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/Iarm.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/Rfc.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/RBus.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/Telemetry.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/Udev.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/pkg.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/maintenanceMGR.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/secure_wrappermock.h \
                      -include ${PWD}/entservices-testframework/Tests/mocks/HdmiCec.h \
                      --coverage -Wall -Werror -Wno-error=format \
                      -Wl,-wrap,system -Wl,-wrap,popen -Wl,-wrap,syslog \
                      -DENABLE_TELEMETRY_LOGGING -DUSE_IARMBUS \
                      -DENABLE_SYSTEM_GET_STORE_DEMO_LINK -DENABLE_DEEP_SLEEP \
                      -DENABLE_SET_WAKEUP_SRC_CONFIG -DENABLE_THERMAL_PROTECTION \
                      -DUSE_DRM_SCREENCAPTURE -DHAS_API_SYSTEM -DHAS_API_POWERSTATE \
                      -DHAS_RBUS -DDISABLE_SECURITY_TOKEN -DENABLE_DEVICE_MANUFACTURER_INFO -DUSE_THUNDER_R4 -DTHUNDER_VERSION=4 -DTHUNDER_VERSION_MAJOR=4 -DTHUNDER_VERSION_MINOR=4" \
  -DCOMCAST_CONFIG=OFF \
  -DRDK_SERVICES_COVERITY=ON \
  -DRDK_SERVICES_L1_TEST=ON \
  -DDS_FOUND=ON \
  -DPLUGIN_HDMICECSINK=OFF \
  -DPLUGIN_HDMICECSOURCE=ON \
  -DPLUGIN_HDCPPROFILE=ON \
  -DPLUGIN_AVINPUT=OFF \
  -DPLUGIN_AVOUTPUT=OFF \
  -DPLUGIN_HDMIINPUT=OFF \

cmake --build build/entservices-inputoutput --target install
echo "======================================================================================"
echo "path of so files"
find . -iname "*.so*"
exit 0
>>>>>>> 3a643117249fb4012cc42886178a62058bacd1ec
