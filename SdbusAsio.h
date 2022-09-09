#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <sdbus-c++/sdbus-c++.h>

class SdbusAsio final : boost::noncopyable {
public:
    explicit SdbusAsio(boost::asio::io_context& io_context,
        std::unique_ptr<sdbus::IConnection> conn = sdbus::createDefaultBusConnection())
        : conn_ { std::move(conn) }
        , timer_ { io_context }
        , dbus_desc_ { io_context }

    {
        auto poll_data = conn_->getEventLoopPollData();
        dbus_desc_.async_wait(boost::asio::posix::stream_descriptor::wait_read,
            [this](const boost::system::error_code&) { processRead(); });
        dbus_desc_.assign(poll_data.fd);

        if (poll_data.timeout_usec != UINT64_MAX) {
            timer_.async_wait([this](boost::system::error_code const&) { processTimeout(); });
            timer_.expires_from_now(boost::posix_time::microsec(poll_data.timeout_usec));
        }
    }

    std::shared_ptr<sdbus::IConnection> getConnection() { return conn_; }

private:
    void process()
    {
        for (auto i = 0; i < DBUS_PROCESS_MAX; i++) {
            if (!conn_->processPendingRequest()) {
                break;
            }
        }
    }
    void processRead()
    {
        process();
        dbus_desc_.async_wait(boost::asio::posix::stream_descriptor::wait_read,
            [this](const boost::system::error_code&) { processRead(); });
    }

    void processTimeout()
    {
        process();
        timer_.async_wait([this](boost::system::error_code const&) { processTimeout(); });
    }

    static constexpr auto DBUS_PROCESS_MAX = 32;
    std::shared_ptr<sdbus::IConnection> conn_;
    boost::asio::deadline_timer timer_;
    boost::asio::posix::stream_descriptor dbus_desc_;
};
