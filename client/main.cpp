#include <iostream>
#include <boost/program_options.hpp>
#include "client.h"

namespace po = boost::program_options;
using namespace token_client_server;

int main(int argc, char** argv) {
    try {
        po::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show help")
            ("id,id", po::value<std::string>(), "client ID")
            ("file,f", po::value<std::string>(), "tokens file path")
            ("num,n", po::value<std::size_t>(), "number of tokens to send [all]")
            ("addr,a", po::value<std::string>(), "server address [localhost]")
            ("port,p", po::value<unsigned short>(), "server port [9000]");

        po::positional_options_description pos;
        pos.add("id", 1).add("file", 2);

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);   

        po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: client [options] <client_id> <filepath>\n" << desc << std::endl;
            return EXIT_FAILURE;
        }

        std::string client_id = vm.count("id") ? vm["id"].as<std::string>() : "";
        if(client_id.empty()) {
            std::cout << "Client id not set." << std::endl;
            return EXIT_FAILURE;
        }

        std::string filepath = vm.count("file") ? vm["file"].as<std::string>() : "";
        if(filepath.empty()) {
            std::cout << "Token file not set." << std::endl;
            return EXIT_FAILURE;
        }

        std::size_t num = vm.count("num") ? vm["num"].as<std::size_t>() : 0;
        std::string addr = vm.count("addr") ? vm["addr"].as<std::string>() : "localhost";
        unsigned short port = vm.count("port") ? vm["port"].as<unsigned short>() : 9000;

        boost::asio::io_context ioc;

        auto client = Client::create(ioc, client_id);
        if( !client->prepare_tokens(filepath, num) ) {
            return EXIT_FAILURE;
        }

        client->start(addr, std::to_string(port));
        ioc.run();
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
