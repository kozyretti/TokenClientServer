#pragma once
#include <string>
#include <atomic>
#include <tuple>
#include <memory>
#include "thread_safe_map.hpp"
#include "delay_simulation.h"

namespace token_client_server {

    class ClientDescr : public std::tuple<std::string, std::string, unsigned short>
    {
    public:
        using std::tuple<std::string, std::string, unsigned short>::tuple;

        const std::string& get_id() const {
            return std::get<0>(*this);
        }

        const std::string& get_addr() const {
            return std::get<1>(*this);
        }

        unsigned short get_port() const {
            return std::get<2>(*this);
        }
    };

    struct ClientData
    {
        ClientData(std::size_t expected)
          : total_expected{expected} { }

        const std::size_t total_expected;
        std::atomic_size_t total_received = 0;
    };

    using ClientDataPtr = std::shared_ptr<ClientData>;

    class TokenHandler
    {
        ThreadSafeMap<ClientDescr, ClientDataPtr> clients_;

    public:
        void operator()(const ClientDescr& descr, ClientDataPtr data) {
            auto [it, created] = clients_.get_or_create(descr, std::move(data));
            g_delay_simulation.delay();
            ++it->second->total_received;
        }

        template<class Func>
        void for_each_client(Func func) const {
            return clients_.for_each(func);
        }
    };

    class TokenHandlersManager
    {
        std::atomic_bool modified_ = false;
        ThreadSafeMap<std::string, TokenHandler> handlers_;

    public:
        TokenHandler& get_or_create_handler(const std::string& token) {
            auto [it, created] = handlers_.get_or_create(token);
            return it->second;
        }

        template<class Func>
        void for_each_handler(Func func) const {
            return handlers_.for_each(func);
        }

        void set_modified(bool val) {
            modified_ = val;
        }

        bool is_modified() {
            return modified_;
        }
    };

    using TokenHandlersManagerPtr = std::shared_ptr<TokenHandlersManager>;

} // namespace token_client_server
