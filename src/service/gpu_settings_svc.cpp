#include "gpu_settings_svc.h"

#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include "gpu_shared.h"

#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <regex>
#include <atomic>
#include <future>

#include "ipc.h"

void PrintError(NvAPI_Status status)
{
    NvAPI_ShortString szDesc = {0};
    NvAPI_GetErrorMessage(status, szDesc);
    printf(" NVAPI error: %s\n", szDesc);
    // exit(-1);
}
void calcMasteringData(NV_HDR_COLOR_DATA *hdrData)
{
    double rx = 0.64;
    double ry = 0.33;
    double gx = 0.30;
    double gy = 0.60;
    double bx = 0.15;
    double by = 0.06;
    double wx = 0.3127;
    double wy = 0.3290;
    double minMaster = 1.0;
    double maxMaster = 1000;
    double maxCLL = 1000;
    double maxFALL = 100;

    hdrData->mastering_display_data.displayPrimary_x0 = (NvU16)ceil(rx * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayPrimary_y0 = (NvU16)ceil(ry * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayPrimary_x1 = (NvU16)ceil(gx * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayPrimary_y1 = (NvU16)ceil(gy * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayPrimary_x2 = (NvU16)ceil(bx * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayPrimary_y2 = (NvU16)ceil(by * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayWhitePoint_x = (NvU16)ceil(wx * 0xC350 + 0.5);
    hdrData->mastering_display_data.displayWhitePoint_y = (NvU16)ceil(wy * 0xC350 + 0.5);
    hdrData->mastering_display_data.max_content_light_level = (NvU16)ceil(maxCLL + 0.5);
    hdrData->mastering_display_data.max_display_mastering_luminance = (NvU16)ceil(maxMaster + 0.5);
    hdrData->mastering_display_data.max_frame_average_light_level = (NvU16)ceil(maxFALL + 0.5);
    hdrData->mastering_display_data.min_display_mastering_luminance = (NvU16)ceil(minMaster * 10000.0 + 0.5);
}

void setHdrDolbyVision(NV_HDR_COLOR_DATA *hdrData)
{
    hdrData->hdrBpc = NV_BPC_8;
    hdrData->hdrColorFormat = NV_COLOR_FORMAT_RGB;
    hdrData->hdrDynamicRange = NV_DYNAMIC_RANGE_AUTO;

    hdrData->hdrMode = NV_HDR_MODE_DOLBY_VISION;
}

void setColorYcc422(NvU32 dispId)
{

    NV_COLOR_DATA color;

    color.version = NV_COLOR_DATA_VER;
    color.size = sizeof(NV_COLOR_DATA);
    color.cmd = NV_COLOR_CMD_SET;

    color.data.colorFormat = NV_COLOR_FORMAT_YUV422;
    color.data.colorimetry = NV_COLOR_COLORIMETRY_BT2020YCC;
    color.data.dynamicRange = NV_DYNAMIC_RANGE_CEA;
    color.data.bpc = NV_BPC_12;
    color.data.colorSelectionPolicy = NV_COLOR_SELECTION_POLICY_USER;

    auto status = NvAPI_Disp_ColorControl(dispId, &color);
    if (status != NVAPI_OK)
    {
        PrintError(status);
    }
}

void setHdrYcc422(NV_HDR_COLOR_DATA *hdrData, NvU32 dispId)
{
    hdrData->hdrBpc = NV_BPC_12;
    hdrData->hdrColorFormat = NV_COLOR_FORMAT_YUV422;
    hdrData->hdrDynamicRange = NV_DYNAMIC_RANGE_AUTO;

    NV_HDR_CAPABILITIES hdrCap;
    NvAPI_Disp_GetHdrCapabilities(dispId, &hdrCap);

    hdrData->mastering_display_data.displayPrimary_x0 = 0;
    hdrData->mastering_display_data.displayPrimary_y0 = 0;
    hdrData->mastering_display_data.displayPrimary_x1 = 25938;
    hdrData->mastering_display_data.displayPrimary_y1 = 13261;
    hdrData->mastering_display_data.displayPrimary_x2 = 32758;
    hdrData->mastering_display_data.displayPrimary_y2 = 0;
    hdrData->mastering_display_data.displayWhitePoint_x = 13888;
    hdrData->mastering_display_data.displayWhitePoint_y = 62399;
    hdrData->mastering_display_data.max_content_light_level = 32760;
    hdrData->mastering_display_data.max_frame_average_light_level = 29136;

    hdrData->mastering_display_data.max_display_mastering_luminance = 39224;
    hdrData->mastering_display_data.min_display_mastering_luminance = 64036;

    hdrData->hdrMode = NV_HDR_MODE_UHDA;
}

void set_hdr_on()
{
    NvAPI_Status status;
    // (0) Initialize NVAPI. This must be done first of all
    status = NvAPI_Initialize();
    if (status != NVAPI_OK)
        PrintError(status);

    NvU32 dispIdCount = 0;

    auto gpuHandles = std::make_unique<NvPhysicalGpuHandle[]>(NVAPI_MAX_PHYSICAL_GPUS);
    NvU32 numOfGPUs = 0;

    NvAPI_EnumPhysicalGPUs(gpuHandles.get(), &numOfGPUs);

    NvU32 connected_displays = 0;

    NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], NULL,
                                     &dispIdCount, NULL);

    auto dispIds = std::make_unique<NV_GPU_DISPLAYIDS[]>(dispIdCount);

    dispIds[0].version = NV_GPU_DISPLAYIDS_VER;

    NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], dispIds.get(),
                                     &dispIdCount, NULL);

    NV_HDR_COLOR_DATA hdrData = {0};

    hdrData.cmd = NV_HDR_CMD_SET;
    hdrData.version = NV_HDR_COLOR_DATA_VER;

    setHdrYcc422(&hdrData, dispIds[0].displayId);

    status = NvAPI_Disp_HdrColorControl(dispIds[0].displayId, &hdrData);
    if (status != NVAPI_OK)
        PrintError(status);
}

void GpuSettingsService::OnStart(DWORD /*argc*/, TCHAR ** /*argv[]*/)
{
    this->WriteToEventLog("staring");
    this->WriteToEventLog("made lambda");

    this->msgThread.push_back(std::thread([&, this] {
        auto error = NvAPI_Initialize();
        if (error != NVAPI_OK)
        {
            this->WriteToEventLog("error initing nv api");
        }

        ipc::route cc{pipename};
        uint16_t a = 0;

        while (1)
        {
            auto dd = cc.recv(1000);
            if (dd.empty())
            {
                continue;
            }

            auto msg = static_cast<char *>(dd.data());
            if (msg == nullptr || msg[0] == '\0')
                return;

            this->WriteToEventLog("recv");
            std::string str(msg);

            if (str.find("set") != std::string::npos)
            {
                set_hdr_on();
            }
        }
    }));

    this->WriteToEventLog("started");

    // SetStatus(SERVICE_RUNNING);
}

void GpuSettingsService::OnStop()
{
    NvAPI_Unload();
}

void GpuSettingsService::OnSessionChange(DWORD evtType,
                                         WTSSESSION_NOTIFICATION *notification)
{
}