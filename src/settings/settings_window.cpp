#include <string>

#include "settings_window.h"
using namespace wl;

static core::HdrData maxHdrData{
    .MaxOutputNits = 5000.0f, // 4000 .8
    .MinOutputNits = 0.0f,    // ?
    .MaxCLL = 2000.0f,        // 1000 .5
    .MaxFALL = 360.0f,        // 180 .5
};

SettingsWindow::SettingsWindow(std::function<void(core::HdrData)> setHdrData)
{
    static auto title = L"HDR Settings";

    static core::HdrData hdrData{
        .MaxOutputNits = 5000.0f, // 4000 .8
        .MinOutputNits = 0.0f,    // ?
        .MaxCLL = 2000.0f,        // 1000 .5
        .MaxFALL = 360.0f,        // 180 .5
    };

    setup.wndClassEx.lpszClassName = L"HDR_SETTINGS_MAIN"; // class name to be registered
    setup.title = title;
    setup.style |= WS_MINIMIZEBOX | WS_SIZEBOX;
    setup.size = {640, 480};

    on_message(WM_CREATE, [this](wl::wm::create p) -> LRESULT {
        set_text(title);

        auto px = 6, py = 6;
        auto sx = px * 2, sy = px * 2;
        auto cx = sx, cy = sy;
        auto cw = 0, ch = 0;

        whiteLabel.create(this->hwnd(), whiteLabelId, L"white", {cx, cy}, {cw = 48, ch = 18});
        cx += cw;
        cx += px * 2;

        whiteSlider.create(this->hwnd(), whiteSliderId, {cx, cy}, cw = 96);
        whiteSlider.set_position(0);
        cx += cw;
        cx += px * 1;

        whiteValueLabel.create(this->hwnd(), whiteValueLabelId, L"0", {cx, cy}, {cw = 32, ch});

        // row
        cy += ch;
        cy += py * 2;
        cx = sx;

        blackLabel.create(this->hwnd(), blackLabelId, L"black", {cx, cy}, {cw = 48, ch = 18});
        cx += cw;
        cx += px * 2;

        blackSlider.create(this->hwnd(), blackSliderId, {cx, cy}, cw = 96);
        blackSlider.set_position(0);
        cx += cw;
        cx += px * 1;

        blackValueLabel.create(this->hwnd(), blackValueLabelId, L"0", {cx, cy}, {cw = 32, ch});
        cy += ch;

        return 0;
    });

    on_message(WM_HSCROLL, [this, setHdrData](wm::hscroll params) {
        if ((params.wParam & TB_ENDTRACK) != 0)
        {
            return 0;
        }

        auto slider = &blackSlider;
        auto label = &blackValueLabel;
        std::function<void(float)> setValue = [](auto val) { hdrData.MinOutputNits = val; };
        auto maxValue = maxHdrData.MinOutputNits;

        if (whiteSlider.hwnd() == params.scrollbar())
        {
            slider = &whiteSlider;
            label = &whiteValueLabel;
            setValue = [](auto val) { hdrData.MaxOutputNits = val; };
            maxValue = maxHdrData.MaxOutputNits;
        }

        auto position = slider->get_position();
        label->set_text(std::to_wstring(position));

        float hdrValue = maxValue * position * 0.01f;
        setValue(hdrValue);

        setHdrData(hdrData);

        return 0;
    });

    on_message(WM_INITDIALOG, [&](params) -> LRESULT {
        return 0;
    });

    on_message(WM_LBUTTONDOWN, [](wl::wm::lbuttondown p) -> LRESULT {
        bool isCtrlDown = p.has_ctrl();
        long xPos = p.pos().x;
        return 0;
    });
}
