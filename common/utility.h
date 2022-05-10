#pragma once
#include <boost/system/error_code.hpp>

void fail(char const* what, const boost::system::error_code& error);
