#include "server.h"
#include "utility.h"
#include "server_session.h"

using namespace token_client_server;

Server::Server(boost::asio::io_context& ioc, unsigned short port, TokenHandlersManagerPtr handlers_manager)
  : acceptor_{ioc, tcp::endpoint(tcp::v4(), port)}, handlers_manager_{std::move(handlers_manager)} {
    do_accept_();
}

void Server::do_accept_() {
    acceptor_.async_accept(
        [this](const error_code& error, tcp::socket socket) {
            if (!error) {
                ServerSession::create(std::move(socket), handlers_manager_)->start();
            }
            else {
                fail("Accept", error);
            }
            do_accept_();
        });
}
