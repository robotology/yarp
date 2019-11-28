network_isConnected {#devel}
---------------

### os

#### `Network`

* Cleanup of the signature of the following methods of `yarp::os::Network`:
```
    static bool connect(const std::string& src, const std::string& dest, const ContactStyle& style);
    static bool connect(const std::string& src, const std::string& dest, const std::string& carrier = "",  bool quiet = true);
    static bool connect(const char* src, const char* dest, const char* carrier,  bool quiet = true);

    static bool disconnect(const std::string& src, const std::string& dest, bool quiet);
    static bool disconnect(const std::string& src, const std::string& dest, const ContactStyle& style);
    static bool disconnect(const std::string& src, const std::string& dest, const std::string& carrier = "", bool quiet = true);
    static bool disconnect(const char* src, const char* dest, const char* carrier, bool quiet = true);
 
    static bool isConnected(const std::string& src, const std::string& dest, bool quiet);
    static bool isConnected(const std::string& src, const std::string& dest, const ContactStyle& style);
    static bool isConnected(const std::string& src, const std::string& dest, const std::string& carrier = "", bool quiet = true);
    static bool isConnected(const char* src, const char* dest, const char* carrier,  bool quiet = true);
```
