#ifndef SERVICE_INSTALLER_H_
#define SERVICE_INSTALLER_H_

#include "service_base.h"

class ServiceInstaller {
public:
  static bool Install(const ServiceBase& service);
  static bool Uninstall(const ServiceBase& service);
private:
  ServiceInstaller() {}
};

#endif // SERVICE_INSTALLER_H_

