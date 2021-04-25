# The ESP8266 Internet Connection Status

This program continually tries to connect to a website and when there is connection built, status success returned.

### ConnectWiFi

_`ConnectWiFi()`_ is a function which accepts the interval delay as the parameter in milliseconds(`ms`).
Return the Boolean value either connected as `true` or connection failed as `false`

```cpp
	// (function) ConnectWiFi :: int -> bool
    bool state = ConnectWiFi(500);
```

### ConnectInternet

_`ConnectInternet()`_ function which return the status of the whether is there any internet available or not.

```cpp
	// (function) ConnectInternet :: void-> bool
    bool state = ConnectInternet();
```

### ConnectNetwork

The function which includes both _ConnectWiFi_ and _ConnectInternet_ functions and returns boolean value stating the network connected or not.

```cpp
// (function) ConnectNetwork:: void-> bool
bool state =  ConnectNetwork();
```
