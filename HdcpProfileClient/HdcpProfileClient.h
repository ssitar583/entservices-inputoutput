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

#pragma once

#include "Module.h"
#include <interfaces/IHdcpProfile.h>


namespace WPEFramework {
    namespace Plugin {

        class HdcpProfileClient : public PluginHost::IPlugin {
        private:

            HdcpProfileClient(const HdcpProfileClient&) = delete;
            HdcpProfileClient& operator=(const HdcpProfileClient&) = delete;

           

        public:
            HdcpProfileClient();
            virtual ~HdcpProfileClient();
            virtual const string Initialize(PluginHost::IShell* shell) override ;
            virtual void Deinitialize(PluginHost::IShell* service) override;
            virtual string Information() const override { return {}; }
            void OnDisplayConnectionChanged(const Exchange::IHdcpProfile::HDCPStatus& hdcpstatus);

            BEGIN_INTERFACE_MAP(HdcpProfileClient)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            END_INTERFACE_MAP

        public:
            static HdcpProfileClient* _instance;
            PluginHost::IShell* m_service = nullptr;
        };
    } // namespace Plugin
} // namespace WPEFramework
