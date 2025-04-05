/**
    Gestion d'un écran OLED utilisant un GPIO d'un contrôleur

    @file MyOled.cpp
    @author Alain Dubé (modifié pour 2 boutons, menu principal toujours affiché)
    @version 1.2 03/04/25
*/

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

// Initialisation
int MyOled::init(int displaySplashTime, uint8_t addrI2C) {
    // Configurer les broches SDA et SCL personnalisées
    Wire.begin(MY_OLED_SDA_PIN, MY_OLED_SCL_PIN);

    // Initialiser l'écran OLED avec les broches configurées
    if (!begin(SSD1306_SWITCHCAPVCC, addrI2C)) {
        return 1; // Échec de l'initialisation
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
    displayMainMenu(); // Affiche le menu principal dès l’init
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
        displayMainMenu(); // Revient au menu principal en cas de veille
        MyOled::veilleTimeLapse = 0;
        veilleActif = true;
    }
    return veilleActif;
}

// Affichage du menu principal
void MyOled::displayMainMenu() {
    clearDisplay();
    setTextSize(1);
    setTextColor(SSD1306_WHITE);

    for (int i = 0; i < 4; i++) {
        setCursor(10, 18 + i * 10);
        if (i == currentMenuIndex) {
            print("> ");
        } else {
            print("  ");
        }
        print(mainMenu[i]);
    }
    display();
}

// Affichage des sous-menus
void MyOled::displaySubMenu(int subMenuIndex, float temperature) {
    clearDisplay();
    setTextSize(1);
    setTextColor(SSD1306_WHITE);
    setCursor(10, 18);

    switch (subMenuIndex) {
        case 0: // État C1
            printIt(10, 18, "Casier 1", false);
            printIt(10, 28, "Etat: Plein", true);
            break;
        case 1: // État C2
            printIt(10, 18, "Casier 2", false);
            printIt(10, 28, "Etat: Vide", true);
            break;
        case 2: // Température
            printIt(10, 18, "Temperature", false);
            char tempStr[10];
            snprintf(tempStr, sizeof(tempStr), "%.1f C", temperature);
            printIt(10, 28, tempStr, true);
            break;
        case 3: // Statut Système
            printIt(10, 18, "Statut System", false);
            printIt(10, 28, "Wi-Fi: OK", true);
            break;
    }
}

// Bouton gauche : Défilement dans le menu principal
void MyOled::moveLeftButton() {
    if (currentSubMenu == -1) { // Dans le menu principal
        currentMenuIndex++;
        if (currentMenuIndex >= 4) { // Boucle au début après le dernier item
            currentMenuIndex = 0;
        }
        displayMainMenu();
    }
    // Pas de défilement dans les sous-menus
}

// Bouton droit : Entrée ou sortie du sous-menu
void MyOled::moveRightButton(float temp) {
    if (currentSubMenu == -1) {
        currentSubMenu = currentMenuIndex;
        displaySubMenu(currentSubMenu, temp); // Passe la température réelle
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
        write(130); // é
    if (spacialCaractere == "o")
        write(248); // Degré Celsius
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