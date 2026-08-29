#include <openiot/core.hpp>

using namespace openiot;

static core::Logger logger;
static core::EventBus event_bus;
static core::Scheduler scheduler;
static core::Config config;
static core::DeviceManager device;
static core::BootManager boot(logger, event_bus, scheduler, config, device);

void setup() {
    (void)boot.begin();
}

void loop() {
    boot.loop();
}

#ifndef ARDUINO
int main() {
    setup();
    loop();
    return 0;
}
#endif
