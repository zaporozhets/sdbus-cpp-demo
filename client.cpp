#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <iostream>

#include "SdbusAsio.h"
// pre-generated header
#include "client-glue.h"
class Proxy final : public sdbus::ProxyInterfaces<org::yo::Example_proxy, sdbus::Properties_proxy> {
public:
    Proxy(sdbus::IConnection& connection, std::string destination, std::string path)
        : ProxyInterfaces(connection, std::move(destination), std::move(path))
    {
        registerProxy();
    }

    ~Proxy() { unregisterProxy(); }

private:
    void onPropertiesChanged(const std::string& interfaceName,
        const std::map<std::string, sdbus::Variant>& changedProperties,
        const std::vector<std::string>& invalidatedProperties)
    {
        std::cout << "interfaceName: " << interfaceName << std::endl;
        for (auto& prop : changedProperties) {
            std::cout << "changedProperty: " << prop.first << std::endl;
        }
    };
};

void handler(boost::asio::deadline_timer& timer, Proxy& proxy)
{
    std::cout << "timer event" << std::endl;
    proxy.UpdateA("hello");

    timer.expires_from_now(boost::posix_time::millisec { 1000 });
    timer.async_wait([&](auto) { handler(timer, proxy); });
}

int main()
{
    boost::asio::io_context ioc;
    boost::asio::deadline_timer timer { ioc };

    SdbusAsio sdbus_asio { ioc };

    Proxy proxy { *sdbus_asio.getConnection(), "org.yo", "/org/yo/example" };

    timer.expires_from_now(boost::posix_time::millisec { 1000 });
    timer.async_wait([&](auto) { handler(timer, proxy); });

    ioc.run();

    return 0;
}
