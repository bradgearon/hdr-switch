#pragma once
#include "winlamb/internals/base_focus_pubm.h"
#include "winlamb/internals/base_native_ctrl_pubm.h"
#include "winlamb/internals/styler.h"
#include "winlamb/wnd.h"
#include "winlamb/internals/base_msg_pubm.h"
#include <CommCtrl.h>

using namespace wl;
class trackbar final : public wnd,
					   public _wli::base_native_ctrl_pubm<trackbar>,
					   public _wli::base_focus_pubm<trackbar>,
					   public _wli::base_msg_pubm<LRESULT>
{
private:
	_wli::base_msg<LRESULT> _baseMsg{_hWnd};
	class _styler final : public _wli::styler<trackbar>
	{
	public:
		explicit _styler(trackbar *pDtp) noexcept : styler(pDtp) {}

		trackbar &autoticks_control(bool doSet) noexcept
		{
			return this->set_style(doSet, TBS_AUTOTICKS);
		}
	};

	HWND _hWnd = nullptr;
	_wli::base_native_ctrl _baseNativeCtrl{_hWnd};

public:
	// Wraps window style changes done by Get/SetWindowLongPtr.
	_styler style{this};

	trackbar() noexcept : wnd(_hWnd), base_msg_pubm(_baseMsg), base_native_ctrl_pubm(_baseNativeCtrl), base_focus_pubm(_hWnd) {}

	trackbar(trackbar &&) = default;
	trackbar &operator=(trackbar &&) = default; // movable only

	trackbar &create(HWND hParent, int ctrlId, POINT pos, LONG width)
	{
		this->_baseNativeCtrl.create(hParent, ctrlId, nullptr, pos, {width, 21}, TRACKBAR_CLASSW);
		return *this;
	}

	trackbar &create(const wnd *parent, int ctrlId, POINT pos, LONG width)
	{
		return this->create(parent->hwnd(), ctrlId, pos, width);
	}

	uint32_t get_position() const noexcept
	{
		auto position = SendMessageW(this->_hWnd, TBM_GETPOS, 0, 0);
		return position;
	}

	trackbar &set_position(uint32_t position) noexcept
	{
		SendMessageW(this->_hWnd, TBM_SETPOS, (WPARAM)TRUE, position);
		return *this;
	}
};
