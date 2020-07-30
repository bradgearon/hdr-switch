#include <string.h>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define TRAY_APPINDICATOR 1
#elif defined(__APPLE__) || defined(__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"

#if TRAY_APPINDICATOR
#define TRAY_ICON1 "indicator-messages"
#define TRAY_ICON2 "indicator-messages-new"
#elif TRAY_APPKIT
#define TRAY_ICON1 "icon.png"
#define TRAY_ICON2 "icon.png"
#elif TRAY_WINAPI
#define TRAY_ICON1 "icon.ico"
#define TRAY_ICON2 "icon.ico"
#endif

#include "hdr_mode.h"
#include "toggle.h"

using namespace core;

static struct tray tray;

static tray_menu *lastItem = NULL;
static HDR_MODE currentMode = HDR_MODE::NONE;
static std::unique_ptr<Toggle> hdrToggle;

static void toggleItem(struct tray_menu *item)
{

  item->checked = !item->checked;
  tray_update(&tray);
}
static void quit(struct tray_menu *item)
{
  (void)item;
  tray_exit();
}

static void setHdr(struct tray_menu *item, HDR_MODE mode)
{

  if (lastItem != NULL)
  {
    toggleItem(lastItem);
  }

  if (item->checked)
  {
    mode = HDR_MODE::NONE;
  }

  hdrToggle->set_hdr(mode);
  currentMode = mode;

  toggleItem(item);
  lastItem = item;
}

static void setHdr444(struct tray_menu *item) { setHdr(item, HDR_MODE::YUV444); }
static void setHdr422(struct tray_menu *item) { setHdr(item, HDR_MODE::YUV422_10); }
static void setHdr420(struct tray_menu *item) { setHdr(item, HDR_MODE::YUV420_12); }
static void setHdrRGB(struct tray_menu *item) { setHdr(item, HDR_MODE::RGB); }

static tray_menu hdrTrayMenu[] = {
    {.text = "YUV444 8 bit", .cb = setHdr444, .submenu = NULL},
    {.text = "YUV422 12 bit", .cb = setHdr422, .submenu = NULL},
    {.text = "YUV420 12 bit", .cb = setHdr420, .submenu = NULL},
    {.text = "RGB 8 bit", .cb = setHdrRGB, .submenu = NULL}
};
static struct tray hdrTray = {
    .icon = TRAY_ICON1,
    .menu = hdrTrayMenu,
};

int main()
{
  hdrToggle = std::make_unique<Toggle>();

  tray = hdrTray;
  if (tray_init(&tray) < 0)
  {
    return 1;
  }
  while (tray_loop(1) == 0)
  {
  }
  return 0;
}