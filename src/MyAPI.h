#ifndef MYAPI_H
#define MYAPI_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class MyAPI
{
public:
    MyAPI();
    bool getBrokerInfo();
    bool checkUserExists(String uid);
    bool verifyIfFactor(String uid);
    bool verifyIfHasDelivery(String uid);
    bool isDelivered(String uid);
    bool isCaseFilled(String uid);
    bool openCase(String uid); // Renommé
    bool updateCaseState(String uid); // Renommé
    bool updateDeliveryState(String uid); // Renommé

    // Getters pour les infos du broker
    String getMqttAddress() const { return _mqttAddress; }
    String getMqttPort() const { return _mqttPort; }
    String getMqttUser() const { return _mqttUser; }
    String getMqttPassword() const { return _mqttPassword; }

    // Getters pour les méthodes
    bool getIsFacteur() const { return _isFacteur; }
    String getUserType() const { return _userType; }
    String getHasFactorDelivery() const { return _hasFactorDelivery; }
    String getIdHasDelivery() const { return _idHasDelivery; }
    String getIdHasClient() const { return _idHasClient; }
    String getIsClientDelivered() const { return _isClientDelivered; }
    bool getIsDelivered() const { return _isDelivered; }
    String getIsClientCaseFull() const { return _isClientCaseFull; }
    bool getIsFull() const { return _isFull; }
    String getIsCaseNumber() const { return _isCaseNumber; }
    String getCaseNumber() const { return _caseNumber; }
    int getCaseState() const { return _caseState; }
    int getDeliveryState() const { return _deliveryState; }

private:
    // Infos du broker
    String _mqttAddress;
    String _mqttPort;
    String _mqttUser;
    String _mqttPassword;

    // Infos pour verifyIfFactor
    bool _isFacteur;
    String _userType;

    // Infos pour verifyIfHasDelivery
    String _hasFactorDelivery;
    String _idHasDelivery;
    String _idHasClient;

    // Infos pour isDelivered
    String _isClientDelivered;
    bool _isDelivered;

    // Infos pour isCaseFilled
    String _isClientCaseFull;
    bool _isFull;

    // Infos pour openCase
    String _isCaseNumber;
    String _caseNumber;

    // Infos pour updateCaseState et updateDeliveryState
    int _caseState;
    int _deliveryState;
};

#endif