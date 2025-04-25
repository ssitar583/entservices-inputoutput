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

#include "UtilsIarm.h"
#include "UtilsJsonRpc.h"
#include "UtilssyncPersistFile.h"
#include "UtilsSearchRDKProfile.h"


#define TR181_HDMICECSINK_CEC_VERSION "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion"


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

       const std::string HdmiCecSink::Initialize(PluginHost::IShell *service)
       {
		   profileType = searchRdkProfile();

		   if (profileType == STB || profileType == NOT_FOUND)
		   {
			   LOGINFO("Invalid profile type for TV \n");
			   return (std::string("Not supported"));
		   }

           HdmiCecSink::_instance = this;
		   smConnection=NULL;
		   cecEnableStatus = false;
		   HdmiCecSink::_instance->m_numberOfDevices = 0;
		   m_logicalAddressAllocated = LogicalAddress::UNREGISTERED;
		   m_currentActiveSource = -1;
		   m_isHdmiInConnected = false;
		   hdmiCecAudioDeviceConnected = false;
		   m_isAudioStatusInfoUpdated = false;
		   m_audioStatusReceived = false;
		   m_audioStatusTimerStarted = false;
                   m_audioDevicePowerStatusRequested = false;
		   m_pollNextState = POLL_THREAD_STATE_NONE;
		   m_pollThreadState = POLL_THREAD_STATE_NONE;
		   m_video_latency = DEFAULT_VIDEO_LATENCY;
		   m_latency_flags = DEFAULT_LATENCY_FLAGS ;
		   m_audio_output_delay = DEFAULT_AUDIO_OUTPUT_DELAY;

           Register(HDMICECSINK_METHOD_SET_ENABLED, &HdmiCecSink::setEnabledWrapper, this);
           Register(HDMICECSINK_METHOD_GET_ENABLED, &HdmiCecSink::getEnabledWrapper, this);
           Register(HDMICECSINK_METHOD_SET_OSD_NAME, &HdmiCecSink::setOSDNameWrapper, this);
           Register(HDMICECSINK_METHOD_GET_OSD_NAME, &HdmiCecSink::getOSDNameWrapper, this);
           Register(HDMICECSINK_METHOD_SET_VENDOR_ID, &HdmiCecSink::setVendorIdWrapper, this);
           Register(HDMICECSINK_METHOD_GET_VENDOR_ID, &HdmiCecSink::getVendorIdWrapper, this);
		   Register(HDMICECSINK_METHOD_PRINT_DEVICE_LIST, &HdmiCecSink::printDeviceListWrapper, this);
		   Register(HDMICECSINK_METHOD_SET_ACTIVE_PATH, &HdmiCecSink::setActivePathWrapper, this);
		   Register(HDMICECSINK_METHOD_SET_ROUTING_CHANGE, &HdmiCecSink::setRoutingChangeWrapper, this); 
		   Register(HDMICECSINK_METHOD_GET_DEVICE_LIST, &HdmiCecSink::getDeviceListWrapper, this);
		   Register(HDMICECSINK_METHOD_GET_ACTIVE_SOURCE, &HdmiCecSink::getActiveSourceWrapper, this);
		   Register(HDMICECSINK_METHOD_SET_ACTIVE_SOURCE, &HdmiCecSink::setActiveSourceWrapper, this);
	       Register(HDMICECSINK_METHOD_GET_ACTIVE_ROUTE, &HdmiCecSink::getActiveRouteWrapper, this);
		   Register(HDMICECSINK_METHOD_REQUEST_ACTIVE_SOURCE, &HdmiCecSink::requestActiveSourceWrapper, this);
           Register(HDMICECSINK_METHOD_SETUP_ARC, &HdmiCecSink::setArcEnableDisableWrapper, this);
		   Register(HDMICECSINK_METHOD_SET_MENU_LANGUAGE, &HdmiCecSink::setMenuLanguageWrapper, this);
           Register(HDMICECSINK_METHOD_REQUEST_SHORT_AUDIO_DESCRIPTOR, &HdmiCecSink::requestShortAudioDescriptorWrapper, this);
           Register(HDMICECSINK_METHOD_SEND_STANDBY_MESSAGE, &HdmiCecSink::sendStandbyMessageWrapper, this);	   
		   Register(HDMICECSINK_METHOD_SEND_AUDIO_DEVICE_POWER_ON, &HdmiCecSink::sendAudioDevicePowerOnMsgWrapper, this);
		   Register(HDMICECSINK_METHOD_SEND_KEY_PRESS,&HdmiCecSink::sendRemoteKeyPressWrapper,this);
		   Register(HDMICECSINK_METHOD_SEND_USER_CONTROL_PRESSED,&HdmiCecSink::sendUserControlPressedWrapper,this);
		   Register(HDMICECSINK_METHOD_SEND_USER_CONTROL_RELEASED,&HdmiCecSink::sendUserControlReleasedWrapper,this);
		   Register(HDMICECSINK_METHOD_SEND_GIVE_AUDIO_STATUS,&HdmiCecSink::sendGiveAudioStatusWrapper,this);
		   Register(HDMICECSINK_METHOD_GET_AUDIO_DEVICE_CONNECTED_STATUS,&HdmiCecSink::getAudioDeviceConnectedStatusWrapper,this);
		   Register(HDMICECSINK_METHOD_REQUEST_AUDIO_DEVICE_POWER_STATUS,&HdmiCecSink::requestAudioDevicePowerStatusWrapper,this);
		   Register(HDMICECSINK_METHOD_SET_LATENCY_INFO, &HdmiCecSink::setLatencyInfoWrapper, this);
           logicalAddressDeviceType = "None";
           logicalAddress = 0xFF;
           // load persistence setting
           loadSettings();

           int err;
           dsHdmiInGetNumberOfInputsParam_t hdmiInput;
           InitializeIARM();
           m_sendKeyEventThreadExit = false;
           m_sendKeyEventThread = std::thread(threadSendKeyEvent);

           m_currentArcRoutingState = ARC_STATE_ARC_TERMINATED;
           m_semSignaltoArcRoutingThread.acquire();
           m_arcRoutingThread = std::thread(threadArcRouting);

	   m_audioStatusDetectionTimer.connect( std::bind( &HdmiCecSink::audioStatusTimerFunction, this ) );
	   m_audioStatusDetectionTimer.setSingleShot(true);
           m_arcStartStopTimer.connect( std::bind( &HdmiCecSink::arcStartStopTimerFunction, this ) );
           m_arcStartStopTimer.setSingleShot(true);
            // get power state:
            uint32_t res = Core::ERROR_GENERAL;
            PowerState pwrStateCur = WPEFramework::Exchange::IPowerManager::POWER_STATE_UNKNOWN;
            PowerState pwrStatePrev = WPEFramework::Exchange::IPowerManager::POWER_STATE_UNKNOWN;

            ASSERT (_powerManagerPlugin);
            if (_powerManagerPlugin) {
                res = _powerManagerPlugin->GetPowerState(pwrStateCur, pwrStatePrev);
                if (Core::ERROR_NONE == res) {
                    powerState = (pwrStateCur == WPEFramework::Exchange::IPowerManager::POWER_STATE_ON) ? DEVICE_POWER_STATE_ON : DEVICE_POWER_STATE_OFF;
                    LOGINFO("Current state is PowerManagerPlugin: (%d) powerState :%d \n", pwrStateCur, powerState);
                }
            }

            err = IARM_Bus_Call(IARM_BUS_DSMGR_NAME,
                            IARM_BUS_DSMGR_API_dsHdmiInGetNumberOfInputs,
                            (void *)&hdmiInput,
                            sizeof(hdmiInput));

           if (err == IARM_RESULT_SUCCESS && hdmiInput.result == dsERR_NONE)
           {
				LOGINFO("Number of Inputs [%d] \n", hdmiInput.numHdmiInputs );
            	m_numofHdmiInput = hdmiInput.numHdmiInputs;
           }else{
				LOGINFO("Not able to get Numebr of inputs so defaulting to 3 \n");
				m_numofHdmiInput = 3;
			}

			LOGINFO("initalize inputs \n");

           for (int i = 0; i < m_numofHdmiInput; i++){
				HdmiPortMap hdmiPort((uint8_t)i);
				LOGINFO(" Add to vector [%d] \n", i);
				hdmiInputs.push_back(hdmiPort);
			}

			LOGINFO("Check the HDMI State \n");

			CheckHdmiInState();
			if (cecSettingEnabled)
            {
               try
               {
                   CECEnable();
               }
               catch(...)
               {
                   LOGWARN("Exception while enabling CEC settings .\r\n");
               }
            }
            getCecVersion();
	   LOGINFO(" HdmiCecSink plugin Initialize completed \n");
           return (std::string());

       }

       void HdmiCecSink::Deinitialize(PluginHost::IShell* /* service */)
       {
		profileType = searchRdkProfile();

		if (profileType == STB || profileType == NOT_FOUND)
		{
			LOGINFO("Invalid profile type for TV \n");
			return ;
		}

        Exchange::IHdmiCecSink::HdmiCecSinkSuccess success;

        HdmiCecSink::_hdmiCecSink->SetEnabled(false,success);

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
