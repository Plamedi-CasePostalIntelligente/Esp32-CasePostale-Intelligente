#ifndef MYOLED_H
#define MYOLED_H

#include <string>

// Logo Ecole Du Web GROS
const unsigned char Cegep16x16[] = {
    0x00, 0x00, 0x03, 0xF0, 0x07, 0xE0, 0x0F, 0xC0, 0x0F, 0x80, 0x0F, 0x18, 0x0E, 0x3C, 0x0C, 0x7E,
    0x08, 0x3E, 0x01, 0x1E, 0x03, 0x8E, 0x07, 0xC6, 0x0F, 0xC2, 0x1F, 0x80, 0x3F, 0x00, 0x7E, 0x00
};

// Bitmaps pour l'animation PushMan (placeholders, à remplir avec vos données)
const unsigned char PushMan24x48_1[] = { /* ... */ };
const unsigned char PushMan24x48_2[] = { /* ... */ };
const unsigned char PushMan24x48_3[] = { /* ... */ };
const unsigned char PushMan24x48_4[] = { /* ... */ };
const unsigned char PushMan24x48_5[] = { /* ... */ };
const unsigned char PushMan24x48_6[] = { /* ... */ };
const unsigned char PushMan24x48_7[] = { /* ... */ };

// Définir les broches SDA et SCL personnalisées
#define MY_OLED_SDA_PIN 13  // GPIO 13 pour SDA
#define MY_OLED_SCL_PIN 14  // GPIO 14 pour SCL

#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

class MyOled : public Adafruit_SSD1306 {
public:
    MyOled(TwoWire *twi, uint8_t RST = 4, uint8_t rawHeight = 64, uint8_t rawWidth = 128) 
        : Adafruit_SSD1306(rawWidth, rawHeight, twi, RST) {};
    
    int init(int displaySplashTime = 0, uint8_t addrI2C = 0x3C);
    void veilleOn(bool active);
    void printSpecialChar(const char *spacialCaractere, int makeDelaySecondes = 0);
    void printIt(const char *toDisplay, bool displayAfter = true, int makeDelaySecondes = 0);
    void printIt(int posx, int posy, const char *toDisplay, bool displayAfter = true, int makeDelaySecondes = 0);
    bool veilleCheck();
    void veilleDelay(int nbreSecondes);
    bool isOperational(); // Nouvelle méthode pour vérifier l'état sans réinitialiser

    // Méthodes pour la gestion des menus avec 2 boutons
    void displayMainMenu();
    void displaySubMenu(int subMenuIndex, float temperature, const char *etatCasier1, const char *etatCasier2, const char *etatWifi);
    void moveLeftButton();
    void moveRightButton(float temp, const char *etatCasier1, const char *etatCasier2, const char *etatWifi);
    int getCurrentMenuIndex();
    int getCurrentSubMenu();

private:
    unsigned char *PushMan24x48Pointers[7];
    void displaySplash(int makeDelaySecondes);
    static void vATaskSecondCount(void *pvParameters);
    static int veilleTimeLapse;
    int veilleNbreSecondes = -1;
    bool veilleActif = false;
    String lastError;

    // Variables pour la gestion des menus
    int currentMenuIndex = 0;
    int currentSubMenu = -1;
    static const char *mainMenu[4]; // Déclaration statique
};

#endif