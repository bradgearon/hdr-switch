#ifndef TOGGLE_PUB_H
#define TOGGLE_PUB_H

#include "hdr_mode.h"

namespace core
{
    class Toggle
    {
    public:
        Toggle();
        ~Toggle();

        void set_hdr(HDR_MODE mode = HDR_MODE::YUV444);
    };

} // namespace core

#endif