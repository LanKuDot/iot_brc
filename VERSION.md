**v1.2.1**
- Feature
	- BRCClient: `beginBRCClient()` will not connect to the same connected AP.

**v1.2**
- Feature
	- KSM111\_ESP8266: `joinAP()` will return error code.
- Fix
	- KSM111\_ESP8266: `DEBUG_STR()` will be called even if there is no incoming message.

**v1.1**
- Features
	- KSM111\_ESP8266: Support `HardwareSerial`
	- BRCClient: Add `sendToClient()` and `broadcast()`
- Fix
	- BRCClient: Return false even if successfully connected to the BRC server
	
**v1.0**
- Initial version: RFID, KSM111\_ESP8266, and BRCClient libraries
