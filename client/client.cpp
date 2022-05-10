#include "client.h"
#include <iostream>
#include "utility.h"

using namespace token_client_server;

std::shared_ptr<Client> Client::create(asio::io_context& ioc, const std::string& client_id) {    
    return std::shared_ptr<Client>(new Client{ioc, client_id});
}

Client::Client(asio::io_context& ioc, const std::string& client_id)
  : SessionBase{tcp::socket{ioc}}, resolver_{ioc}, client_id_{client_id} { }

bool Client::prepare_tokens(const std::string& filepath, std::size_t num) {
    tokens_all_ = (num == 0);
    tokens_count_ = num;

    tokens_stream_.open(filepath);
    if(!tokens_stream_.is_open()) {
        std::cout << "Cannot open file " << filepath << std::endl; 
        return false;
    }
    return true;
}

void Client::start(const std::string& host, const std::string& port) {
    resolver_.async_resolve(host, port,
        [this](error_code error, tcp::resolver::results_type endpoints) {
            if (error) {
                return fail("Resolve", error);
            }

            asio::async_connect(get_socket_(), endpoints,
                [this](const error_code& error, const tcp::endpoint& /*endpoint*/) {
                    if (error) {
                        return fail("Connect", error);
                    }

                    process_(std::string(), pt::ptree());
                });
        });
}

#include <boost/asio/yield.hpp>

void Client::process_(const std::string& op, const pt::ptree& payload) {
    reenter (coro_)
    {
        yield send_greeting_();
        if(op != "Ready") {
            std::cout << "Wrong answer: " << op << std::endl;
            return;
        }

        while((tokens_all_ || tokens_count_) && tokens_stream_) {
            yield {
                std::string token;
                tokens_stream_ >> token;
                if(token.empty()) {
                    continue;
                }
                
                std::cout << client_id_ << " sends token: " << token << std::endl;

                --tokens_count_;
                send_token_(token);
            }
            
            if(op != "Accepted") {
                std::cout << "Wrong answer: " << op << std::endl;
                return;
            }
        }
    }
}

#include <boost/asio/unyield.hpp>

void Client::send_greeting_() {
    pt::ptree payload;
    payload.put("client_id", client_id_);
    payload.put<std::size_t>("tokens_expected", tokens_count_);
    send_op_("Greeting", payload);
}

void Client::send_token_(const std::string& token) {
    pt::ptree payload;
    payload.put("token", token);
    send_op_("Token", payload);
}
