#pragma once

// Copy this file to network_secrets.hpp on your local machine.
// Edit the values locally. NEVER commit network_secrets.hpp.
// Use a 2.4 GHz Wi-Fi network supported by the ESP32.
#include <openiot/isrgrootx1.hpp>

#define OPENIOT_WIFI_SSID "YOUR_WIFI_SSID"
#define OPENIOT_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define OPENIOT_MQTT_HOST "YOUR_MQTT_HOST"
#define OPENIOT_MQTT_PORT 8883
#define OPENIOT_MQTT_USERNAME "YOUR_MQTT_USERNAME"
#define OPENIOT_MQTT_PASSWORD "YOUR_MQTT_PASSWORD"

// CA trust anchor used by MqttConfig when TLS is enabled.
#define OPENIOT_MQTT_CA_CERTIFICATE_PEM (::openiot::network::kIsrgRootX1Pem)
