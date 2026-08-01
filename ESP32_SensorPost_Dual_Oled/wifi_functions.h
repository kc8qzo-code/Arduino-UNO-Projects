#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

// Attempts to connect to the configured Wi-Fi network.
// Returns false after the timeout so the application can continue running.
bool connectToWiFi();

#endif
