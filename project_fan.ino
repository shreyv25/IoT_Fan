#define CAYENNE_PRINT Serial 
#define DHTPIN 5              
int relay = 0;
#define DHTTYPE DHT11  

#include <CayenneMQTTESP8266.h>
#include <DHT.h>

// WiFi network info.
char ssid[] = "dlink-CC0D";
char wifiPassword[] = "chmtd93436";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "24023180-3ffe-11ed-bf0a-bb4ba43bd3f6";
char password[] = "95f5f36e4f384b42baf34d1c809f30072ece7134";
char clientID[] = "492d1dd0-3ffe-11ed-baf6-35fab7fd0ac8";

DHT dht(DHTPIN, DHTTYPE);

float h;
float t;
int ato;
int fan_state;
bool fstt;
int threshold;
void setup() {
  Serial.begin(9600);
  Serial.print("Setup");
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  dht.begin();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  ato = 1;
  fan_state = 0;
  fstt = 0;
  threshold = 25;
}

void loop() {
  //Run Cayenne Functions
  Cayenne.loop();
}

CAYENNE_OUT_DEFAULT() {   Cayenne.celsiusWrite(1, t);   Cayenne.virtualWrite(5, fan_state);}
CAYENNE_IN(0) {
  Serial.println("Fan Switch: ");
  fan_state = getValue.asInt();
  Serial.print(fan_state);
  if (ato != 1) {
    if (fan_state == 1) {
      digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;
    } else {
      digitalWrite(relay, HIGH);fan_state = 0;fstt = fan_state;
    }
  }else{
    if (t > threshold) {digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;}
    else { digitalWrite(relay, HIGH); fan_state = 0;fstt = fan_state;}
  }
  CAYENNE_OUT_DEFAULT();
}

CAYENNE_IN(1) {
  ato = getValue.asInt();
  Serial.println("Automatic mode changed to");fstt = fan_state;
  Serial.print(ato);
  if (ato != 1) {
  if (fan_state == 1) {
      digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;
  } else {
      digitalWrite(relay, HIGH);fan_state = 0;fstt = fan_state;
  }
  }else {
    if (t > threshold) {digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;}
    else { digitalWrite(relay, HIGH); fan_state = 0;fstt = fan_state;}
  }
  CAYENNE_OUT_DEFAULT();
}

CAYENNE_IN(2)
{
  threshold = getValue.asInt();
  Serial.print("Threshold Temperature changed to: ");
  Serial.println(threshold);
  if (ato == 1) {
    if (t > threshold) {digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;}
    else { digitalWrite(relay, HIGH); fan_state = 0;fstt = fan_state;}
  }
  CAYENNE_OUT_DEFAULT();
}

CAYENNE_OUT(5) {
  Cayenne.virtualWrite(5, fstt);
}

CAYENNE_OUT(1) {
  do {
    t = dht.readTemperature();
    delay(1000);
  } while (isnan(t));
  if (ato == 1) {
    if (t > threshold) {digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;}
    else { digitalWrite(relay, HIGH); fan_state = 0;fstt = fan_state;}
  }else{
    if (fan_state == 1) {
      digitalWrite(relay, LOW);fan_state = 1;fstt = fan_state;
    } else {
      digitalWrite(relay, HIGH);fan_state = 0;fstt = fan_state;
    }
  }
  Serial.print("temperature: ");
  Serial.println(t);
  Cayenne.celsiusWrite(1, t);
  CAYENNE_OUT_DEFAULT();
}