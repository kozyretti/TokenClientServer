#include <iostream>
#include <boost/program_options.hpp>
#include "server.h"
#include "token_handlers_manager.hpp"

namespace po = boost::program_options;
using namespace token_client_server;

int main(int argc, char** argv) {
    try {
        unsigned int threads_count = std::thread::hardware_concurrency() * 2;
        auto threads_hint = "running threads 1..[" + std::to_string(threads_count) + "]";

        po::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show help")
            ("threads,t", po::value<unsigned int>(), threads_hint.c_str())
            ("sec,s", po::value<unsigned int>(), "output period in seconds [10]")
            ("addr,a", po::value<std::string>(), "server address [localhost]")
            ("port,p", po::value<unsigned short>(), "server port [9000]")
            ("delay,d", po::value<unsigned int>(), "handler delay simulation in msec +/-10% [0]");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);   

        if (vm.count("help")) {
            std::cout << "Usage: server [options]\n" << desc << std::endl;
            return EXIT_FAILURE;
        }

        if(vm.count("threads")) {
            auto val = vm["threads"].as<unsigned int>();
            if(val == 0 || val > threads_count) {
                std::cout << "Wrong threads number. Must be form 1 to " << threads_count * 2 << std::endl;
                return EXIT_FAILURE;
            }
            threads_count = val;
        }

        threads_count = vm.count("threads") ? vm["threads"].as<unsigned int>() : threads_count;
        unsigned int sec = vm.count("sec") ? vm["sec"].as<unsigned int>() : 10;
        std::string addr = vm.count("addr") ? vm["addr"].as<std::string>() : "localhost";
        unsigned short port = vm.count("port") ? vm["port"].as<unsigned short>() : 9000;
        unsigned int delay = vm.count("delay") ? vm["delay"].as<unsigned int>() : 0;

        g_delay_simulation.init(delay);

        boost::asio::io_context ioc;

        auto handlers_manager = std::make_shared<TokenHandlersManager>();

        Server server{ioc, port, handlers_manager};
        PeriodicOutput periodic_output_{ioc, std::chrono::seconds{sec}, handlers_manager};

        std::vector<std::thread> threads;
        threads.reserve(threads_count);

        for(int i = 0; i < threads_count; ++i) {
            threads.emplace_back([&ioc] {
                ioc.run();
            });
        }

        std::cout << "Server started at localhost:" << port << std::endl;

        for(auto& thread : threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
