#include "main.h"
#include "settings_window.h"
#include "hdr_data.h"

Main::Main(std::function<void(core::HdrData)> setHdrData)
{
    this->_setHdrData = setHdrData;
}

void Main::start(HINSTANCE hwnd)
{
    auto settings = SettingsWindow(this->_setHdrData);
    settings.winmain_run(hwnd, SW_NORMAL | SW_SHOW);
}