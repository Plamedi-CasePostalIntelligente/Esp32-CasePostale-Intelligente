#ifndef MY_SOLENOIDE
#define MY_SOLENOIDE
#include <Arduino.h>

class MySolenoide
{
private:
int relayPinSolenoide ;//= 26 ;  // GPIO26, à adapter selon ton branchement;
//#define RELAY_PIN_SOLENOIDE1 = 26;  // GPIO26, à adapter selon ton branchement
//#define RELAY_PIN_SOLENOIDE2 = 33;  // GPIO33, à adapter selon ton branchement

public:
    MySolenoide(int relayPinSolenoide);
    ~MySolenoide();

    bool init();
    void openCase();
    void closeCase();
    void close();
};

#endif