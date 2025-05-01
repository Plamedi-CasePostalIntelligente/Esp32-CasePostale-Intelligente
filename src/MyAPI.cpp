#include "MyAPI.h"

MyAPI::MyAPI()
{
    // Initialisation des variables du broker
    _mqttAddress = "";
    _mqttPort = "";
    _mqttUser = "";
    _mqttPassword = "";

    // Initialisation des variables pour lockersController
    _isFacteur = false;
    _userType = "";
    _hasFactorDelivery = "";
    _idHasDelivery = "";
    _idHasClient = "";
    _isClientDelivered = "";
    _isDelivered = false;
    _isClientCaseFull = "";
    _isFull = false;
    _isCaseNumber = "";
    _caseNumber = "";
    _caseState = -1;
    _deliveryState = -1;
}

const char api_url[100] = "https://casepostaleapi.merdyspeed.ca/";

bool MyAPI::getBrokerInfo()
{
    HTTPClient http;

    Serial.println("Récupération des informations du broker...");

    http.begin("https://casepostaleapi.merdyspeed.ca/api/broker/infoBroker");
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        _mqttAddress = doc["mqttAddress"].as<String>();
        _mqttPort = doc["mqttPort"].as<String>();
        _mqttUser = doc["mqttUser"].as<String>();
        _mqttPassword = doc["mqttPassword"].as<String>();

        Serial.println("Informations du broker récupérées :");
        Serial.println("Adresse: " + _mqttAddress);
        Serial.println("Port: " + _mqttPort);
        Serial.println("Utilisateur: " + _mqttUser);

        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::checkUserExists(String uid)
{
    HTTPClient http;

    Serial.println("Vérification de l'existence de l'utilisateur avec UID: " + uid);

    String url = String(api_url) + "api/getUserByUid/" + uid;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "Existant")
        {
            Serial.println("Utilisateur trouvé");
            http.end();
            return true;
        }

        Serial.println("Utilisateur non trouvé ou réponse inattendue");
        http.end();
        return false;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::verifyIfFactor(String uid)
{
    HTTPClient http;

    Serial.println("Vérification si l'utilisateur avec UID " + uid + " est un facteur...");

    String url = String(api_url) + "verifyIfFactor/" + uid;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "User not found")
        {
            Serial.println("Utilisateur non trouvé");
            http.end();
            return false;
        }

        _isFacteur = doc["is_facteur"].as<bool>();
        _userType = doc["UserType"].as<String>();

        Serial.println("Utilisateur est un " + _userType + " (isFacteur: " + String(_isFacteur) + ")");
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::verifyIfHasDelivery(String uid)
{
    HTTPClient http;

    Serial.println("Vérification si l'utilisateur avec UID " + uid + " a une livraison...");

    String url = String(api_url) + "verifyIfHasDelivery/" + uid;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "User not found")
        {
            Serial.println("Utilisateur non trouvé");
            http.end();
            return false;
        }

        _hasFactorDelivery = doc["hasFactorDelivery"].as<String>();
        _idHasDelivery = doc["idHasDelivery"].as<String>();
        _idHasClient = doc["idHasClient"].as<String>();

        Serial.println("hasFactorDelivery: " + _hasFactorDelivery);
        Serial.println("idHasDelivery: " + _idHasDelivery);
        Serial.println("idHasClient: " + _idHasClient);
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::isDelivered(String uid)
{
    HTTPClient http;

    Serial.println("Vérification si l'utilisateur avec UID " + uid + " a une livraison livrée...");

    String url = String(api_url) + "isDelivered/" + uid;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "User not found")
        {
            Serial.println("Utilisateur non trouvé");
            http.end();
            return false;
        }

        _isClientDelivered = doc["isClientDelivered"].as<String>();
        _isDelivered = doc["isDelivered"].as<bool>();

        Serial.println("isClientDelivered: " + _isClientDelivered);
        Serial.println("isDelivered: " + String(_isDelivered));
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::isCaseFilled(String uid)
{
    HTTPClient http;

    Serial.println("Vérification si le casier de l'utilisateur avec UID " + uid + " est plein...");

    String url = String(api_url) + "isCaseFilled/" + uid;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "User not found")
        {
            Serial.println("Utilisateur non trouvé");
            http.end();
            return false;
        }

        _isClientCaseFull = doc["isClientCaseFull"].as<String>();
        _isFull = doc["isFull"].as<bool>();

        Serial.println("isClientCaseFull: " + _isClientCaseFull);
        Serial.println("isFull: " + String(_isFull));
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::openCase(String uid)
{
    HTTPClient http;

    Serial.println("Récupération du numéro de casier pour l'utilisateur avec UID " + uid + "...");

    String url = String(api_url) + "openCase/" + uid; // URL mise à jour
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && doc["message"] == "User not found")
        {
            Serial.println("Utilisateur non trouvé");
            http.end();
            return false;
        }

        _isCaseNumber = doc["isCaseNumber"].as<String>();
        _caseNumber = doc["caseNumber"].as<String>();

        Serial.println("isCaseNumber: " + _isCaseNumber);
        Serial.println("caseNumber: " + _caseNumber);
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::updateCaseState(String uid)
{
    HTTPClient http;

    Serial.println("Mise à jour de l'état du casier pour l'utilisateur avec UID " + uid + "...");

    String url = String(api_url) + "updateCaseState/" + uid; // URL mise à jour
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && (doc["message"] == "Locker not found" || doc["message"] == "Aucune mise à jour effectuée"))
        {
            Serial.println(doc["message"].as<String>());
            http.end();
            return false;
        }

        _caseState = doc["response"].as<int>();

        Serial.println("État du casier mis à jour: " + String(_caseState));
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::updateDeliveryState(String uid)
{
    HTTPClient http;

    Serial.println("Mise à jour de l'état de la livraison pour l'utilisateur avec UID " + uid + "...");

    String url = String(api_url) + "updateDeliveryState/" + uid; // URL mise à jour
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (doc.containsKey("message") && (doc["message"] == "Delivery not found" || doc["message"] == "Aucune mise à jour effectuée"))
        {
            Serial.println(doc["message"].as<String>());
            http.end();
            return false;
        }

        _deliveryState = doc["isDelivered"].as<int>();

        Serial.println("État de la livraison mis à jour: " + String(_deliveryState));
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}