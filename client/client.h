#pragma once
#include <fstream>
#include "session_base.h"

namespace token_client_server
{
    class Client : public SessionBase
    {
        asio::coroutine coro_;
        tcp::resolver resolver_;

        std::string client_id_;
        std::ifstream tokens_stream_;
        std::size_t tokens_count_ = 0;
        bool tokens_all_ = false;

    public:
        static std::shared_ptr<Client> create(asio::io_context& ioc, const std::string& client_id);

        bool prepare_tokens(const std::string& filepath, std::size_t num);
        void start(const std::string& host, const std::string& port);

    private:
        Client(asio::io_context& ioc, const std::string& client_id);

        void process_(const std::string& op, const pt::ptree& payload) override;

        void send_greeting_();
        void send_token_(const std::string& token);
    };

} // namespace token_client_server
