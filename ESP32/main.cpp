#include <WiFi.h>
#include <HTTPClient.h>

// Configurations
const char* WIFI_SSID = "Livebox-6F40";
const char* WIFI_PASSWORD = "pNcQpvRePLkHaYr6MA";
const char* SERVER_URL = "https://w6qh92oq5h.execute-api.eu-west-3.amazonaws.com";
const char* API_KEY = "qwertyuiop";
const char* CLIENT_ID = "1";

const int buttonPin = 25;
const int ledRPin = 32;
const int ledGPin = 33;
const int ledBPin = 12;
const int buzzerPin = 27;

unsigned long lastBlinkTime = 0;
unsigned long lastDebounceTime = 0;  // Pour le debounce
bool isOscillating = false;
bool lastButtonState = LOW;
unsigned long idleBlinkStart = 0;
const unsigned long blinkInterval = 10000;

// États
enum State {
    IDLE, SHORT_PRESS, LONG_PRESS, VERY_LONG_PRESS, 
    SHORT_PROCESSING, LONG_PROCESSING, VERY_LONG_PROCESSING,
    STATUS_ACCEPTED, STATUS_DENIED, STATUS_TIMEOUT,
    CONNECTING_WIFI, WIFI_SUCCESS, WIFI_FAIL
};

State currentState = IDLE;

void setColor(int r, int g, int b) {
    ledcWrite(0, r);
    ledcWrite(1, g);
    ledcWrite(2, b);
}

void oscillateLEDs(int rMax = 255, int gMax = 255, int bMax = 255) {
    if (isOscillating) {
        float progress = (sin(millis() / 1000.0 * PI * 2) + 1) / 2;
        setColor(rMax * (1 - progress), gMax * progress, bMax * (1 - progress));
    }
}

void playSound(const char* type) {
    if (strcmp(type, "shortPress") == 0) {
        tone(buzzerPin, 800, 300);
    } else if (strcmp(type, "longPress") == 0) {
        tone(buzzerPin, 500, 500);
    } else if (strcmp(type, "shortAccepted") == 0) {
        for (int i = 0; i < 2; i++) {
            tone(buzzerPin, 800, 300);
            delay(150);
        }
    } else if (strcmp(type, "denied") == 0) {
        for (int i = 0; i < 3; i++) {
            tone(buzzerPin, 500, 500);
        }
    } else if (strcmp(type, "wifiSuccess") == 0) {
        tone(buzzerPin, 800, 300);
    } else if (strcmp(type, "error") == 0) {
        for (int i = 0; i < 5; i++) {
            tone(buzzerPin, 300, 200);
            delay(150);
        }
    }
}

bool sendRequest(const char* endpoint) {
    HTTPClient http;
    http.begin(String(SERVER_URL) + endpoint + "?client_id=" + CLIENT_ID);
    http.addHeader("Authorization", API_KEY);

    printf("Sending request to %s\n", endpoint);
    printf("Client ID : %s\n", CLIENT_ID);
    printf("API Key : %s\n", API_KEY);
    String urlString = String(SERVER_URL) + endpoint + "?client_id=" + CLIENT_ID;
    printf("URL : %s\n", urlString.c_str());

    int httpCode = http.GET();
    printf("HTTP Code : %d\n", httpCode);

    String payload = http.getString();
    printf("Payload : %s\n", payload.c_str());

    if (httpCode != HTTP_CODE_OK) {
        Serial.println("Erreur lors de la requête.");
        playSound("error");
        http.end();
        return false;
    }

    if (payload == "accepted") {
        currentState = STATUS_ACCEPTED;
        playSound("shortAccepted");
    } else if (payload == "denied") {
        currentState = STATUS_DENIED;
        playSound("denied");
    }

    http.end();
    return true;
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    currentState = CONNECTING_WIFI;
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    isOscillating = true;

    unsigned long wifiStartTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStartTime < 10000) {
        oscillateLEDs();
        delay(100);
    }

    isOscillating = false;

    if (WiFi.status() == WL_CONNECTED) {
        currentState = WIFI_SUCCESS;
        Serial.println("Successfully connected to WiFi.");
    } else {
        currentState = WIFI_FAIL;
        Serial.println("Failed to connect to WiFi.");
    }
}

void handleButton() {
    unsigned long currentTime = millis();
    static unsigned long buttonDownTime = 0;

    bool reading = digitalRead(buttonPin);

    if ((currentTime - lastDebounceTime) > 50) {
        if (reading != lastButtonState) {
            lastDebounceTime = currentTime;

            if (reading) {
                buttonDownTime = currentTime;
            } else {
                unsigned long buttonDuration = currentTime - buttonDownTime;
                if (buttonDuration > 5000) {
                    currentState = VERY_LONG_PRESS;

                } else if (buttonDuration > 1000) {
                    currentState = LONG_PRESS;
                } else {
                    currentState = SHORT_PRESS;
                }
            }
        }
    }

    lastButtonState = reading;
}


void handleState() {
    switch (currentState) {
        case IDLE:
        Serial.println("IDLE");
            setColor(0, 0, 0);
            if (millis() - idleBlinkStart >= blinkInterval) {
                if (digitalRead(ledGPin)) {
                    setColor(0, 0, 0);
                } else {
                    setColor(0, 128, 0);
                }
            idleBlinkStart = millis();
            }
            break;
        case SHORT_PRESS:
            Serial.println("SHORT_PRESS");
            setColor(0, 255, 0);
            playSound("shortPress");
            sendRequest("/send_short");
            isOscillating = true;
            currentState = SHORT_PROCESSING;
            break;
        case LONG_PRESS:
            Serial.println("LONG_PRESS");
            setColor(0, 0, 255);
            playSound("longPress");
            sendRequest("/send_long");
            currentState = LONG_PROCESSING;
            break;
        case VERY_LONG_PRESS:
            Serial.println("VERY_LONG_PRESS");
            setColor(255, 0, 0);
            playSound("error");
            currentState = VERY_LONG_PROCESSING;
            break;
        case SHORT_PROCESSING:
            Serial.println("SHORT_PROCESSING");
            isOscillating = true;
            oscillateLEDs(0,255,0);
            sendRequest("/get_status");
            break;
        case LONG_PROCESSING:
            Serial.println("LONG_PROCESSING");
            setColor(128, 0, 128);
            sendRequest("/get_status");
            break;
        case VERY_LONG_PROCESSING:
            Serial.println("VERY_LONG_PROCESSING");
            setColor(128, 128, 0);
            ESP.restart();
            break;
        case STATUS_ACCEPTED:
            Serial.println("STATUS_ACCEPTED");
            isOscillating = false;
            setColor(0, 255, 0);
            playSound("shortAccepted");
            delay(4000);
            currentState = IDLE;
            break;
        case STATUS_DENIED:
            Serial.println("STATUS_DENIED");
            setColor(255, 0, 0);
            playSound("denied");
            currentState = IDLE;
            break;
        case STATUS_TIMEOUT:
            Serial.println("STATUS_TIMEOUT");
            setColor(255, 0, 0);
            playSound("error");
            currentState = IDLE;
            break;
        case CONNECTING_WIFI:
            Serial.println("CONNECTING_WIFI");
            setColor(0, 0, 255);
            break;
        case WIFI_SUCCESS:
            Serial.println("WIFI_SUCCESS");
            setColor(0, 0, 0);
            playSound("wifiSuccess");
            currentState = IDLE;
            break;
        case WIFI_FAIL:
            Serial.println("WIFI_FAIL");
            setColor(255, 0, 0);
            playSound("error");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLDOWN);
    pinMode(ledRPin, OUTPUT);
    pinMode(ledGPin, OUTPUT);
    pinMode(ledBPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);

    ledcSetup(0, 5000, 8);
    ledcSetup(1, 5000, 8);
    ledcSetup(2, 5000, 8);

    ledcAttachPin(ledRPin, 0);
    ledcAttachPin(ledGPin, 1);
    ledcAttachPin(ledBPin, 2);

    connectToWiFi();

    setColor(0,0,255);
    delay(100);
}

void loop() {
    handleButton();
    handleState();
    oscillateLEDs();
}