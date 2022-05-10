#pragma once
#include <chrono>
#include <boost/asio.hpp>
#include "token_handlers_manager.hpp"

namespace token_client_server {

    class PeriodicOutput
    {
        boost::asio::system_timer timer_;
        const std::chrono::seconds duration_;
        TokenHandlersManagerPtr handlers_manager_;

    public:
        PeriodicOutput(boost::asio::io_context& ioc, const std::chrono::seconds& duration,
            TokenHandlersManagerPtr handlers_manager);

    private:
        void start_();
        void output_();
    };

} // namespace token_client_server
