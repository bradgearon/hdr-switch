#ifndef HOST_H
#define HOST_H

#include <memory>
#include <functional>
#include <thread>

#include "toggle.h"

namespace service
{

    class Host
    {

    public:
        Host(std::function<void()> toggleFn);
        

        std::thread start();
        void sendToggle();

    private:
        const char *pipename = "hdr-switch-svc";
        std::function<void()> toggleFn;
    };

} // namespace service

#endif