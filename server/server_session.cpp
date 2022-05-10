#include "server_session.h"
#include <iostream>

using namespace token_client_server;

std::shared_ptr<ServerSession> ServerSession::create(tcp::socket&& socket, TokenHandlersManagerPtr handlers_manager) {    
    return std::shared_ptr<ServerSession>(new ServerSession{std::move(socket), std::move(handlers_manager)});
}

ServerSession::ServerSession(tcp::socket&& socket, TokenHandlersManagerPtr handlers_manager)
  : SessionBase{std::move(socket)}, handlers_manager_{std::move(handlers_manager)} { }

void ServerSession::start() {
    process_(std::string(), pt::ptree());
}

#include <boost/asio/yield.hpp>

void ServerSession::process_(const std::string& op, const pt::ptree& payload) {
    reenter (coro_)
    {
        yield receive_();
        if(op != "Greeting") {
            std::cerr << "Wrong first request: " << op << std::endl;
            return;
        }

        client_descr_ = {
            payload.get<std::string>("client_id"),
            get_socket_().remote_endpoint().address().to_string(),
            get_socket_().remote_endpoint().port()
        };
        client_data_ = std::make_shared<ClientData>(payload.get<std::size_t>("tokens_expected"));

        yield send_ready_();

        while(true) {
            if(op != "Token") {
                std::cerr << "Wrong request: " << op << std::endl;
                return;
            }

            yield {
                auto token = payload.get<std::string>("token");
                auto& handler = handlers_manager_->get_or_create_handler(token);
                handler(client_descr_, client_data_);
                handlers_manager_->set_modified(true);

                send_accepted_(token);
            }
        }
    }
}

#include <boost/asio/unyield.hpp>

void ServerSession::send_ready_() {
    send_op_("Ready", pt::ptree());
}

void ServerSession::send_accepted_(const std::string& token) {
    pt::ptree payload;
    payload.put("token", token);
    send_op_("Accepted", payload);
}
