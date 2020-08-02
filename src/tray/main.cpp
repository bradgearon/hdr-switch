#include <string>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
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
#define TRAY_ICON1 "hdr_switch_tray.exe"
#define TRAY_ICON2 "hdr_switch_tray.exe"
#endif

#include "color_mode.h"
#include "toggle.h"
#include "service/host.h"
#include "settings/main.h"

using namespace core;

static struct tray tray;
static std::unique_ptr<Toggle> hdrToggle;
static std::unique_ptr<std::thread> serviceThread;

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

static void showError(const char *msg)
{
  MessageBox(NULL, msg, "HDR Switch Error", MB_OK | MB_ICONWARNING);
}

static void setHdrData(HdrData data) {
  hdrToggle->setHdrData(data);
}

static void openSettings(struct tray_menu *item)
{
  Main settingsWindow = Main(setHdrData);
  settingsWindow.start(wc.hInstance);
}

static void setHdrMode(struct tray_menu *item)
{
  auto enabled = !item->checked;

  auto status = hdrToggle->setHdrMode(enabled);
  if (!status.IsSuccessful)
  {
    auto msg = "Error setting hdr mode: " + status.Message;
    showError(msg.c_str());
  }

  toggleItem(item);
}

static void setColorMode(struct tray_menu *item, COLOR_MODE mode)
{
  static auto lastColorItem{item};

  if (item->checked)
  {
    return;
  }

  if (item != lastColorItem)
  {
    toggleItem(lastColorItem);
  }

  auto status = hdrToggle->setColorMode(mode);
  if (!status.IsSuccessful)
  {
    auto msg = "Error setting color mode: " + status.Message;
    showError(msg.c_str());
  }

  toggleItem(item);
  lastColorItem = item;
}

auto setColorCb = [](auto item, COLOR_MODE mode) { setColorMode(item, mode); };

static tray_menu hdrTrayMenu[] = {
    {.text = "HDR Mode", .cb = setHdrMode},
    {.text = "Settings", .cb = openSettings},
    {.text = "-"},
    {.text = "YUV444 8bit", .cb = [](auto item) { setColorMode(item, COLOR_MODE::YUV444); }},
    {.text = "YUV422 12bit", .cb = [](auto item) { setColorMode(item, COLOR_MODE::YUV422_10); }},
    {.text = "YUV420 12bit", .cb = [](auto item) { setColorMode(item, COLOR_MODE::YUV420_12); }},
    {.text = "RGB 8 bit", .cb = [](auto item) { setColorMode(item, COLOR_MODE::RGB); }},
    {.text = "RGB 10 bit", .cb = [](auto item) { setColorMode(item, COLOR_MODE::RGB_10); }},
    {.text = "-"},
    {.text = "Exit", .last = true, .cb = quit},
};

static struct tray hdrTray = {
    .icon = TRAY_ICON1,
    .menu = hdrTrayMenu,
};

service::Host host([]() {
  setHdrMode(&hdrTrayMenu[0]);
});

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    serviceThread = std::make_unique<std::thread>(host.start());

    hdrToggle = std::make_unique<Toggle>();

    tray = hdrTray;
    if (tray_init(&tray) < 0)
    {
      return 1;
    }
    while (tray_loop(1) == 0)
    {
    }
  }

  std::vector<std::string> args(argv, argv + argc);

  auto isToggle = args[1].find("hdr");
  if (isToggle != std::string::npos)
  {
    host.sendToggle();
    return 0;
  }

  return 0;
}