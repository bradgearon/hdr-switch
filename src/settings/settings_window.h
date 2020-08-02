#include <functional>

#include "hdr_data.h"
#include "winlamb/window_main.h"
#include "winlamb/label.h"
#include "trackbar_control.h"

class SettingsWindow : public wl::window_main
{
public:
    SettingsWindow(std::function<void(core::HdrData)> setHdrData);

private:
    wl::label whiteLabel;
    u_int whiteLabelId = 1101;

    trackbar whiteSlider;
    u_int whiteSliderId = 1102;

    wl::label whiteValueLabel;
    u_int whiteValueLabelId = 1103;

    wl::label blackLabel;
    u_int blackLabelId = 1104;

    trackbar blackSlider;
    u_int blackSliderId = 1105;

    wl::label blackValueLabel;
    u_int blackValueLabelId = 1106;

    
};