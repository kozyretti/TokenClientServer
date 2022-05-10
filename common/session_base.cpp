#include "session_base.h"
#include <boost/property_tree/json_parser.hpp>
#include "utility.h"

using namespace token_client_server;

SessionBase::SessionBase(tcp::socket&& socket)
  : socket_{std::move(socket)} { }

void SessionBase::receive_() {
    socket_.async_read_some(asio::buffer(data_),
        [self=shared_from_this()](const error_code& error, std::size_t length) { self->on_receive_(error, length); });
}

void SessionBase::on_receive_(const error_code& error, std::size_t length) {
    if (error) {
        return fail("Receive", error);
    }

    std::stringstream ss;
    ss.write(data_.data(), length);
    pt::ptree msg;
    pt::read_json(ss, msg);
    process_(msg.get<std::string>("op"), msg.get_child("payload"));
}

void SessionBase::send_op_(const std::string& op, const pt::ptree& payload) {
    pt::ptree msg;
    msg.put("op", op);
    msg.add_child("payload", payload);

    std::stringstream ss;
    pt::write_json(ss, msg, false);
    asio::async_write(socket_, asio::buffer(ss.str()),
        [self=shared_from_this()](const error_code& error, std::size_t length) {
            if (error) {
                return fail("Send", error);
            }
            self->receive_();
        });
}
