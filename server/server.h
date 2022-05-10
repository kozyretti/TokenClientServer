#pragma once
#include <boost/asio.hpp>
#include "periodic_output.h"
#include "token_handlers_manager.hpp"

namespace token_client_server {
    
    class Server
    {
        boost::asio::ip::tcp::acceptor acceptor_;
        TokenHandlersManagerPtr handlers_manager_;

    public:
        Server(boost::asio::io_context& ioc, unsigned short port, TokenHandlersManagerPtr handlers_manager);

    private:
        void do_accept_();
    };

} // namespace token_client_server
