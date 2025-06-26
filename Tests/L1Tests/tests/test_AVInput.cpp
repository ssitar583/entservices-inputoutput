/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2024 RDK Management
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

#include <gtest/gtest.h>

#include "AVInput.h"

#include "CompositeInputMock.h"
#include "FactoriesImplementation.h"
#include "IarmBusMock.h"
#include "HostMock.h"
#include "ServiceMock.h"
#include "HdmiInputMock.h"
#include "ThunderPortability.h"

using namespace WPEFramework;

using ::testing::NiceMock;

class AVInputTest : public ::testing::Test {
protected:
    Core::ProxyType<Plugin::AVInput> plugin;
    Core::JSONRPC::Handler& handler;
    DECL_CORE_JSONRPC_CONX connection;
    string response;

    AVInputTest()
        : plugin(Core::ProxyType<Plugin::AVInput>::Create())
        , handler(*(plugin))
        , INIT_CONX(1, 0)
    {
    }
    virtual ~AVInputTest() = default;
};

class AVInputDsTest : public AVInputTest {
protected:
    HdmiInputImplMock   *p_hdmiInputImplMock = nullptr ;
    CompositeInputImplMock   *p_compositeInputImplMock = nullptr ;
    HostImplMock   *p_HostImplMock = nullptr ;
    IARM_EventHandler_t dsAVGameFeatureStatusEventHandler;
    IARM_EventHandler_t dsAVEventHandler;
    IARM_EventHandler_t dsAVSignalStatusEventHandler;
    IARM_EventHandler_t dsAVStatusEventHandler;
    IARM_EventHandler_t dsAVVideoModeEventHandler;
    IARM_EventHandler_t dsAviContentTypeEventHandler;

    AVInputDsTest()
        : AVInputTest()
    {
        p_hdmiInputImplMock  = new NiceMock <HdmiInputImplMock>;
        device::HdmiInput::setImpl(p_hdmiInputImplMock);

        p_compositeInputImplMock = new NiceMock <CompositeInputImplMock>;
        device::CompositeInput::setImpl(p_compositeInputImplMock);

        p_HostImplMock = new NiceMock <HostImplMock>;
        device::Host::setImpl(p_HostImplMock);


    }
    virtual ~AVInputDsTest() override
    {
        device::HdmiInput::setImpl(nullptr);
        if (p_hdmiInputImplMock != nullptr)
        {
            delete p_hdmiInputImplMock;
            p_hdmiInputImplMock = nullptr;
        }

        device::CompositeInput::setImpl(nullptr);
        if (p_compositeInputImplMock != nullptr)
        {
            delete p_compositeInputImplMock;
            p_compositeInputImplMock = nullptr;
        }

        device::Host::setImpl(nullptr);
        if (p_HostImplMock != nullptr)
        {
            delete p_HostImplMock;
            p_HostImplMock = nullptr;
        }
    }
};

class AVInputInit : public AVInputDsTest {
protected:
    IarmBusImplMock* p_iarmBusImplMock = nullptr;
    NiceMock<FactoriesImplementation> factoriesImplementation;
    PLUGINHOST_DISPATCHER* dispatcher;
    ServiceMock service;
    Core::JSONRPC::Message message;

    AVInputInit()
        : AVInputDsTest()
    {
        p_iarmBusImplMock = new NiceMock<IarmBusImplMock>;
        IarmBus::setImpl(p_iarmBusImplMock);

        ON_CALL(*p_iarmBusImplMock, IARM_Bus_RegisterEventHandler(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Invoke(
                [&](const char* ownerName, IARM_EventId_t eventId, IARM_EventHandler_t handler) {
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_HOTPLUG)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVSignalStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVVideoModeEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_ALLM_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVGameFeatureStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_VRR_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVGameFeatureStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_HOTPLUG)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVSignalStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_STATUS)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVStatusEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_VIDEO_MODE_UPDATE)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAVVideoModeEventHandler = handler;
                    }
                    if ((string(IARM_BUS_DSMGR_NAME) == string(ownerName)) && (eventId == IARM_BUS_DSMGR_EVENT_HDMI_IN_AVI_CONTENT_TYPE)) {
                        EXPECT_TRUE(handler != nullptr);
                        dsAviContentTypeEventHandler = handler;
                    }
                    return IARM_RESULT_SUCCESS;
                }));
        EXPECT_EQ(string(""), plugin->Initialize(&service));

        PluginHost::IFactories::Assign(&factoriesImplementation);
        dispatcher = static_cast<PLUGINHOST_DISPATCHER*>(
            plugin->QueryInterface(PLUGINHOST_DISPATCHER_ID));
        dispatcher->Activate(&service);
    }

    virtual ~AVInputInit() override
    {
        dispatcher->Deactivate();
        dispatcher->Release();
        PluginHost::IFactories::Assign(nullptr);

        plugin->Deinitialize(&service);

        IarmBus::setImpl(nullptr);
        if (p_iarmBusImplMock != nullptr) {
            delete p_iarmBusImplMock;
            p_iarmBusImplMock = nullptr;
        }
    }
};

TEST_F(AVInputTest, RegisteredMethods)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("numberOfInputs")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("currentVideoMode")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("contentProtected")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setEdid2AllmSupport")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getEdid2AllmSupport")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setVRRSupport")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getVRRSupport")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getVRRFrameRate")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getInputDevices")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("writeEDID")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("readEDID")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getRawSPD")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getSPD")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setEdidVersion")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getEdidVersion")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getHdmiVersion")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setMixerLevels")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("startInput")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("stopInput")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setVideoRectangle")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getSupportedGameFeatures")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getGameFeatureStatus")));
}

TEST_F(AVInputInit, getInputDevices)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getNumberOfInputs())
        .WillOnce(::testing::Return(1));
    EXPECT_CALL(*p_compositeInputImplMock, getNumberOfInputs())
        .WillOnce(::testing::Return(1));
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getInputDevices"), _T("{}"), response));
    EXPECT_EQ(response, string("{\"devices\":[{\"id\":0,\"connected\":false,\"locator\":\"hdmiin:\\/\\/localhost\\/deviceid\\/0\"},{\"id\":0,\"connected\":false,\"locator\":\"cvbsin:\\/\\/localhost\\/deviceid\\/0\"}],\"success\":true}"));
}

TEST_F(AVInputInit, writeEDID)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("writeEDID"), _T("{\"portId\": \"1\",\"message\":\"Test\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, readEDID)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getEDIDBytesInfo(::testing::_, ::testing::_))
        .WillOnce([](int port, std::vector<uint8_t>& edid) {
            edid = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("readEDID"), _T("{\"portId\": \"1\"}"), response));
    EXPECT_EQ(response, string("{\"EDID\":\"AP\\/\\/\\/\\/\\/\\/\\/wA=\",\"success\":true}"));
}

TEST_F(AVInputInit, getRawSPD)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getHDMISPDInfo(::testing::_, ::testing::_))
        .WillOnce([](int port, std::vector<uint8_t>& data) {
            data = { 0x53, 0x50, 0x44, 0x00 };
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getRawSPD"), _T("{\"portId\": \"1\"}"), response));
    EXPECT_EQ(response, string("{\"HDMISPD\":\"U1BEAA\",\"success\":true}"));
}

TEST_F(AVInputInit, getSPD)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getHDMISPDInfo(::testing::_, ::testing::_))
        .WillOnce([](int port, std::vector<uint8_t>& data) {
            data = { 0x53, 0x50, 0x44, 0x00 };
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getSPD"), _T("{\"portId\": \"1\"}"), response));
    EXPECT_EQ(response, string("{\"HDMISPD\":\"Packet Type:53,Version:80,Length:68,vendor name:wn,product des:,source info:00\",\"success\":true}"));
}

TEST_F(AVInputInit, setEdidVersion)
{
    EXPECT_CALL(*p_hdmiInputImplMock, setEdidVersion(::testing::_, ::testing::_))
        .WillOnce([](int port, int edidVersion) {
            EXPECT_EQ(port, 1);
            EXPECT_EQ(edidVersion, 0);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setEdidVersion"), _T("{\"portId\": \"1\", \"edidVersion\":\"HDMI1.4\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, getEdidVersion)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getEdidVersion(::testing::_, ::testing::_))
        .WillOnce([](int port, int* edidVersion) {
            *edidVersion = 4;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getEdidVersion"), _T("{\"portId\": \"1\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, getHdmiVersion)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getHdmiVersion(::testing::_, ::testing::_))
        .WillOnce([](int port, dsHdmiMaxCapabilityVersion_t* capVersion) {
            if (capVersion) {
                *capVersion = HDMI_COMPATIBILITY_VERSION_21;
            }
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getHdmiVersion"), _T("{\"portId\": \"1\"}"), response));
    EXPECT_EQ(response, string("{\"HdmiCapabilityVersion\":\"2.1\",\"success\":true}"));
}

TEST_F(AVInputInit, setMixerLevels)
{
    EXPECT_CALL(*p_HostImplMock, setAudioMixerLevels(dsAUDIO_INPUT_PRIMARY, ::testing::_))
        .WillOnce([](dsAudioInput_t input, int volume) {
            EXPECT_EQ(input, dsAUDIO_INPUT_PRIMARY);
            EXPECT_EQ(volume, 50);
        });

    EXPECT_CALL(*p_HostImplMock, setAudioMixerLevels(dsAUDIO_INPUT_SYSTEM, ::testing::_))
        .WillOnce([](dsAudioInput_t input, int volume) {
            EXPECT_EQ(input, dsAUDIO_INPUT_SYSTEM);
            EXPECT_EQ(volume, 30);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setMixerLevels"), _T("{\"primaryVolume\": 50 ,\"inputVolume\":30}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, startInput_HDMI)
{
    EXPECT_CALL(*p_hdmiInputImplMock, selectPort(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce([](int8_t Port, bool audioMix, int videoPlane, bool topMost) {
            EXPECT_EQ(Port, 1);
            EXPECT_EQ(audioMix, true);
            EXPECT_EQ(videoPlane, 1);
            EXPECT_EQ(topMost, true);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("startInput"), _T("{\"portId\": 1 ,\"typeOfInput\":\"HDMI\", \"requestAudioMix\": true, \"plane\" : 1, \"topMost\" : true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, startInput_COMPOSITE)
{
    EXPECT_CALL(*p_compositeInputImplMock, selectPort(::testing::_))
        .WillOnce([](int8_t Port) {
            EXPECT_EQ(Port, 2);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("startInput"), _T("{\"portId\": 2 ,\"typeOfInput\":\"COMPOSITE\", \"requestAudioMix\": true, \"plane\" : 1, \"topMost\" : true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, stopInput_HDMI)
{
    EXPECT_CALL(*p_hdmiInputImplMock, selectPort(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce([](int8_t Port, bool audioMix, int videoPlane, bool topMost) {
            EXPECT_EQ(Port, -1);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("stopInput"), _T("{\"typeOfInput\":\"HDMI\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, stopInput_COMPOSITE)
{
    EXPECT_CALL(*p_compositeInputImplMock, selectPort(::testing::_))
        .WillOnce([](int8_t Port) {
            EXPECT_EQ(Port, -1);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("stopInput"), _T("{\"typeOfInput\":\"COMPOSITE\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, setVideoRectangle_HDMI)
{
    EXPECT_CALL(*p_hdmiInputImplMock, scaleVideo(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce([](int32_t x, int32_t y, int32_t width, int32_t height) {
            EXPECT_EQ(x, 0);
            EXPECT_EQ(y, 0);
            EXPECT_EQ(width, 3840);
            EXPECT_EQ(height, 2160);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setVideoRectangle"), _T("{\"x\" : 0, \"y\" : 0, \"w\" : 3840, \"h\" : 2160 ,\"typeOfInput\":\"HDMI\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, setVideoRectangle_COMPOSITE)
{
    EXPECT_CALL(*p_compositeInputImplMock, scaleVideo(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce([](int32_t x, int32_t y, int32_t width, int32_t height) {
            EXPECT_EQ(x, 0);
            EXPECT_EQ(y, 0);
            EXPECT_EQ(width, 720);
            EXPECT_EQ(height, 480);
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setVideoRectangle"), _T("{\"x\" : 0, \"y\" : 0, \"w\" : 720, \"h\" : 480 ,\"typeOfInput\":\"COMPOSITE\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputInit, getSupportedGameFeatures)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getSupportedGameFeatures(::testing::_))
        .WillOnce([](std::vector<std::string>& featureList) {
            featureList = { "ALLM", "VRR", "QMS" };
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getSupportedGameFeatures"), _T("{}"), response));
    EXPECT_EQ(response, string("{\"supportedGameFeatures\":[\"ALLM\",\"VRR\",\"QMS\"],\"success\":true}"));
}

TEST_F(AVInputInit, getGameFeatureStatus_ALLM)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getHdmiALLMStatus(::testing::_, ::testing::_))
        .WillOnce([](int iHdmiPort, bool* allmStatus) {
            EXPECT_EQ(iHdmiPort, 1);
            *allmStatus = true;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getGameFeatureStatus"), _T("{\"portId\" : 1, \"gameFeature\" : \"ALLM\"}"), response));
    EXPECT_EQ(response, string("{\"mode\":true,\"success\":true}"));
}

TEST_F(AVInputInit, getGameFeatureStatus_VRR_HDMI)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getVRRStatus(::testing::_, ::testing::_))
        .WillOnce([](int iHdmiPort, dsHdmiInVrrStatus_t* vrrStatus) {
            ASSERT_NE(vrrStatus, nullptr);
            vrrStatus->vrrType = dsVRR_HDMI_VRR;
            vrrStatus->vrrAmdfreesyncFramerate_Hz = 90.00;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getGameFeatureStatus"), _T("{\"portId\" : 1, \"gameFeature\" : \"VRR-HDMI\"}"), response));
    EXPECT_EQ(response, string("{\"mode\":true,\"success\":true}"));
}

TEST_F(AVInputInit, getGameFeatureStatus_VRR_FREESYNC)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getVRRStatus(::testing::_, ::testing::_))
        .WillOnce([](int iHdmiPort, dsHdmiInVrrStatus_t* vrrStatus) {
            ASSERT_NE(vrrStatus, nullptr);
            vrrStatus->vrrType = dsVRR_AMD_FREESYNC;
            vrrStatus->vrrAmdfreesyncFramerate_Hz = 100.00;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getGameFeatureStatus"), _T("{\"portId\" : 1, \"gameFeature\" : \"VRR-FREESYNC\"}"), response));
    EXPECT_EQ(response, string("{\"mode\":true,\"success\":true}"));
}

TEST_F(AVInputInit, getGameFeatureStatus_VRR_FREESYNC_PREMIUM)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getVRRStatus(::testing::_, ::testing::_))
        .WillOnce([](int iHdmiPort, dsHdmiInVrrStatus_t* vrrStatus) {
            ASSERT_NE(vrrStatus, nullptr);
            vrrStatus->vrrType = dsVRR_AMD_FREESYNC_PREMIUM;
            vrrStatus->vrrAmdfreesyncFramerate_Hz = 120.00;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getGameFeatureStatus"), _T("{\"portId\" : 1, \"gameFeature\" : \"VRR-FREESYNC-PREMIUM\"}"), response));
    EXPECT_EQ(response, string("{\"mode\":true,\"success\":true}"));
}

TEST_F(AVInputInit, getGameFeatureStatus_VRR_FREESYNC_PREMIUM_PRO)
{
    EXPECT_CALL(*p_hdmiInputImplMock, getVRRStatus(::testing::_, ::testing::_))
        .WillOnce([](int iHdmiPort, dsHdmiInVrrStatus_t* vrrStatus) {
            ASSERT_NE(vrrStatus, nullptr);
            vrrStatus->vrrType = dsVRR_AMD_FREESYNC_PREMIUM_PRO;
            vrrStatus->vrrAmdfreesyncFramerate_Hz = 144.00;
        });

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getGameFeatureStatus"), _T("{\"portId\" : 1, \"gameFeature\" : \"VRR-FREESYNC-PREMIUM-PRO\"}"), response));
    EXPECT_EQ(response, string("{\"mode\":true,\"success\":true}"));
}

TEST_F(AVInputInit, onDevicesChangedHDMI)
{
    ASSERT_TRUE(dsAVEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_connect.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_connect.isPortConnected = true;

    EVENT_SUBSCRIBE(0, _T("onDevicesChanged"), _T("client.events.onDevicesChanged"), message);

    dsAVEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_HOTPLUG, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onDevicesChanged"), _T("client.events.onDevicesChanged"), message);
}

TEST_F(AVInputInit, onDevicesChangedCOMPOSITE)
{
    ASSERT_TRUE(dsAVEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_connect.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_connect.isPortConnected = true;

    EVENT_SUBSCRIBE(0, _T("onDevicesChanged"), _T("client.events.onDevicesChanged"), message);

    dsAVEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_HOTPLUG, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onDevicesChanged"), _T("client.events.onDevicesChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedStableHDMI)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_sig_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_sig_status.status = dsHDMI_IN_SIGNAL_STATUS_STABLE;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedNoSignalHDMI)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_sig_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_sig_status.status = dsHDMI_IN_SIGNAL_STATUS_NOSIGNAL;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedUnstableHDMI)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_sig_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_sig_status.status = dsHDMI_IN_SIGNAL_STATUS_UNSTABLE;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedNotSupportedHDMI)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_sig_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_sig_status.status = dsHDMI_IN_SIGNAL_STATUS_NOTSUPPORTED;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedDefaultHDMI)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_sig_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_sig_status.status = dsHDMI_IN_SIGNAL_STATUS_MAX;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_SIGNAL_STATUS, &eventData, 0);
    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedStableCOMPOSITE)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_sig_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_sig_status.status = dsCOMP_IN_SIGNAL_STATUS_STABLE;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedNoSignalCOMPOSITE)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_sig_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_sig_status.status = dsCOMP_IN_SIGNAL_STATUS_NOSIGNAL;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedUnstableCOMPOSITE)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_sig_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_sig_status.status = dsCOMP_IN_SIGNAL_STATUS_UNSTABLE;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedNotSupportedCOMPOSITE)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_sig_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_sig_status.status = dsCOMP_IN_SIGNAL_STATUS_NOTSUPPORTED;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onSignalChangedDefaultCOMPOSITE)
{
    ASSERT_TRUE(dsAVSignalStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_sig_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_sig_status.status = dsCOMP_IN_SIGNAL_STATUS_MAX;

    EVENT_SUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);

    dsAVSignalStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_SIGNAL_STATUS, &eventData, 0);
    EVENT_UNSUBSCRIBE(0, _T("onSignalChanged"), _T("client.events.onSignalChanged"), message);
}

TEST_F(AVInputInit, onInputStatusChangeOn_HDMI)
{
    ASSERT_TRUE(dsAVStatusEventHandler != nullptr);

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("startInput"), _T("{\"portId\": \"0\" , \"typeOfInput\":\"HDMI\", \"requestAudioMix\": true, \"plane\" : 1, \"topMost\" : true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_status.isPresented = true;

    EVENT_SUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);

    dsAVStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);
}

TEST_F(AVInputInit, onInputStatusChangeOff_HDMI)
{
    ASSERT_TRUE(dsAVStatusEventHandler != nullptr);

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("stopInput"), _T("{\"typeOfInput\":\"HDMI\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_status.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_status.isPresented = false;

    EVENT_SUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);

    dsAVStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);
}

TEST_F(AVInputInit, onInputStatusChangeOn_COMPOSITE)
{
    ASSERT_TRUE(dsAVStatusEventHandler != nullptr);

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("startInput"), _T("{\"portId\": \"0\" , \"typeOfInput\":\"COMPOSITE\", \"requestAudioMix\": true, \"plane\" : 1, \"topMost\" : true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_status.isPresented = true;

    EVENT_SUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);

    dsAVStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);
}

TEST_F(AVInputInit, onInputStatusChangeOff_COMPOSITE)
{
    ASSERT_TRUE(dsAVStatusEventHandler != nullptr);

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("stopInput"), _T("{\"typeOfInput\":\"COMPOSITE\"}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_status.port = dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_status.isPresented = false;

    EVENT_SUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);

    dsAVStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("onInputStatusChanged"), _T("client.events.onInputStatusChanged"), message);
}

TEST_F(AVInputInit, hdmiGameFeatureStatusUpdate)
{
    ASSERT_TRUE(dsAVGameFeatureStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_allm_mode.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_allm_mode.allm_mode = true;
    EVENT_SUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);

    dsAVGameFeatureStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_ALLM_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);
}

TEST_F(AVInputInit, hdmiGameFeatureStatusUpdate_HDMI_VRR)
{
    ASSERT_TRUE(dsAVGameFeatureStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_vrr_mode.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_vrr_mode.vrr_type = dsVRR_HDMI_VRR;
    EVENT_SUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);

    dsAVGameFeatureStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VRR_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);
}

TEST_F(AVInputInit, hdmiGameFeatureStatusUpdate_AMD_FREESYNC)
{
    ASSERT_TRUE(dsAVGameFeatureStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_vrr_mode.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_vrr_mode.vrr_type = dsVRR_AMD_FREESYNC;
    EVENT_SUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);

    dsAVGameFeatureStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VRR_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);
}

TEST_F(AVInputInit, hdmiGameFeatureStatusUpdate_AMD_FREESYNC_PREMIUM)
{
    ASSERT_TRUE(dsAVGameFeatureStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_vrr_mode.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_vrr_mode.vrr_type = dsVRR_AMD_FREESYNC_PREMIUM;
    EVENT_SUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);

    dsAVGameFeatureStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VRR_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);
}

TEST_F(AVInputInit, hdmiGameFeatureStatusUpdate_AMD_FREESYNC_PREMIUM_PRO)
{
    ASSERT_TRUE(dsAVGameFeatureStatusEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_vrr_mode.port = dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_vrr_mode.vrr_type = dsVRR_AMD_FREESYNC_PREMIUM_PRO;
    EVENT_SUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);

    dsAVGameFeatureStatusEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VRR_STATUS, &eventData, 0);

    EVENT_UNSUBSCRIBE(0, _T("hdmiGameFeatureStatusUpdate"), _T("client.events.hdmiGameFeatureStatusUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate1_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1920x1080;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_59dot94;

    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);

    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);

    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate2_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_720x480;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_24;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);

    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);

    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate3_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_720x576;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_25;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);

}

TEST_F(AVInputInit, videoStreamInfoUpdate4_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_3840x2160;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_30;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);

}

TEST_F(AVInputInit, videoStreamInfoUpdate5_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_4096x2160;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_50;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate6_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_4096x2160;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_60;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate7_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_4096x2160;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_23dot98;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate8_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_4096x2160;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_29dot97;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate9_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_29dot97;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate10_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_100;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate11_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_119dot88;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate12_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_120;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate13_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_200;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate14_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_239dot76;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate15_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_1280x720;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_240;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdateDefault_HDMI)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_video_mode.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_MAX;
    eventData.data.hdmi_in_video_mode.resolution.interlaced = true;
    eventData.data.hdmi_in_video_mode.resolution.frameRate= dsVIDEO_FRAMERATE_MAX;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate1_COMPOSITE)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_video_mode.port =dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_720x480;
    eventData.data.composite_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_24;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdate2_COMPOSITE)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_video_mode.port =dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_720x576;
    eventData.data.composite_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_25;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, videoStreamInfoUpdateDefault_COMPOSITE)
{
   ASSERT_TRUE(dsAVVideoModeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.composite_in_video_mode.port =dsCOMPOSITE_IN_PORT_0;
    eventData.data.composite_in_video_mode.resolution.pixelResolution = dsVIDEO_PIXELRES_MAX;
    eventData.data.composite_in_video_mode.resolution.frameRate = dsVIDEO_FRAMERATE_MAX;
    EVENT_SUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
    dsAVVideoModeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_COMPOSITE_IN_VIDEO_MODE_UPDATE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("videoStreamInfoUpdate"), _T("client.events.videoStreamInfoUpdate"), message);
}

TEST_F(AVInputInit, aviContentTypeUpdate_HDMI)
{
   ASSERT_TRUE(dsAviContentTypeEventHandler != nullptr);

    IARM_Bus_DSMgr_EventData_t eventData;
    eventData.data.hdmi_in_content_type.port =dsHDMI_IN_PORT_0;
    eventData.data.hdmi_in_content_type.aviContentType = dsAVICONTENT_TYPE_GRAPHICS;
    EVENT_SUBSCRIBE(0, _T("aviContentTypeUpdate"), _T("client.events.aviContentTypeUpdate"), message);
    dsAviContentTypeEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_AVI_CONTENT_TYPE, &eventData , 0);
    EVENT_UNSUBSCRIBE(0, _T("aviContentTypeUpdate"), _T("client.events.aviContentTypeUpdate"), message);
}

TEST_F(AVInputTest, contentProtected)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("contentProtected"), _T("{}"), response));
    EXPECT_EQ(response, string("{\"isContentProtected\":true,\"success\":true}"));
}

TEST_F(AVInputDsTest, numberOfInputs)
{
    ON_CALL(*p_hdmiInputImplMock, getNumberOfInputs())
        .WillByDefault(::testing::Return(1));

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("numberOfInputs"), _T("{}"), response));
    EXPECT_EQ(response, string("{\"numberOfInputs\":1,\"success\":true}"));
}

TEST_F(AVInputDsTest, currentVideoMode)
{
    ON_CALL(*p_hdmiInputImplMock, getCurrentVideoMode())
        .WillByDefault(::testing::Return(string("unknown")));

    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("currentVideoMode"), _T("{}"), response));
    EXPECT_EQ(response, string("{\"currentVideoMode\":\"unknown\",\"success\":true}"));
}

TEST_F(AVInputDsTest, getEdid2AllmSupport)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getEdid2AllmSupport"), _T("{\"portId\": \"0\",\"allmSupport\":true}"), response));
    EXPECT_EQ(response, string("{\"allmSupport\":true,\"success\":true}"));
}


TEST_F(AVInputDsTest, setEdid2AllmSupport)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setEdid2AllmSupport"), _T("{\"portId\": \"0\",\"allmSupport\":true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputDsTest, getVRRSupport)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getVRRSupport"), _T("{\"portId\": \"0\",\"vrrSupport\":true}"), response));
    EXPECT_EQ(response, string("{\"vrrSupport\":true,\"success\":true}"));
}

TEST_F(AVInputDsTest, setVRRSupport)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("setVRRSupport"), _T("{\"portId\": \"0\",\"vrrSupport\":true}"), response));
    EXPECT_EQ(response, string("{\"success\":true}"));
}

TEST_F(AVInputDsTest, getVRRFrameRate)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Invoke(connection, _T("getVRRFrameRate"), _T("{\"portId\": \"0\"}"), response));
    EXPECT_EQ(response, string("{\"currentVRRVideoFrameRate\":0,\"success\":true}"));
}

