#ifndef TOGGLE_H
#define TOGGLE_H

#include "nvapi_deps.h"
#include "include/toggle.h"

namespace core
{
    class ToggleImpl : Toggle
    {

    public:
        NV_HDR_COLOR_DATA setColorData(HDR_MODE mode);
        unsigned long getPrimaryDispId();
    };

} // namespace core

#endif