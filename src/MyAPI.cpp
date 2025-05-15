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
    _idFactorHasDelivery = "";
    _idClientHasDelivery = "";
    _isClientDelivered = "";
    _isDelivered = false;
    _isClientCaseFull = "";
    _isFull = false;
    _isCaseNumber = "";
    _caseNumbers = ""; // Initialize as empty string
    _caseState = -1;
    _deliveryState = -1;
    _insertSuccess = false;
}

#ifdef LOCAL_TESTING
const char api_url[100] = "http://192.168.113.138:3003/";
#else
const char api_url[100] = "https://casepostaleapi.merdyspeed.ca/";
#endif

bool MyAPI::getBrokerInfo()
{
    HTTPClient http;

    Serial.println("Récupération des informations du broker...");

    String url = String(api_url) + "api/broker/infoBroker";
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

    String url = String(api_url) + "api/users/getUserByUid/" + uid;
    
    Serial.println("URL utilisée: " + url);
    
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
    Serial.println("URL utilisée: " + url);
    http.end();
    return false;
}

bool MyAPI::verifyIfFactor(String uid)
{
    HTTPClient http;

    Serial.println("Vérification si l'utilisateur avec UID " + uid + " est un facteur...");

    String url = String(api_url) + "api/lockers/verifyIfFactor/" + uid;
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

    String url = String(api_url) + "api/lockers/verifyIfHasDelivery/" + uid;
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
        _idFactorHasDelivery = doc["idFactorHasDelivery"].as<String>();
        _idClientHasDelivery = doc["idClientHasDelivery"].as<String>();

        Serial.println("hasFactorDelivery: " + _hasFactorDelivery);
        Serial.println("idFactorHasDelivery: " + _idFactorHasDelivery);
        Serial.println("idClientHasDelivery: " + _idClientHasDelivery);
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

    String url = String(api_url) + "api/lockers/isDelivered/" + uid;
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

    String url = String(api_url) + "api/lockers/isCaseFilled/" + uid;
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

    String url = String(api_url) + "api/lockers/openCase/" + uid;
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
        // Parse caseNumbers array and join into a comma-separated string
        JsonArray caseNumbers = doc["caseNumbers"].as<JsonArray>();
        _caseNumbers = "";
        for (size_t i = 0; i < caseNumbers.size(); i++)
        {
            _caseNumbers += caseNumbers[i].as<String>();
            if (i < caseNumbers.size() - 1) _caseNumbers += ",";
        }

        Serial.println("isCaseNumber: " + _isCaseNumber);
        Serial.println("caseNumbers: " + _caseNumbers);
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

    Serial.println("Mise à jour de l'état du casier pour UID: " + uid + "...");

    String url = String(api_url) + "api/lockers/updateCaseState";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["uid"] = uid;
    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (responseDoc.containsKey("message") && (responseDoc["message"] == "Locker not found" || responseDoc["message"] == "Aucune mise à jour effectuée"))
        {
            Serial.println(responseDoc["message"].as<String>());
            http.end();
            return false;
        }

        _caseState = responseDoc["response"].as<int>();
        //Serial.println("État du casier mis à jour: " + String(_caseState));
        http.end();
        return true;
    }

    //Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::updateDeliveryState(String uid)
{
    HTTPClient http;

    //Serial.println("Mise à jour de l'état de la livraison pour UID: " + uid + "...");

    String url = String(api_url) + "api/lockers/updateDeliveryState";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["uid"] = uid;
    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (responseDoc.containsKey("message") && (responseDoc["message"] == "Delivery not found" || responseDoc["message"] == "Aucune mise à jour effectuée"))
        {
            Serial.println(responseDoc["message"].as<String>());
            http.end();
            return false;
        }

        _deliveryState = responseDoc["isDelivered"].as<int>();
        //Serial.println("État de la livraison mis à jour: " + String(_deliveryState));
        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}

bool MyAPI::insertAccessTries(String uid, bool status)
{
    HTTPClient http;

    //Serial.println("Insertion d'une tentative d'accès pour UID: " + uid + "...");

    String url = String(api_url) + "api/accessTry/insertAccessTry";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["uid"] = uid;
    doc["status"] = status;
    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);

    if (httpCode == HTTP_CODE_CREATED)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, payload);

        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        if (responseDoc["status"].as<bool>())
        {
            _insertSuccess = true;
            //Serial.println("Tentative insérée avec succès");
        }
        else
        {
            //Serial.println("Échec de l'insertion: " + responseDoc["message"].as<String>());
        }

        http.end();
        return true;
    }
    else
    {
        Serial.print("Erreur HTTP: ");
        Serial.println(httpCode);
        Serial.println("URL utilisée: " + url);
        http.end();
        return false;
    }
}