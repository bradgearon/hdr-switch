#ifndef GPU_SETTINGS_SERVICE_H_
#define GPU_SETTINGS_SERVICE_H_

#include <fstream>

#include <thread>
#include <vector>

#include "nvapi.h"
#include "service_base.h"
class GpuSettingsService : public ServiceBase
{
public:
   GpuSettingsService(const GpuSettingsService &other) = delete;
   GpuSettingsService &operator=(const GpuSettingsService &other) = delete;

   GpuSettingsService(GpuSettingsService &&other) = delete;
   GpuSettingsService &operator=(GpuSettingsService &&other) = delete;

   GpuSettingsService()
       : ServiceBase(_T("GpuSettingsService"),
                     _T("gpu settings service"),
                     SERVICE_DEMAND_START,
                     SERVICE_ERROR_NORMAL,
                     SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE) {}
   void OnStart(DWORD argc, TCHAR *argv[]) override;

private:
   std::vector<std::thread> msgThread;
   void OnStop() override;
   void OnSessionChange(DWORD evtType,
                        WTSSESSION_NOTIFICATION *notification) override;

#ifdef UNICODE
   using tofstream = std::wofstream;
#else
   using tofstream = std::ofstream;
#endif

   tofstream m_logFile;
};

#endif // GPU_SETTINGS_SERVICE_H_