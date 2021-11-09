#pragma once
#ifndef BASIC_HPP__
#define BASIC_HPP__

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/config.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace basic
{

// Report a failure
void fail(beast::error_code ec, char const* what)
{
    BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message() << "\n";
}


void init_log()
{
    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Scope",
                                                  boost::log::attributes::named_scope());
#ifdef NDEBUG
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info
    );
#else
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::trace
    );
#endif // NDEBUG

    /* log formatter: https://gist.github.com/xiongjia/e23b9572d3fc3d677e3d
     * [TimeStamp] [Severity Level] [Scope] Log message
     */
    auto timestamp = boost::log::expressions::
        format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
    auto severity = boost::log::expressions::
        attr<boost::log::trivial::severity_level>("Severity");
    auto scope = boost::log::expressions::format_named_scope("Scope",
        boost::log::keywords::format = "%n(%f:%l)",
        boost::log::keywords::iteration = boost::log::expressions::reverse,
        boost::log::keywords::depth = 2);
    boost::log::formatter final_format =
        boost::log::expressions::format("[%1%] (%2%): %3%")
        % timestamp % severity
        % boost::log::expressions::smessage;

    /* console sink */
    auto console = boost::log::add_console_log(std::clog);
    console->set_formatter(final_format);
}

}// namespace basic

#endif // BASIC_HPP__