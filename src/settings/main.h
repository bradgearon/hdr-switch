#include <functional>

#include <windows.h>
#include "hdr_data.h"

class Main
{

private:
    std::function<void(core::HdrData)> _setHdrData;

public:
    Main(std::function<void(core::HdrData data)> setHdrData);
    void start(HINSTANCE hwnd);
};