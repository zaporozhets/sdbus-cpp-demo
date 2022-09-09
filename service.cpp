#include <thread>

// pre-generated header
#include "service-glue.h"

class Adaptor : public sdbus::AdaptorInterfaces<org::yo::Example_adaptor, sdbus::Properties_adaptor> {
public:
    Adaptor(sdbus::IConnection& connection, std::string path)
        : AdaptorInterfaces(connection, std::move(path))
    {
        registerAdaptor();
    }

    ~Adaptor() { unregisterAdaptor(); }

private:
    void UpdateA(const std::string& value)
    {
        m_a = value;
        emitPropertiesChangedSignal(org::yo::Example_adaptor::INTERFACE_NAME, { "A" });

        m_b = value + value;
        emitPropertiesChangedSignal(org::yo::Example_adaptor::INTERFACE_NAME, { "B" });
    };
    std::string A() { return m_a; };
    std::string B() { return m_b; };
    std::string C() { return m_c; };

    std::string m_a;
    std::string m_b;
    std::string m_c;
};

int main()
{
    auto connection = sdbus::createSessionBusConnection();
    connection->requestName("org.yo");
    connection->enterEventLoopAsync();

    Adaptor client(*connection, "/org/yo/example");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    connection->releaseName("org.yo");
    connection->leaveEventLoop();

    return 0;
}
