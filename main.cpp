#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>

using namespace boost;

void async_print
(
    const system::error_code&,
    asio::steady_timer* timer,
    int* count
)
{
    if(*count < 10)
    {
        std::cout << "Async Elapsed : " << *count+1 << ".\n";
        ++(*count);

        timer->expires_at(timer->expiry() + asio::chrono::seconds{1});

        timer->async_wait
            (bind(async_print, asio::placeholders::error, timer, count));
    }
}

int main()
{
    asio::io_context sapIoContext{};

    asio::ip::udp::endpoint sapEndpoint{ asio::ip::udp::v4(), 9875 };
    asio::ip::udp::socket   socket{ sapIoContext, sapEndpoint };

    asio::steady_timer      stimer{ sapIoContext, asio::chrono::seconds{ 5 } };

    int count{};
    asio::steady_timer      atimer{ sapIoContext, asio::chrono::seconds{ 1 } };

    atimer.async_wait
        (bind(async_print, asio::placeholders::error, &atimer, &count));

    stimer.wait();
    std::cout << "Sync Elapsed.\n";

    sapIoContext.run();

    return EXIT_SUCCESS;
}