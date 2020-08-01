#include "service_base.h"
#include <cassert>

ServiceBase* ServiceBase::m_service = nullptr;

ServiceBase::ServiceBase(const CString& name,
                         const CString& displayName,
                         DWORD dwStartType,
                         DWORD dwErrCtrlType,
                         DWORD dwAcceptedCmds,
                         const CString& depends,
                         const CString& account,
                         const CString& password)
 : m_name(name),
   m_displayName(displayName),
   m_dwStartType(dwStartType),
   m_dwErrorCtrlType(dwErrCtrlType),
   m_depends(depends),
   m_account(account),
   m_password(password),
   m_svcStatusHandle(nullptr) {  

  m_svcStatus.dwControlsAccepted = dwAcceptedCmds;
  m_svcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  m_svcStatus.dwCurrentState = SERVICE_START_PENDING;
  m_svcStatus.dwWin32ExitCode = NO_ERROR;
  m_svcStatus.dwServiceSpecificExitCode = 0;
  m_svcStatus.dwCheckPoint = 0;
  m_svcStatus.dwWaitHint = 0;
}

void ServiceBase::SetStatus(DWORD dwState, DWORD dwErrCode, DWORD dwWait) {
  m_svcStatus.dwCurrentState = dwState;
  m_svcStatus.dwWin32ExitCode = dwErrCode;
  m_svcStatus.dwWaitHint = dwWait;

  ::SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}

void ServiceBase::WriteToEventLog(const CString& msg, WORD type) {
  HANDLE hSource = RegisterEventSource(nullptr, m_name);
  if (hSource) {
    const TCHAR* msgData[2] = {m_name, msg};
    ReportEvent(hSource, type, 0, 0, nullptr, 2, 0, msgData, nullptr);
    DeregisterEventSource(hSource);
  }
}

// static
void WINAPI ServiceBase::SvcMain(DWORD argc, TCHAR* argv[]) {
  assert(m_service);

  m_service->m_svcStatusHandle = ::RegisterServiceCtrlHandlerEx(m_service->GetName(),
                                                                ServiceCtrlHandler, NULL);
  if (!m_service->m_svcStatusHandle) {
    m_service->WriteToEventLog(_T("Can't set service control handler"),
                               EVENTLOG_ERROR_TYPE);
    return;
  }

  m_service->Start(argc, argv);
}

// static
DWORD WINAPI ServiceBase::ServiceCtrlHandler(DWORD ctrlCode, DWORD evtType,
                                             void* evtData, void* /*context*/) {
  switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
      m_service->Stop();
    break;

    case SERVICE_CONTROL_PAUSE:
      m_service->Pause();
    break;

    case SERVICE_CONTROL_CONTINUE:
      m_service->Continue();
    break;

    case SERVICE_CONTROL_SHUTDOWN:
      m_service->Shutdown();
    break;

    case SERVICE_CONTROL_SESSIONCHANGE:
      m_service->OnSessionChange(evtType, reinterpret_cast<WTSSESSION_NOTIFICATION*>(evtData));
    break;

    default:
    break;
  }

  return 0;
}

bool ServiceBase::RunInternal(ServiceBase* svc) {
  m_service = svc;

  TCHAR* svcName = const_cast<CString&>(m_service->GetName()).GetBuffer();

  SERVICE_TABLE_ENTRY tableEntry[] = {
    {svcName, SvcMain},
    {nullptr, nullptr}
  };

  return ::StartServiceCtrlDispatcher(tableEntry) == TRUE;
}

void ServiceBase::Start(DWORD argc, TCHAR* argv[]) {
  SetStatus(SERVICE_START_PENDING);
  OnStart(argc, argv);
  SetStatus(SERVICE_RUNNING);
}

void ServiceBase::Stop() {
  SetStatus(SERVICE_STOP_PENDING);
  OnStop();
  SetStatus(SERVICE_STOPPED);
}

void ServiceBase::Pause() {
  SetStatus(SERVICE_PAUSE_PENDING);
  OnPause();
  SetStatus(SERVICE_PAUSED);
}

void ServiceBase::Continue() {
  SetStatus(SERVICE_CONTINUE_PENDING);
  OnContinue();
  SetStatus(SERVICE_RUNNING);
}

void ServiceBase::Shutdown() {
  OnShutdown();
  SetStatus(SERVICE_STOPPED);
}