#include "periodic_output.h"
#include <iostream>
#include <iomanip>

using namespace token_client_server;

PeriodicOutput::PeriodicOutput(boost::asio::io_context& ioc, const std::chrono::seconds& duration, 
    TokenHandlersManagerPtr handlers_manager)
  : timer_{make_strand(ioc)}, duration_{duration}, handlers_manager_{handlers_manager} {
    start_();
}

void PeriodicOutput::start_() {
    timer_.expires_after(duration_);
    timer_.async_wait([this](const boost::system::error_code& error) {
        output_();
        start_();
    });
}

void PeriodicOutput::output_() {
    static const std::string delimiter(60, '=');

    if(!handlers_manager_->is_modified()) {
        return;
    }

    std::cout << delimiter << '\n';

    size_t handlers_count = 0;
    handlers_manager_->for_each_handler([&handlers_count](auto& token, auto& handler) {
        ++handlers_count;
        std::cout << token << '\n';
        handler.for_each_client([](auto& descr, auto& data) {
            std::cout << "    client(" << descr.get_id() << ") addr(" << descr.get_addr() << ':' << descr.get_port()
                << ") total_expected(" << data->total_expected << ") total_recived(" << data->total_received << ")\n";
        });
    });
    handlers_manager_->set_modified(false);

    std::time_t result = std::time(nullptr);
    std::cout << delimiter << "\nTotal " << handlers_count << " handlers at " << std::asctime(std::localtime(&result)) << std::endl;
}
