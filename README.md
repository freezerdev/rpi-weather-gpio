# rpi-weather-gpio
Raspberry Pi GPIO weather station

Copyright Brian Friesen 2016

Source for a simple weather station on a Raspberry Pi with sensors directly
wired to the GPIO pins.  Written mostly in C++ with a little bit of C.

This code is for a weather station with the following sensors.  The sensors are
queried once per minute.

1.  BME280 temperature, humidity, and pressure sensor.  This code requires the
    sensor to be wired to the Pi using the I2C bus.
2.  A Sparkfun weather meter kit (or other similar kit).  The rain tip bucket,
    anemometer, and wind vane are wired directly to the Pi's GPIO pins.


The output from this code is threefold.

1.  The sensor data is written to an SQLite database file.  This allows the
    data to be searched later on.
2.  The sensor data is written to an HTML file in the local web root folder.
    If the Pi has a web server installed (e.g. Lighttpd) then one can access
    the weather data using a web browser.
3.  The sensor data is submitted online to WeatherUnderground.  This requires
    a Personal Weather Station (PWS) account with WeatherUnderground which is
    free.


In order to compile you must download the following SDKs:

WiringPi:	http://wiringpi.com/
SQLite:		https://www.sqlite.org/
