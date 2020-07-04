#include "gpu_settings_svc.h"
#include "service_installer.h"
int service(bool install, std::string command) {
  GpuSettingsService service;

  if (install) {
    if (command == "install") {
      printf("Installing service\n");
      if (!ServiceInstaller::Install(service)) {
        printf("Couldn't install service: %d\n", ::GetLastError());
        return -1;
      }

      printf("Service installed\n");
      return 0;
    }
    
    if (command == "uninstall") {
      printf("Uninstalling service\n");
      if (!ServiceInstaller::Uninstall(service)) {
        printf(("Couldn't uninstall service: %d\n"), ::GetLastError());
        return -1;
      }

      printf("Service uninstalled\n");
      return 0;
    }

    printf("Invalid argument\n");
    return -1;
  }
  
  service.Run();

  return 0;
}