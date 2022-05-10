#include "utility.h"
#include <iostream>

void fail(char const* what, const boost::system::error_code& error) {
    std::cerr << what << ": " << error.message() << std::endl;
}
