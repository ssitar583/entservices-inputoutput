/**
 * If not stated otherwise in this file or this component's LICENSE
 * file the following copyright and licenses apply:
 *
 * Copyright 2025 RDK Management
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

#include "HdcpProfileImplementation.h"

#include "UtilsJsonRpc.h"

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 0
#define API_VERSION_NUMBER_PATCH 0

namespace WPEFramework
{
    namespace Plugin
    {

        SERVICE_REGISTRATION(HdcpProfileImplementation, 1, 0);
        HdcpProfileImplementation *HdcpProfileImplementation::_instance = nullptr;

        HdcpProfileImplementation::HdcpProfileImplementation()
        {
            LOGINFO("Create HdcpProfileImplementation Instance");
            HdcpProfileImplementation::_instance = this;
        }

        HdcpProfileImplementation::~HdcpProfileImplementation()
        {
            LOGINFO("Call HdcpProfileImplementation destructor\n");

            HdcpProfileImplementation::_instance = nullptr;
        }

        /**
         * Register a notification callback
         */
        Core::hresult HdcpProfileImplementation::Register(Exchange::IHdcpProfile::INotification *notification)
        {
            ASSERT(nullptr != notification);

            //_adminLock.Lock();

            // Make sure we can't register the same notification callback multiple times
            if (std::find(_hdcpProfileNotification.begin(), _hdcpProfileNotification.end(), notification) == _hdcpProfileNotification.end())
            {
                _hdcpProfileNotification.push_back(notification);
                notification->AddRef();
            }
            else
            {
                LOGERR("same notification is registered already");
            }

         //   _adminLock.Unlock();

            return Core::ERROR_NONE;
        }

        /**
         * Unregister a notification callback
         */
        Core::hresult HdcpProfileImplementation::Unregister(Exchange::IHdcpProfile::INotification *notification)
        {
            Core::hresult status = Core::ERROR_GENERAL;

            ASSERT(nullptr != notification);

           // _adminLock.Lock();

            // we just unregister one notification once
            auto itr = std::find(_hdcpProfileNotification.begin(), _hdcpProfileNotification.end(), notification);
            if (itr != _hdcpProfileNotification.end())
            {
                (*itr)->Release();
                LOGINFO("Unregister notification");
                _hdcpProfileNotification.erase(itr);
                status = Core::ERROR_NONE;
            }
            else
            {
                LOGERR("notification not found");
            }

          //  _adminLock.Unlock();

            return status;
        }

        void HdcpProfileImplementation::dispatchEvent(Event event, const JsonObject &params)
        {
            Core::IWorkerPool::Instance().Submit(Job::Create(this, event, params));
        }

        void HdcpProfileImplementation::Dispatch(Event event, const JsonObject params)
        {
            //_adminLock.Lock();

            std::list<Exchange::IHdcpProfile::INotification *>::const_iterator index(_hdcpProfileNotification.begin());

            switch (event)
            {
				case HDCPPROFILE_EVENT_DISPLAYCONNECTIONCHANGED:
					while (index != _hdcpProfileNotification.end())
					{
                        HDCPStatus hdcpstatus; // Create an instance of HDCPStatus
                        (*index)->OnDisplayConnectionChanged(hdcpstatus);
                        ++index;
					}
                break;

            default:
                LOGWARN("Event[%u] not handled", event);
                break;
            }
          //  _adminLock.Unlock();
        }

        Core::hresult HdcpProfileImplementation::GetHDCPStatus(HDCPStatus& hdcpstatus,Result &result)
        {
			result.success = true;
            return Core::ERROR_NONE;
        }

		Core::hresult HdcpProfileImplementation::GetSettopHDCPSupport( SupportedHdcpInfo& supportedHdcpInfo,Result &result)
        {
			result.success = true;
            return Core::ERROR_NONE;
        }
       

    } // namespace Plugin
} // namespace WPEFramework
