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
        :_notification(this),_notification1(this)
        {
            LOGINFO("ctor");
            LOGINFO("HdcpProfileClient Instance creating");
            HdcpProfileClient::_instance = this;
            LOGINFO("HdcpProfileClient Instance created");
            //Register("checkcomrpccomm", &HdcpProfileClient::checkcomrpccomm, this);
        }

        HdcpProfileClient::~HdcpProfileClient()
        {
            LOGINFO("dtor");
        }

        const string HdcpProfileClient::Initialize(PluginHost::IShell* service)
        {
            LOGINFO("Initialize");
             Exchange::IHdcpProfile* _remotStoreObject = nullptr;
            ASSERT(service != nullptr);
            ASSERT(m_service == nullptr);
            LOGINFO("After assert");
            m_service = service;
            m_service->AddRef();
            LOGINFO("After AddRef");

            PluginHost::IPlugin* m_SharedStorageRef = m_service->QueryInterfaceByCallsign<PluginHost::IPlugin>("org.rdk.SharedStorage");
            Exchange::IStore2* _sharedStore = nullptr;
            Exchange::IStoreInspector* _inspector = nullptr;
            Exchange::IStoreLimit* _limit = nullptr;
            Exchange::IStoreCache* _cache = nullptr;

            if (m_SharedStorageRef != nullptr)
            {
                _inspector = m_SharedStorageRef->QueryInterface<Exchange::IStoreInspector>();
                _sharedStore = m_SharedStorageRef->QueryInterface<Exchange::IStore2>();
                _limit = m_SharedStorageRef->QueryInterface<Exchange::IStoreLimit>();
                _cache = m_SharedStorageRef->QueryInterface<Exchange::IStoreCache>();


                if (_sharedStore != nullptr)
                {
                    uint32_t ttl = 500;
                    uint32_t readTTL = 0;
                    std::string value;
                    uint32_t result = 0;

                    //setValueAccount
                    result = _sharedStore->SetValue(Exchange::IStore2::ScopeType::ACCOUNT, "ns200", "key200", "v200", ttl);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (ACCOUNT) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (ACCOUNT) succeeded");
                    }

                    //getValueAccount
                    result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::ACCOUNT, "ns200", "key200", value, readTTL);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (ACCOUNT) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (ACCOUNT) succeeded. Value = %s", value.c_str());
                    }

                    // getNamespacesAccount
                    if (_inspector != nullptr)
                    {
                        RPC::IStringIterator* accountIterator = nullptr;
                        result = _inspector->GetNamespaces(Exchange::IStore2::ScopeType::ACCOUNT, accountIterator);
                        if (result != Core::ERROR_NONE || accountIterator == nullptr)
                        {
                            LOGINFO("HdcpProfileClient: ACCOUNT GetNamespaces failed. Code: %u", result);
                        }
                        else
                        {
                            std::string ns;
                            while (accountIterator->Next(ns))
                            {
                                LOGINFO("HdcpProfileClient: ACCOUNT Namespace: %s", ns.c_str());
                            }
                            accountIterator->Release();
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: GetNamespaces inspector is null");
                    }



                    // DeleteNamespaceAccount
                    result = _sharedStore->DeleteNamespace(Exchange::IStore2::ScopeType::ACCOUNT, "ns200");
                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: DeleteNamespace (ACCOUNT) failed. Code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: DeleteNamespace (ACCOUNT) succeeded.");
                    }

                    //getValueAccount
                    result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::ACCOUNT, "ns200", "key200", value, readTTL);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (ACCOUNT) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (ACCOUNT) succeeded. Value = %s", value.c_str());
                    }

                    //setValueDevice
                    result = _sharedStore->SetValue(Exchange::IStore2::ScopeType::DEVICE, "test_device", "key_device", "value321", ttl);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (DEVICE) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (DEVICE) succeeded");
                    }

                    //getValueDevice
                    
                    result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::DEVICE, "test_device", "key_device", value, readTTL);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue  (DEVICE) succeeded. Value = %s", value.c_str());
                    }


                    //getNamespacesDevice
                    if (_inspector != nullptr)
                    {
                        RPC::IStringIterator* deviceIterator = nullptr;
                        result = _inspector->GetNamespaces(Exchange::IStore2::ScopeType::DEVICE, deviceIterator);
                        if (result != Core::ERROR_NONE || deviceIterator == nullptr)
                        {
                            LOGINFO("HdcpProfileClient: DEVICE GetNamespaces failed. Code: %u", result);
                        }
                        else
                        {
                            std::string ns;
                            while (deviceIterator->Next(ns))
                            {
                                LOGINFO("HdcpProfileClient: DEVICE Namespace: %s", ns.c_str());
                            }
                            deviceIterator->Release();
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: DEVICE getnamespaces inspector is null");
                    }


                    //getStorageSizeDevice
                    if (_inspector)
                    {
                        Exchange::IStoreInspector::INamespaceSizeIterator* deviceSizeIterator = nullptr;
                        result = _inspector->GetStorageSizes(Exchange::IStore2::ScopeType::DEVICE, deviceSizeIterator);

                        if (result != Core::ERROR_NONE || deviceSizeIterator == nullptr)
                        {
                            LOGINFO("HdcpProfileClient: DEVICE getstoragesizes failed. Code: %u", result);
                        }
                        else
                        {
                            Exchange::IStoreInspector::NamespaceSize nsObject;
                            while (deviceSizeIterator->Next(nsObject))
                            {
                                LOGINFO("HdcpProfileClient: getStorageSize DEVICE Namespace: %s, Size: %u", nsObject.ns.c_str(), nsObject.size);
                            }
                            deviceSizeIterator->Release();
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: DEVICE getStorageSizes inspector is null");
                    }

                    // //getKeys
                    if (_inspector)
                    {
                        RPC::IStringIterator *deviceKeyIterator = nullptr;
                        result = _inspector->GetKeys(Exchange::IStore2::ScopeType::DEVICE, "test_device", deviceKeyIterator);
                        if (result != Core::ERROR_NONE || deviceKeyIterator == nullptr)
                        {
                            LOGINFO("HdcpProfileClient: DEVICE GetKeys failed. Code: %u", result);
                        }
                        else
                        {
                            std::string key;
                            while (deviceKeyIterator->Next(key))
                            {
                                LOGINFO("HdcpProfileClient: DEVICE key: %s", key.c_str());
                            }
                            deviceKeyIterator->Release();
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: DEVICE getKeys inspector is null");
                    }


                    //setNamespaceStorageLimit
                    if (_limit)
                    {
                        result = _limit->SetNamespaceStorageLimit(Exchange::IStore2::ScopeType::DEVICE, "test_device", 1024);
                        if (result != Core::ERROR_NONE)
                        {
                            LOGINFO("HdcpProfileClient: SetNamespaceStorageLimit (DEVICE) failed with code: %u", result);
                        }
                        else
                        {
                            LOGINFO("HdcpProfileClient: SetNamespaceStorageLimit (DEVICE) succeeded.");
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: DEVICE getNamespaceStorageLimit limit is null");
                    }

                    //getNamespaceStorageLimit
                    if (_limit)
                    {
                        uint32_t storageLimit = 0;
                        result = _limit->GetNamespaceStorageLimit(Exchange::IStore2::ScopeType::DEVICE, "test_device", storageLimit);
                        if (result != Core::ERROR_NONE)
                        {
                            LOGINFO("HdcpProfileClient: GetNamespaceStorageLimit (DEVICE) failed with code: %u", result);
                        }
                        else
                        {
                            LOGINFO("HdcpProfileClient: GetNamespaceStorageLimit (DEVICE) succeeded. Limit = %u", storageLimit);
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileCLient: Getnamespacestoragelimit limit is null");
                    }

                    //deleteKeyDevice
                    result = _sharedStore->DeleteKey(Exchange::IStore2::ScopeType::DEVICE, "test_device", "key_device");
                    if(result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: deleteKey (DEVICE) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: deleteKey (DEVICE) succeeded");
                    }

                    //getValueDevice
                    result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::DEVICE, "test_device", "key_device", value, readTTL);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) succeeded. Value = %s", value.c_str());
                    }



                    // DeleteNamespaceDevice
                    result = _sharedStore->DeleteNamespace(Exchange::IStore2::ScopeType::DEVICE, "test_device");
                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: Device DeleteNamespace (DEVICE) failed. Code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: Device DeleteNamespace (DEVICE) succeeded.");
                    }


                     //getValueDevice
                     result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::DEVICE, "test_device", "key_device", value, readTTL);

                     if (result != Core::ERROR_NONE)
                     {
                         LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) failed with code: %u", result);
                     }
                     else
                     {
                         LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) succeeded. Value = %s", value.c_str());
                     }

                    //flushCache
                    if (_cache)
                    {
                        result = _cache->FlushCache();
                        if (result != Core::ERROR_NONE)
                        {
                            LOGINFO("HdcpProfileClient: Flushcache failed with code: %u", result);
                        }
                        else
                        {
                            LOGINFO("HdcpProfileClient: FlushCache succeeded.");
                        }
                    }
                    else{
                        LOGINFO("HdcpProfileClient: DEVICE flushcache cache is null");
                    }


                     // ========ADDITIONAL NEGATIVE TESTS============//
                     //getValueDevice
                     result = _sharedStore->GetValue(Exchange::IStore2::ScopeType::DEVICE, "ns1", "doesnotexit", value, readTTL);

                     if (result != Core::ERROR_NONE)
                     {
                         LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) failed with code: %u", result);
                     }
                     else
                     {
                         LOGINFO("HdcpProfileClient: SharedStorage GetValue (DEVICE) succeeded. Value = %s", value.c_str());
                     }

                     //setValueDevice
                    result = _sharedStore->SetValue(Exchange::IStore2::ScopeType::DEVICE, "", "key_device", "", ttl);

                    if (result != Core::ERROR_NONE)
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (DEVICE) failed with code: %u", result);
                    }
                    else
                    {
                        LOGINFO("HdcpProfileClient: SharedStorage SetValue (DEVICE) succeeded");
                    }

                    _sharedStore->Release();
                    
                }
                else
                {
                    LOGINFO("HdcpProfileClient: Failed to get IStore2 from SharedStorage");
                }

                m_SharedStorageRef->Release();
            }
            else 
            {
                LOGINFO("HdcpProfileClient: Failed to get SharedStorage Plugin");
            }

            _remotStoreObject = m_service->QueryInterfaceByCallsign<Exchange::IHdcpProfile>("org.rdk.HdcpProfile");
            if (_remotStoreObject == nullptr) {
                LOGINFO("Failed to get IHdcpProfile interface");
                return "Failed to get IHdcpProfile interface";
            }


            Exchange::IHdcpProfile::HDCPStatus _hdcpstatus;
            bool _success = false;
            _remotStoreObject->GetHDCPStatus(_hdcpstatus, _success);
            if (!_success) {
                LOGINFO("getHDCPStatus failed");
                _remotStoreObject->Release();
                return "getHDCPStatus failed";
            }

            LOGINFO("getHDCPStatus: isConnected: %d isHDCPCompliant: %d isHDCPEnabled: %d hdcpReason: %d supportedHDCPVersion: %s receiverHDCPVersion: %s currentHDCPVersion: %s",
                _hdcpstatus.isConnected, _hdcpstatus.isHDCPCompliant, _hdcpstatus.isHDCPEnabled, _hdcpstatus.hdcpReason,
                _hdcpstatus.supportedHDCPVersion.c_str(), _hdcpstatus.receiverHDCPVersion.c_str(), _hdcpstatus.currentHDCPVersion.c_str());
            
                string supportedHDCPVersion = "0";
                bool isHDCPSupported = false;
                bool success1 = false;
                _remotStoreObject->GetSettopHDCPSupport(supportedHDCPVersion,isHDCPSupported, success1);
                if (!success1) {
                    LOGINFO("getSettopHDCPSupport failed");
                    return "getSettopHDCPSupport failed";
                }
                LOGINFO("getSettopHDCPSupport: supportedHDCPVersion: %s isHDCPSupported: %d",
                    supportedHDCPVersion.c_str(), isHDCPSupported);

            Exchange::IScreenCapture* _remotStoreObject1 = nullptr;
            _remotStoreObject1 = m_service->QueryInterfaceByCallsign<Exchange::IScreenCapture>("org.rdk.ScreenCapture");
            if (_remotStoreObject1 == nullptr) {
                LOGINFO("Failed to get IScreenCapture interface");
                _remotStoreObject->Release();
                return "Failed to get IScreenCapture interface";
            }
            else
            {
                LOGINFO("Got IScreenCapture interface-%p", _remotStoreObject1);
            }
           
            Exchange::IScreenCapture::Result result;
            _remotStoreObject1->UploadScreenCapture("http://server/cgi-bin/upload.cgi","12345",result);
            if (!result.success) {
                LOGINFO("UploadScreenCapture failed");
                _remotStoreObject1->Release();
                return "UploadScreenCapture failed";
            }
            LOGINFO("UploadScreenCapture: result: %d", result.success);
           

            // On success, return an empty string
            return string();
        }


       
        void HdcpProfileClient::OnDisplayConnectionChanged(const Exchange::IHdcpProfile::HDCPStatus& hdcpstatus)
        {
            LOGINFO("OnDisplayConnectionChanged callback triggered: isConnected: %d isHDCPCompliant: %d isHDCPEnabled: %d hdcpReason: %d supportedHDCPVersion: %s receiverHDCPVersion: %s currentHDCPVersion: %s",
                 hdcpstatus.isConnected, hdcpstatus.isHDCPCompliant, hdcpstatus.isHDCPEnabled, hdcpstatus.hdcpReason,
                 hdcpstatus.supportedHDCPVersion.c_str(), hdcpstatus.receiverHDCPVersion.c_str(), hdcpstatus.currentHDCPVersion.c_str());
         }

        void HdcpProfileClient::UploadComplete(const bool& status,const std::string& message,const std::string& call_guid)
        {
            LOGINFO("UploadComplete callback triggered: status: %d message: %s call_guid: %s", status, message.c_str(), call_guid.c_str());
        }


        void HdcpProfileClient::Deinitialize(PluginHost::IShell*  service )
        {
            LOGINFO("Deinitialize");
            ASSERT(service == m_service);
            LOGINFO("Deinitialize: service == m_service");
            m_service->Release();
            m_service = nullptr;
        }

       

    } // namespace Plugin
} // namespace WPEFramework
