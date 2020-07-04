#ifndef SERVICE_BASE_H_
#define SERVICE_BASE_H_

#include <windows.h>
#include <atlstr.h>

// Base Service class used to create windows services.
class ServiceBase {
 public:
  ServiceBase(const ServiceBase& other) = delete;
  ServiceBase& operator=(const ServiceBase& other) = delete;

  ServiceBase(ServiceBase&& other) = delete;
  ServiceBase& operator=(ServiceBase&& other) = delete;

  virtual ~ServiceBase() {}
  
  // Called by windows when starting the service.
  bool Run() {
    return RunInternal(this);
  }

  const CString& GetName() const { return m_name; }
  const CString& GetDisplayName() const { return m_displayName; }
  const DWORD GetStartType() const { return m_dwStartType; }
  const DWORD GetErrorControlType() const { return m_dwErrorCtrlType; }
  const CString& GetDependencies() const { return m_depends; }

  // Account info service runs under.
  const CString& GetAccount() const { return m_account; }
  const CString& GetPassword() const { return m_password; }
 protected:
  ServiceBase(const CString& name,
              const CString& displayName,
              DWORD dwStartType,
              DWORD dwErrCtrlType = SERVICE_ERROR_NORMAL,
              DWORD dwAcceptedCmds = SERVICE_ACCEPT_STOP,
              const CString& depends = _T(""),
              const CString& account = _T(""),
              const CString& password = _T(""));

  void SetStatus(DWORD dwState, DWORD dwErrCode = NO_ERROR, DWORD dwWait = 0);

  // TODO(Olster): Move out of class/make static.
  // Writes |msg| to Windows event log.
  void WriteToEventLog(const CString& msg, WORD type = EVENTLOG_INFORMATION_TYPE);

  // Overro=ide these functions as you need.
  virtual void OnStart(DWORD argc, TCHAR* argv[]) = 0;
  virtual void OnStop() {}
  virtual void OnPause() {}
  virtual void OnContinue() {}
  virtual void OnShutdown() {}

  virtual void OnSessionChange(DWORD /*evtType*/,
                               WTSSESSION_NOTIFICATION* /*notification*/) {}
 private:
  // Registers handle and starts the service.
  static void WINAPI SvcMain(DWORD argc, TCHAR* argv[]);

  // Called whenever service control manager updates service status.
  static DWORD WINAPI ServiceCtrlHandler(DWORD ctrlCode, DWORD evtType,
                                         void* evtData, void* context);

  static bool RunInternal(ServiceBase* svc);

  void Start(DWORD argc, TCHAR* argv[]);
  void Stop();
  void Pause();
  void Continue();
  void Shutdown();

  CString m_name;
  CString m_displayName;
  DWORD m_dwStartType;
  DWORD m_dwErrorCtrlType;
  CString m_depends;
  CString m_account;
  CString m_password;

  // Info about service dependencies and user account.
  bool m_hasDepends = false;
  bool m_hasAcc = false;
  bool m_hasPass = false;

  SERVICE_STATUS m_svcStatus;
  SERVICE_STATUS_HANDLE m_svcStatusHandle;

  static ServiceBase* m_service;
};

#endif // SERVICE_BASE_H_
