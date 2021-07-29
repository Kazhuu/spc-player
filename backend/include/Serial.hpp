#ifdef USE_WEBUSB_SERIAL
    #include <WebUSB.h>
    #define Serial_t WebUSB
#else
    // In Arduino code Serial is instance of Serial_ class.
    #define Serial_t Serial_
#endif
