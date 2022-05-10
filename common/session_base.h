#pragma once
#include <memory>
#include <array>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

namespace token_client_server
{
    namespace asio = boost::asio;
    using boost::asio::ip::tcp;
    using boost::system::error_code;
    namespace pt = boost::property_tree;

    class SessionBase : public std::enable_shared_from_this<SessionBase>
    {
        tcp::socket socket_;       
        std::array<char, 1024> data_;

    public:
        virtual ~SessionBase() = default;

    protected:
        SessionBase(tcp::socket&& socket);
        void send_op_(const std::string& op, const pt::ptree& payload);
        void receive_();

        tcp::socket& get_socket_() {
            return socket_;
        }

    private:
        virtual void process_(const std::string& op, const pt::ptree& payload) =0;

        void on_receive_(const error_code& error, std::size_t length);
    };

} // namespace token_client_server
