# IOT\_BRC API #

The API for a car unit operating in a pre-build scene on class iot\_brc (IoT Based Reality Combat).

## Libraries ##

The target platform of libraries is Arduino.

- BRCClient: The API for communicating with the BRC server. Inherit from class KSM111\_ESP8266.
- KSM111\_ESP8266: The API for directly communicating with module KSM111\_ESP8266.
- RFID: The API for reading serial number of RFIG tag via module MF-RC522.

## Usage ##

1. Download the directory of the corressponding library
2. Put the directory in the directory of custom libraries of Arduino IDE.
3. Restart the Arduino IDE if it's opened.
4. Import library: Sketch -> Include Library -> Contributed libraries
5. Open examples: File -> Examples -> Examples from Custom Libraries
