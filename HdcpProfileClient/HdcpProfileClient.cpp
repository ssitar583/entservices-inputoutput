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

#include "HdcpProfileClient.h"

#include "UtilsJsonRpc.h"

#include <glib.h>
#include <glib/gstdio.h>

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 0
#define API_VERSION_NUMBER_PATCH 0

using namespace std;

namespace WPEFramework {

    namespace {

        static Plugin::Metadata<Plugin::HdcpProfileClient> metadata(
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

    namespace Plugin {

        SERVICE_REGISTRATION(HdcpProfileClient, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

        HdcpProfileClient* HdcpProfileClient::_instance = nullptr;

        HdcpProfileClient::HdcpProfileClient()
        {
            LOGINFO("ctor");
            HdcpProfileClient::_instance = this;
            //Register("checkcomrpccomm", &HdcpProfileClient::checkcomrpccomm, this);
        }

        HdcpProfileClient::~HdcpProfileClient()
        {
            LOGINFO("dtor");
        }

        const string HdcpProfileClient::Initialize(PluginHost::IShell* service)
        {
	        Exchange::IHdcpProfile* _remotStoreObject = nullptr;
            ASSERT(service != nullptr);
            ASSERT(m_service == nullptr);

            m_service = service;
            m_service->AddRef();
            _remotStoreObject = service->QueryInterfaceByCallsign<Exchange::IHdcpProfile>("org.rdk.HdcpProfile");

            // HDCPStatus _hdcpstatus;
            Exchange::IHdcpProfile::HDCPStatus _hdcpstatus;
            bool _success = false;
            _remotStoreObject->GetHDCPStatus(_hdcpstatus, _success);
            if (_success == false)
            {
                LOGERR("getHDCPStatus failed");
                return (string());
            }
            LOGINFO("getHDCPStatus: isConnected: %d isHDCPCompliant: %d isHDCPEnabled: %d hdcpReason: %d supportedHDCPVersion: %s receiverHDCPVersion: %s currentHDCPVersion: %s",
                _hdcpstatus.isConnected, _hdcpstatus.isHDCPCompliant, _hdcpstatus.isHDCPEnabled, _hdcpstatus.hdcpReason,
                _hdcpstatus.supportedHDCPVersion.c_str(), _hdcpstatus.receiverHDCPVersion.c_str(), _hdcpstatus.currentHDCPVersion.c_str());
	        ASSERT (nullptr != _remotStoreObject);  
            string supportedHDCPVersion;
            bool isHDCPSupported = false;
            bool success1 = false;
            _remotStoreObject->GetSettopHDCPSupport(supportedHDCPVersion, isHDCPSupported, success1);
            if (success1 == false)
            {
                LOGERR("getSettopHDCPSupport failed");
                return (string());
            }
            LOGINFO("getSettopHDCPSupport: supportedHDCPVersion: %s isHDCPSupported: %d", supportedHDCPVersion.c_str(), isHDCPSupported);
            //subscribe the events onDisplayConnectionChanged through com-rpc
            //register the notification
            Exchange::IHdcpProfile::INotification* notification = service->QueryInterfaceByCallsign<Exchange::IHdcpProfile::INotification>("org.rdk.HdcpProfile");
            if (notification == nullptr)
            {
                LOGERR("Failed to get notification interface");
                return (string());
            }
            //register the notification
            _remotStoreObject->Register(notification);


            // if (notification != nullptr)
            // {
            //     notification->Release();
            //     notification = nullptr;
            // }

            // On success return empty, to indicate there is no error text.
            return (string());
        }


        //define the onDisplayConnectionChanged method
        void HdcpProfileClient::OnDisplayConnectionChanged(const Exchange::IHdcpProfile::HDCPStatus& hdcpstatus)
        {
            LOGINFO("OnDisplayConnectionChanged callback triggered: isConnected: %d isHDCPCompliant: %d isHDCPEnabled: %d hdcpReason: %d supportedHDCPVersion: %s receiverHDCPVersion: %s currentHDCPVersion: %s",
                hdcpstatus.isConnected, hdcpstatus.isHDCPCompliant, hdcpstatus.isHDCPEnabled, hdcpstatus.hdcpReason,
                hdcpstatus.supportedHDCPVersion.c_str(), hdcpstatus.receiverHDCPVersion.c_str(), hdcpstatus.currentHDCPVersion.c_str());
         }


        void HdcpProfileClient::Deinitialize(PluginHost::IShell*  service )
        {
            ASSERT(service == m_service);

            m_service->Release();
            m_service = nullptr;
        }

       

    } // namespace Plugin
} // namespace WPEFramework
