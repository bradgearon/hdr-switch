#ifndef SDK_STATUS_PUB_H
#define SDK_STATUS_PUB_H

namespace core
{
    struct SdkStatus
    {
        const char *Message;
        bool IsSuccessful = false;
    };

} // namespace core
#endif