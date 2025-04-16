/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2019 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include "HdmiCecSink.h"

#include "ccec/Connection.hpp"
#include "ccec/CECFrame.hpp"
#include "ccec/MessageEncoder.hpp"
#include "host.hpp"
#include "UtilsgetRFCConfig.h"

#include "dsMgr.h"
#include "dsRpc.h"
#include "dsDisplay.h"
#include "videoOutputPort.hpp"
#include "manager.hpp"
#include "websocket/URL.h"

#include "UtilsIarm.h"
#include "UtilsJsonRpc.h"
#include "UtilssyncPersistFile.h"
#include "UtilsSearchRDKProfile.h"

#define HDMICECSINK_METHOD_SET_ENABLED 			"setEnabled"
#define HDMICECSINK_METHOD_GET_ENABLED 			"getEnabled"
#define HDMICECSINK_METHOD_OTP_SET_ENABLED 		"setOTPEnabled"
#define HDMICECSINK_METHOD_OTP_GET_ENABLED 		"getOTPEnabled"
#define HDMICECSINK_METHOD_SET_OSD_NAME 		"setOSDName"
#define HDMICECSINK_METHOD_GET_OSD_NAME 		"getOSDName"
#define HDMICECSINK_METHOD_SET_VENDOR_ID 		"setVendorId"
#define HDMICECSINK_METHOD_GET_VENDOR_ID 		"getVendorId"
#define HDMICECSINK_METHOD_PRINT_DEVICE_LIST 	"printDeviceList"
#define HDMICECSINK_METHOD_SET_ACTIVE_PATH 		"setActivePath"
#define HDMICECSINK_METHOD_SET_ROUTING_CHANGE 	"setRoutingChange"
#define HDMICECSINK_METHOD_GET_DEVICE_LIST 		"getDeviceList"
#define HDMICECSINK_METHOD_GET_ACTIVE_SOURCE 	"getActiveSource"
#define HDMICECSINK_METHOD_SET_ACTIVE_SOURCE 	"setActiveSource"
#define HDMICECSINK_METHOD_GET_ACTIVE_ROUTE   	"getActiveRoute"
#define HDMICECSINK_METHOD_SET_MENU_LANGUAGE  	"setMenuLanguage"
#define HDMICECSINK_METHOD_REQUEST_ACTIVE_SOURCE "requestActiveSource"
#define HDMICECSINK_METHOD_SETUP_ARC              "setupARCRouting"
#define HDMICECSINK_METHOD_REQUEST_SHORT_AUDIO_DESCRIPTOR  "requestShortAudioDescriptor"
#define HDMICECSINK_METHOD_SEND_STANDBY_MESSAGE            "sendStandbyMessage"
#define HDMICECSINK_METHOD_SEND_AUDIO_DEVICE_POWER_ON "sendAudioDevicePowerOnMessage"
#define HDMICECSINK_METHOD_SEND_KEY_PRESS                          "sendKeyPressEvent"
#define HDMICECSINK_METHOD_SEND_USER_CONTROL_PRESSED          "sendUserControlPressed"
#define HDMICECSINK_METHOD_SEND_USER_CONTROL_RELEASED         "sendUserControlReleased"
#define HDMICECSINK_METHOD_SEND_GIVE_AUDIO_STATUS          "sendGetAudioStatusMessage"
#define HDMICECSINK_METHOD_GET_AUDIO_DEVICE_CONNECTED_STATUS   "getAudioDeviceConnectedStatus"
#define HDMICECSINK_METHOD_REQUEST_AUDIO_DEVICE_POWER_STATUS   "requestAudioDevicePowerStatus"
#define HDMICECSINK_METHOD_SET_LATENCY_INFO	"setLatencyInfo"

#define TEST_ADD 0
#define HDMICECSINK_REQUEST_MAX_RETRY 				3
#define HDMICECSINK_REQUEST_MAX_WAIT_TIME_MS 		2000
#define HDMICECSINK_PING_INTERVAL_MS 				10000
#define HDMICECSINK_WAIT_FOR_HDMI_IN_MS 			1000
#define HDMICECSINK_REQUEST_INTERVAL_TIME_MS 		500
#define HDMICECSINK_NUMBER_TV_ADDR 					2
#define HDMICECSINK_UPDATE_POWER_STATUS_INTERVA_MS    (60 * 1000)
#define HDMISINK_ARC_START_STOP_MAX_WAIT_MS           4000
#define HDMICECSINK_UPDATE_AUDIO_STATUS_INTERVAL_MS    500


#define SAD_FMT_CODE_AC3 2
#define SAD_FMT_CODE_ENHANCED_AC3 10

#define SYSTEM_AUDIO_MODE_ON 0x01
#define SYSTEM_AUDIO_MODE_OFF 0x00
#define AUDIO_DEVICE_POWERSTATE_OFF 1

#define DEFAULT_VIDEO_LATENCY 100
#define DEFAULT_LATENCY_FLAGS 3
#define DEFAULT_AUDIO_OUTPUT_DELAY 100

//Device Type is TV - Bit 7 is set to 1
#define ALL_DEVICE_TYPES  128

//RC Profile of TV is 3 - Typical TV Remote
#define RC_PROFILE_TV 10

//Device Features supported by TV - ARC Tx
#define DEVICE_FEATURES_TV 4

#define TR181_HDMICECSINK_CEC_VERSION "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion"

enum {
	DEVICE_POWER_STATE_ON = 0,
	DEVICE_POWER_STATE_OFF = 1
};


#define CEC_SETTING_ENABLED_FILE "/opt/persistent/ds/cecData_2.json"
#define CEC_SETTING_OTP_ENABLED "cecOTPEnabled"
#define CEC_SETTING_ENABLED "cecEnabled"
#define CEC_SETTING_OSD_NAME "cecOSDName"
#define CEC_SETTING_VENDOR_ID "cecVendorId"

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 3
#define API_VERSION_NUMBER_PATCH 10

namespace WPEFramework
{
	namespace {

        static Plugin::Metadata<Plugin::HdmiCecSink> metadata(
            // Version (Major, Minor, Patch)
            API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    namespace Plugin
    {
        SERVICE_REGISTRATION(HdmiCecSink, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

        HdmiCecSink* HdmiCecSink::_instance = nullptr;
        static int libcecInitStatus = 0;


//=========================================== HdmiCecSink =========================================

       HdmiCecSink::HdmiCecSink()
       : PluginHost::JSONRPC()
        , _pwrMgrNotification(*this)
        , _registeredEventHandlers(false)
       {
           LOGWARN("Initlaizing HdmiCecSink");
       }

       HdmiCecSink::~HdmiCecSink()
       {
       }
       const std::string HdmiCecSink::Initialize(PluginHost::IShell *service)
       {
		   profileType = searchRdkProfile();

		   if (profileType == STB || profileType == NOT_FOUND)
		   {
			   LOGINFO("Invalid profile type for TV \n");
			   return (std::string("Not supported"));
		   }

           string msg = "";

           ASSERT(nullptr != service);
           ASSERT(nullptr == _service);
           ASSERT(nullptr == _hdmiCecSink);
           ASSERT(0 == _connectionId);


           _service = service;
           _service->AddRef();
           _service->Register(&_notification);
           _hdmiCecSink = _service->Root<Exchange::IHdmiCecSink>(_connectionId, 5000, _T("HdmiCecSinkImplementation"));

           if(nullptr != _hdmiCecSink)
            {
                _hdmiCecSink->Configure(service);
                _hdmiCecSink->Register(&_notification);
                Exchange::JHdmiCecSink::Register(*this, _hdmiCecSink);
                LOGINFO("HdmiCecSink plugin is available. Successfully activated HdmiCecSink Plugin");
            }
            else
            {
                msg = "HdmiCecSink plugin is not available";
                LOGINFO("HdmiCecSink plugin is not available. Failed to activate HdmiCecSink Plugin");
            }

            if (0 != msg.length())
            {
                Deinitialize(service);
            }

           // On success return empty, to indicate there is no error text.
           return msg;


       }

       void HdmiCecSink::Deinitialize(PluginHost::IShell* /* service */)
       {
           if(_powerManagerPlugin)
           {
               _powerManagerPlugin.Reset();
           }
           _registeredEventHandlers = false;

		profileType = searchRdkProfile();

		if (profileType == STB || profileType == NOT_FOUND)
		{
			LOGINFO("Invalid profile type for TV \n");
			return ;
		}

	    CECDisable();

        if(nullptr != _hdmiCecSink)
           {
             _hdmiCecSink->Unregister(&_notification);
             Exchange::JHdmiCecSink::Unregister(*this);
             _hdmiCecSink->Release();
             _hdmiCecSink = nullptr;

             RPC::IRemoteConnection* connection = _service->RemoteConnection(_connectionId);
             if (connection != nullptr)
             {
                try{
                    connection->Terminate();
                }
                catch(const std::exception& e)
                {
                    std::string errorMessage = "Failed to terminate connection: ";
                    errorMessage += e.what();
                    LOGWARN("%s",errorMessage.c_str());
                }

                connection->Release();
             }
           }

        _connectionId = 0;
        _service->Release();
        _service = nullptr;
	    LOGWARN(" HdmiCecSink Deinitialize() Done");
       }

       string HdmiCecSink::Information() const
        {
            return("This HdmiCecSink PLugin Facilitates the HDMI CEC Sink Control");
        }

        void HdmiCecSink::Deactivated(RPC::IRemoteConnection* connection)
        {
            if (connection->Id() == _connectionId)
            {
                ASSERT(_service != nullptr);
                Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
            }
        }

    } // namespace Plugin
} // namespace WPEFrameworklk
