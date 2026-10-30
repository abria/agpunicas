#include "Singleton.h"

namespace
{
int destroyed = 0;
int derivedDestroyed = 0;

struct Prefix { virtual ~Prefix() = default; };

class Service : public Prefix, public agp::Singleton<Service>
{
    friend class agp::Singleton<Service>;
protected:
    Service() = default;
public:
    ~Service() override { ++destroyed; }
};

class DerivedService : public Service
{
public:
    ~DerivedService() override { ++derivedDestroyed; }
};

void require(bool condition)
{
    if (!condition)
        throw "Singleton lifetime regression";
}
}

int main()
{
    auto* service = Service::instance();
    Service::setInstance(service);
    require(destroyed == 0 && Service::instance() == service);

    Service::setInstance(new DerivedService());
    require(destroyed == 1);
    Service::destroyInstance();
    Service::destroyInstance();
    require(destroyed == 2 && derivedDestroyed == 1);

    // Like a HUD owned and deleted by the scene stack, with multiple inheritance.
    delete Service::instance();
    Service::destroyInstance();
    require(destroyed == 3);
    Service::instance();
    Service::destroyInstance();
    require(destroyed == 4);
}
