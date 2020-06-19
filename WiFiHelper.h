#ifndef _WIFIHELPER_h
#define _WIFIHELPER_h



int wifiConnect(int maxAttempts = 50);
void WiFiStart(bool EnterpriseMode = false);

String wifiMacAddress();

#endif