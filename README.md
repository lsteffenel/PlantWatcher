# PlantWatcher

Required material:
* ESP8266 (Wemos D1 mini)
* soil moisture sensor
* water pump
* source power (AC adapter, battery, solar panel, etc.)
* water recipient

There are several versions of the code, each one with different features:
* PlantWatcher-simple.ino - basic version with just sensing/pumping
* PlantWatcher-wifi.ino - "connected" version that sends data to ThingSpeak
* PlantWatcher-NTP.ino - Thingspeak + NTP so that watering avoids "hot" hours of the day (do not use solar panels in this case)

All versions use ESP8266 DeepSleep to minimize power consumption (4xAA batteries should be enough for a couple of months). Do not use USB powerbanks as most of them shutdown at low consumption and then your circuit will be offline forever.
