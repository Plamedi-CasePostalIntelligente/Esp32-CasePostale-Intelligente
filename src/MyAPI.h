#ifndef MYAPI_H
#define MYAPI_H

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
    bool openCase(String uid);
    bool updateCaseState(String uid);
    bool updateDeliveryState(String uid);
    bool insertAccessTries(String uid, bool status);

    // Getters
    String getMqttAddress() { return _mqttAddress; }
    String getMqttPort() { return _mqttPort; }
    String getMqttUser() { return _mqttUser; }
    String getMqttPassword() { return _mqttPassword; }
    bool getIsFacteur() { return _isFacteur; }
    String getUserType() { return _userType; }
    String getHasFactorDelivery() { return _hasFactorDelivery; }
    String getidFactorHasDelivery() { return _idFactorHasDelivery; }
    String getIdClientHasDelivery() { return _idClientHasDelivery; }
    String getIsClientDelivered() { return _isClientDelivered; }
    bool getIsDelivered() { return _isDelivered; }
    String getIsClientCaseFull() { return _isClientCaseFull; }
    bool getIsFull() { return _isFull; }
    String getIsCaseNumber() { return _isCaseNumber; }
    String getCaseNumbers() { return _caseNumbers; } // Updated to handle multiple case numbers
    int getCaseState() { return _caseState; }
    int getDeliveryState() { return _deliveryState; }
    bool getInsertSuccess() { return _insertSuccess; }

private:
    String _mqttAddress;
    String _mqttPort;
    String _mqttUser;
    String _mqttPassword;

    bool _isFacteur;
    String _userType;
    String _hasFactorDelivery;
    String _idFactorHasDelivery;
    String _idClientHasDelivery;
    String _isClientDelivered;
    bool _isDelivered;
    String _isClientCaseFull;
    bool _isFull;
    String _isCaseNumber;
    String _caseNumbers; // Updated to store comma-separated case numbers
    int _caseState;
    int _deliveryState;
    bool _insertSuccess;
};

#endif