#include <cassert>
#include <openiot/drivers.hpp>

using namespace openiot::drivers;
using openiot::foundation::ErrorCode;

int main() {
  Dht22 dht;
  auto dht_before = dht.read();
  assert(dht_before.error() == ErrorCode::NotInitialized);
  assert(!dht_before.ok());
  assert(dht.begin(4) == ErrorCode::Ok);
  auto dht_reading = dht.read();
  assert(dht_reading.ok());
  assert(dht_reading.value().temperature_c == 0.0F);
  assert(dht_reading.value().humidity_pct == 0.0F);

  Ds18b20 ds;
  auto ds_before = ds.readCelsius();
  assert(ds_before.error() == ErrorCode::NotInitialized);
  assert(!ds_before.ok());
  assert(ds.begin(5) == ErrorCode::Ok);
  auto ds_value = ds.readCelsius();
  assert(ds_value.ok());
  assert(ds_value.value() == 0.0F);

  SoilMoisture soil;
  auto soil_before = soil.readRaw();
  assert(soil_before.error() == ErrorCode::NotInitialized);
  assert(!soil_before.ok());
  assert(soil.begin(34) == ErrorCode::Ok);
  auto soil_value = soil.readRaw();
  assert(soil_value.ok());
  assert(soil_value.value() == 0U);

  Relay relay;
  assert(relay.set(true) == ErrorCode::NotInitialized);
  assert(relay.begin(26) == ErrorCode::Ok);
  assert(!relay.state());
  assert(relay.set(true) == ErrorCode::Ok);
  assert(relay.state());
  assert(relay.set(false) == ErrorCode::Ok);
  assert(!relay.state());

  return 0;
}
