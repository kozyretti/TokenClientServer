#pragma once
#include <boost/asio.hpp>
#include "token_handlers_manager.hpp"
#include "session_base.h"

namespace token_client_server {

    class ServerSession : public SessionBase
    {
        asio::coroutine coro_;
        TokenHandlersManagerPtr handlers_manager_;
        ClientDescr client_descr_;
        ClientDataPtr client_data_;

    public:
        static std::shared_ptr<ServerSession> create(tcp::socket&& socket, TokenHandlersManagerPtr handlers_manager);

        void start();

    private:
        ServerSession(tcp::socket&& socket, TokenHandlersManagerPtr handlers_manager);

        void process_(const std::string& op, const pt::ptree& payload) override;
        void send_ready_();
        void send_accepted_(const std::string& token);
    };

} // namespace token_client_server
