#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

#include "service/gpu_settings_svc.h"
#include "service/install.h"

#include "ipc.h"
#include "nvapi.h"
#include "NvApiDriverSettings.h"
#include <stdlib.h>
#include <stdio.h>

using spsc_msg_queue = ipc::chan<ipc::wr<ipc::relat::single, ipc::relat::single, ipc::trans::unicast>>;

void err(NvAPI_Status status)
{
    NvAPI_ShortString szDesc = {0};
    NvAPI_GetErrorMessage(status, szDesc);
    printf(" NVAPI error: %s\n", szDesc);
    // exit(-1);
}

std::unique_ptr<GpuSettingsService> svcPtr;

void runandwait()
{
    auto t = std::async([]() {
        svcPtr->OnStart(NULL, NULL);
        while (1)
            std::this_thread::sleep_for(std::chrono::seconds(5));
    });
    t.wait();
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        return service(false, "");
    }

    std::vector<std::string> args(argv, argv + argc);

    auto isInstall = args[1].find("inst");
    if (isInstall != std::string::npos)
    {
        service(true, args[1]);
        return 0;
    }

    auto isTest = args[1].find("test");
    if (isTest != std::string::npos)
    {
        svcPtr.reset(new GpuSettingsService());
        runandwait();
    }

    const char *pipename = "gpu-settings-svc";
    if (args[1].find("client") != std::string::npos)
    {

        ipc::route cc{pipename};

        // waiting for connection
        while (cc.recv_count() == 0)
        {
            std::this_thread::yield();
        }

        cc.send("set");

        // bool cxnd = false;
        // while (!cxnd)
        // {
        //     cc.send("set");
        //     cxnd = cc.wait_for_recv(1, 1000);
        // }

        return 0;
    }

    return 0;
}