#include <string>
#include <thread>
#include <future>
#include <chrono>

#include <ipc.h>

#include "host.h"
#include "ipc.h"

void service::Host::sendToggle()
{
    ipc::channel client{this->pipename, ipc::receiver};

    while (client.recv_count() == 0)
    {
        std::this_thread::yield();
    }

    client.send("hdr");
}

std::thread service::Host::start()
{
    static ipc::channel server{pipename, ipc::receiver};
    return std::thread([this] {
        while (1)
        {
            auto dd = server.recv(1000);
            if (dd.empty())
            {
                continue;
            }

            auto msg = static_cast<char *>(dd.data());
            if (msg == nullptr || msg[0] == '\0')
                return;

            std::string str(msg);

            if (str.find("hdr") != std::string::npos)
            {
                auto enabled = str.find("on") != std::string::npos;
                this->toggleFn();
            }
        }
    });
}

service::Host::Host(std::function<void()> toggleFn)
{
    this->toggleFn = toggleFn;
}