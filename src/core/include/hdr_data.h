#ifndef HDR_DATA_H
#define HDR_DATA_H

namespace core
{
    struct DisplayChromacities
    {
        float RedX, RedY;
        float GreenX, GreenY;
        float BlueX, BlueY;
        float WpX, WpY;
    };

    const DisplayChromacities DisplayChromacityList[] =
        {
            {0.64000f, 0.33000f, 0.30000f, 0.60000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f},  // DG_Rec709
            {0.68000f, 0.32000f, 0.26500f, 0.69000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f},  // DG_DCI-P3 D65
            {0.70800f, 0.29200f, 0.17000f, 0.79700f, 0.13100f, 0.04600f, 0.31270f, 0.32900f},  // DG_Rec2020
            {0.73470f, 0.26530f, 0.00000f, 1.00000f, 0.00010f, -0.07700f, 0.32168f, 0.33767f}, // DG_ACES
            {0.71300f, 0.29300f, 0.16500f, 0.83000f, 0.12800f, 0.04400f, 0.32168f, 0.33767f},  // DG_ACEScg
    };

    struct HdrData
    {
        float MaxOutputNits = 1000.0f;
        float MinOutputNits = 0.0f;
        float MaxCLL = 0.0f; // 1000
        float MaxFALL = 0.0f; // 180
    };
} // namespace core

#endif