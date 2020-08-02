#include <tuple>
#include <memory>

#include "include/hdr_data.h"
#include "toggle.h"
#include "sdk_status.h"

namespace core
{
    Toggle::Toggle()
    {
        auto status = SdkStatusImpl(NvAPI_Initialize());

        if (!status.IsSuccessful)
        {
            // std::cerr << status.Message;
        }
    }
    Toggle::~Toggle()
    {
        NvAPI_Unload();
    }

    void calcMasteringData(NV_HDR_COLOR_DATA hdrData, HdrData data, DisplayChromacities chroma)
    {
        hdrData.mastering_display_data.displayPrimary_x0 = NvU16(chroma.RedX * 50000.0f);
        hdrData.mastering_display_data.displayPrimary_y0 = NvU16(chroma.RedY * 50000.0f);
        hdrData.mastering_display_data.displayPrimary_x1 = NvU16(chroma.GreenX * 50000.0f);
        hdrData.mastering_display_data.displayPrimary_y1 = NvU16(chroma.GreenY * 50000.0f);
        hdrData.mastering_display_data.displayPrimary_x2 = NvU16(chroma.BlueX * 50000.0f);
        hdrData.mastering_display_data.displayPrimary_y2 = NvU16(chroma.BlueY * 50000.0f);
        hdrData.mastering_display_data.displayWhitePoint_x = NvU16(chroma.WpX * 50000.0f);
        hdrData.mastering_display_data.displayWhitePoint_y = NvU16(chroma.WpY * 50000.0f);
        hdrData.mastering_display_data.max_display_mastering_luminance = NvU16(data.MaxOutputNits);
        hdrData.mastering_display_data.min_display_mastering_luminance = NvU16(data.MinOutputNits);
        hdrData.mastering_display_data.max_content_light_level = NvU16(data.MaxCLL);
        hdrData.mastering_display_data.max_frame_average_light_level = NvU16(data.MaxFALL);
    }

    NV_COLOR_DATA ToggleImpl::setColorData(COLOR_MODE mode)
    {
        NV_COLOR_DATA color = {0};

        color.version = NV_COLOR_DATA_VER;
        color.cmd = NV_COLOR_CMD_SET;
        color.size = sizeof(NV_COLOR_DATA);

        auto format = NV_COLOR_FORMAT_RGB;
        auto bpc = NV_BPC_8;

        switch (mode)
        {
        case COLOR_MODE::RGB_10:
            format = NV_COLOR_FORMAT_RGB;
            bpc = NV_BPC_10;
            // color.data.depth = NV_DESKTOP_COLOR_DEPTH_16BPC_FLOAT_HDR;
            break;
        case COLOR_MODE::YUV420_12:
            format = NV_COLOR_FORMAT_YUV420;
            bpc = NV_BPC_12;
            break;
        case COLOR_MODE::YUV422_10:
            format = NV_COLOR_FORMAT_YUV422;
            bpc = NV_BPC_12;
            break;
        case COLOR_MODE::YUV444:
            format = NV_COLOR_FORMAT_YUV444;
            bpc = NV_BPC_8;
            break;
        }

        color.data.colorFormat = format;
        color.data.bpc = bpc;

        color.data.dynamicRange = NV_DYNAMIC_RANGE_AUTO;

        return color;
    }

    NV_HDR_COLOR_DATA ToggleImpl::setHdrData(bool enabled)
    {
        NV_HDR_COLOR_DATA color = {0};

        color.version = NV_HDR_COLOR_DATA_VER;
        color.cmd = NV_HDR_CMD_SET;

        if (enabled)
        {
            color.hdrColorFormat = NV_COLOR_FORMAT_RGB;
            color.hdrBpc = NV_BPC_8;
        }

        color.hdrDynamicRange = NV_DYNAMIC_RANGE_AUTO;
        color.hdrMode = enabled ? NV_HDR_MODE_UHDA : NV_HDR_MODE_OFF;

        return color;
    }

    SdkStatus Toggle::setHdrData(HdrData data)
    {
        NV_HDR_COLOR_DATA color = {0};

        color.version = NV_HDR_COLOR_DATA_VER;
        color.cmd = NV_HDR_CMD_SET;

        auto chroma = DisplayChromacityList[2];

        calcMasteringData(color, data, chroma);

        auto impl = (ToggleImpl *)this;

        auto [dispId, dispStatus] = impl->getPrimaryDispId();
        if (!dispStatus.IsSuccessful)
        {
            return dispStatus;
        }

        auto status = SdkStatusImpl(NvAPI_Disp_HdrColorControl(dispId, &color));
        return status;
    }

    SdkStatus Toggle::setColorMode(COLOR_MODE mode)
    {
        auto impl = (ToggleImpl *)this;

        auto [dispId, dispStatus] = impl->getPrimaryDispId();
        if (!dispStatus.IsSuccessful)
        {
            return dispStatus;
        }

        auto color = impl->setColorData(mode);
        auto status = SdkStatusImpl(NvAPI_Disp_ColorControl(dispId, &color));

        return status;
    }

    SdkStatus Toggle::setHdrMode(bool enabled)
    {
        auto impl = (ToggleImpl *)this;

        auto [dispId, dispStatus] = impl->getPrimaryDispId();
        if (!dispStatus.IsSuccessful)
        {
            return dispStatus;
        }

        auto color = impl->setHdrData(enabled);
        auto status = SdkStatusImpl(NvAPI_Disp_HdrColorControl(dispId, &color));

        return status;
    }

    std::tuple<unsigned long, SdkStatus> ToggleImpl::getPrimaryDispId()
    {
        NvU32 dispIdCount = 0;

        auto gpuHandles = std::make_unique<NvPhysicalGpuHandle[]>(NVAPI_MAX_PHYSICAL_GPUS);
        NvU32 numOfGPUs = 0;

        SdkStatus status = SdkStatusImpl(NvAPI_EnumPhysicalGPUs(gpuHandles.get(), &numOfGPUs));
        if (!status.IsSuccessful)
        {
            return {-1, status};
        }

        NvU32 connected_displays = 0;

        status = SdkStatusImpl(NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], NULL, &dispIdCount, NULL));
        if (!status.IsSuccessful)
        {
            return {-1, status};
        }

        auto dispIds = std::make_unique<NV_GPU_DISPLAYIDS[]>(dispIdCount);

        dispIds[0].version = NV_GPU_DISPLAYIDS_VER;

        status = SdkStatusImpl(NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], dispIds.get(), &dispIdCount, NULL));
        if (!status.IsSuccessful)
        {
            return {-1, status};
        }

        return {dispIds[0].displayId, status};
    }

} // namespace core