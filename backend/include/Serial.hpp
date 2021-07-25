// Comment this out if USB serial is not used.
#define USE_USB_SERIAL

#ifdef USE_USB_SERIAL
    #include <WebUSB.h>
    #define Serial_t WebUSB
#else
    // In Arduino code Serial is instance of Serial_ class.
    #define Serial_t Serial_
#endif
