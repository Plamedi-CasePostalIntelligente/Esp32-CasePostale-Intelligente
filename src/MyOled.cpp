#include <Arduino.h>
#include "MyOled.h"

int MyOled::veilleTimeLapse = 0;
const char *MyOled::mainMenu[4] = {"Etat C1", "Etat C2", "Temperature", "Statut Systeme"};

// Génère des intervalles d'une seconde pour synchroniser le système
void MyOled::vATaskSecondCount(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        MyOled::veilleTimeLapse++;
        xLastWakeTime = xTaskGetTickCount();
    }
    vTaskDelete(NULL);
}

// Vérifie si l'OLED est opérationnel sans réinitialiser son état
bool MyOled::isOperational() {
    Wire.beginTransmission(0x3C);
    int error = Wire.endTransmission();
    return (error == 0);
}

// Initialisation (inchangée)
int MyOled::init(int displaySplashTime, uint8_t addrI2C) {
    Wire.begin(MY_OLED_SDA_PIN, MY_OLED_SCL_PIN);
    if (!begin(SSD1306_SWITCHCAPVCC, addrI2C)) {
        return 1;
    }
    PushMan24x48Pointers[0] = const_cast<unsigned char *>(&PushMan24x48_1[0]);
    PushMan24x48Pointers[1] = const_cast<unsigned char *>(&PushMan24x48_2[0]);
    PushMan24x48Pointers[2] = const_cast<unsigned char *>(&PushMan24x48_3[0]);
    PushMan24x48Pointers[3] = const_cast<unsigned char *>(&PushMan24x48_4[0]);
    PushMan24x48Pointers[4] = const_cast<unsigned char *>(&PushMan24x48_5[0]);
    PushMan24x48Pointers[5] = const_cast<unsigned char *>(&PushMan24x48_6[0]);
    PushMan24x48Pointers[6] = const_cast<unsigned char *>(&PushMan24x48_7[0]);
    cp437(true);
    clearDisplay();
    setTextColor(SSD1306_WHITE);
    setTextSize(1);
    setCursor(1, 1);
    display();
    if (displaySplashTime != 0)
        displaySplash(displaySplashTime);
    xTaskCreate(vATaskSecondCount, "vATask Loop", 10000, NULL, 1, NULL);
    displayMainMenu();
    return 0;
}

void MyOled::veilleDelay(int nbreSecondes) {
    veilleNbreSecondes = nbreSecondes;
    MyOled::veilleTimeLapse = 0;
    veilleActif = true;
}

bool MyOled::veilleCheck() {
    if (MyOled::veilleTimeLapse > veilleNbreSecondes) {
        clearDisplay();
        displayMainMenu();
        MyOled::veilleTimeLapse = 0;
        veilleActif = true;
    }
    return veilleActif;
}

// Affichage du menu principal avec encadré
void MyOled::displayMainMenu() {
    clearDisplay();
    setTextColor(SSD1306_WHITE);

    // Dessiner un cadre autour du menu (128x64 pixels, marges de 2 pixels)
    drawRect(2, 2, 124, 60, SSD1306_WHITE);

    // Afficher un titre "Menu" en haut
    setTextSize(1);
    setCursor(50, 6);
    print("Menu");

    // Afficher les options du menu
    setTextSize(1); // Police plus petite pour compacité
    for (int i = 0; i < 4; i++) {
        setCursor(12, 18 + i * 10); // Espacement réduit pour les 4 options
        if (i == currentMenuIndex) {
            print("> "); // Indicateur de sélection
        } else {
            print("  ");
        }
        print(mainMenu[i]);
    }
    display();
}

// Affichage des sous-menus avec encadré
void MyOled::displaySubMenu(int subMenuIndex, float temperature, const char *etatCasier1, const char *etatCasier2, const char *etatWifi) {
    clearDisplay();
    setTextColor(SSD1306_WHITE);

    // Dessiner un cadre autour du sous-menu
    drawRect(2, 2, 124, 60, SSD1306_WHITE);

    // Afficher le titre du sous-menu
    setTextSize(2); // Titre en plus grand
    setCursor(10, 6);
    switch (subMenuIndex) {
        case 0: print("Casier 1"); break;
        case 1: print("Casier 2"); break;
        case 2: print("Temperat."); break;
        case 3: print("Etat Wifi"); break;
    }

    // Afficher les détails
    setTextSize(1); // Détails en police standard
    switch (subMenuIndex) {
        case 0: // État C1
            printIt(10, 24, "Etat: ", false);
            printIt(40, 24, etatCasier1, true);
            break;
        case 1: // État C2
            printIt(10, 24, "Etat: ", false);
            printIt(40, 24, etatCasier2, true);
            break;
        case 2: // Température
            char tempStr[10];
            snprintf(tempStr, sizeof(tempStr), "%.1f C", temperature);
            printIt(10, 24, "Valeur: ", false);
            printIt(50, 24, tempStr, true);
            break;
        case 3: // Statut Système
            printIt(10, 24, "Etat: ", false);
            printIt(40, 24, etatWifi, true);
            break;
    }

    // Ajouter une indication "Retour" en bas
    setTextSize(1);
    setCursor(10, 50);
    print("[Droit] Retour");
    display();
}

// Bouton gauche : Défilement dans le menu principal
void MyOled::moveLeftButton() {
    if (currentSubMenu == -1) {
        currentMenuIndex++;
        if (currentMenuIndex >= 4) {
            currentMenuIndex = 0;
        }
        displayMainMenu();
    }
}

// Bouton droit : Entrée ou sortie du sous-menu
void MyOled::moveRightButton(float temp, const char *etatCasier1, const char *etatCasier2, const char *etatWifi) {
    if (currentSubMenu == -1) {
        currentSubMenu = currentMenuIndex;
        displaySubMenu(currentSubMenu, temp, etatCasier1, etatCasier2, etatWifi);
    } else {
        currentSubMenu = -1;
        displayMainMenu();
    }
}

// Getters
int MyOled::getCurrentMenuIndex() {
    return currentMenuIndex;
}

int MyOled::getCurrentSubMenu() {
    return currentSubMenu;
}

// Autres fonctions inchangées
void MyOled::printIt(int posx, int posy, const char *toDisplay, bool displayAfter, int makeDelaySecondes) {
    setCursor(posx, posy);
    print(toDisplay);
    if (displayAfter)
        display();
    delay(makeDelaySecondes);
}

void MyOled::printIt(const char *toDisplay, bool displayAfter, int makeDelaySecondes) {
    print(toDisplay);
    if (displayAfter)
        display();
    delay(makeDelaySecondes);
}

void MyOled::printSpecialChar(const char *spacialCaractere, int makeDelaySecondes) {
    if (spacialCaractere == "é")
        write(130);
    if (spacialCaractere == "o")
        write(248);
    delay(makeDelaySecondes);
}

void MyOled::displaySplash(int makeDelaySecondes) {
    int indexMan = 0;
    int lastFramePosition = 12;
    int posX = 0;
    int posY = 20;
    if (makeDelaySecondes == 0)
        return;
    clearDisplay();
    setTextSize(1);
    for (int positionX = -128; positionX <= lastFramePosition; positionX++) {
        drawLine(positionX + 128, 36, positionX + 128 - 12, 36, WHITE);
        drawBitmap(positionX + 128, 20, PushMan24x48Pointers[(++indexMan) % 7], 24, 48, WHITE);
        drawBitmap(0, 0, Cegep16x16, 16, 16, WHITE);
        printIt(22, 0, "CEGEP DE", false, 0);
        printIt(22, 9, "RIVIERE-DU-LOUP", false, 0);
        fillCircle(posX + 7 + positionX, posY + 2 + 3, 4, SSD1306_WHITE);
        fillRect(posX + positionX, posY + 10 + 3, 5, 20, SSD1306_WHITE);
        fillRect(posX + positionX + 10, posY + 10 + 3, 5, 20, SSD1306_WHITE);
        printIt(posX + positionX + 24, posY + 8, "TECHNIQUES  DE", false, 0);
        printIt(posX + positionX + 24, posY + 18, "L'INFORMATIQUE", false, 0);
        printIt(posX + positionX + 24, posY + 28, "-ECOLE DU WEB-", false, 0);
        display();
        delay(makeDelaySecondes / 128);
        clearDisplay();
    }   
    delay(makeDelaySecondes * 2);
}

void MyOled::veilleOn(bool active) {
    veilleActif = active;
}