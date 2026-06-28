#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================
// CONFIGURAÇÕES DE REDE
// ========================

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Ubidots
const char* mqtt_server = "industrial.api.ubidots.com";
const int mqtt_port = 1883;
const char* TOKEN = "SEU_TOKEN_AQUI";
const char* DEVICE_LABEL = "esp32-fazenda";

// ========================
// PINOS DO ESP32
// ========================

#define PINO_SENSOR_SOLO 34
#define PINO_LDR 35
#define PINO_RELE 2
#define PINO_DHT 4
#define DHTTYPE DHT22

#define PINO_COOLER 18
#define PINO_LUZ_ARTIFICIAL 19
#define PINO_BUZZER 23

// ========================
// OBJETOS
// ========================

DHT dht(PINO_DHT, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========================
// VARIÁVEIS GLOBAIS
// ========================

unsigned long ultimoEnvio = 0;
unsigned long ultimaTrocaLCD = 0;

const unsigned long intervaloEnvio = 3000;
const unsigned long intervaloLCD = 2500;

int telaLCD = 0;

// ========================
// CONEXÃO WIFI
// ========================

void conectarWiFi() {
  Serial.println();
  Serial.println("Conectando ao WiFi Wokwi-GUEST...");

  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi conectado");
  delay(1000);
}

// ========================
// CONEXÃO MQTT / UBIDOTS
// ========================

void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao Ubidots MQTT... ");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando");
    lcd.setCursor(0, 1);
    lcd.print("Ubidots...");

    String clientId = "ESP32-Estufa-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), TOKEN, "")) {
      Serial.println("Conectado!");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ubidots OK");
      delay(1000);
    } else {
      Serial.print("Falhou. rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// ========================
// LCD COM TELAS ALTERNADAS
// ========================

void atualizarLCD(
  int umidade_solo,
  float temperatura,
  float umidade_ar,
  int luminosidade,
  int status_bomba,
  int status_cooler,
  int status_luz_artificial,
  int status_alerta,
  bool mqtt_ok
) {
  if (millis() - ultimaTrocaLCD >= intervaloLCD) {
    ultimaTrocaLCD = millis();
    telaLCD++;

    if (telaLCD > 3) {
      telaLCD = 0;
    }

    lcd.clear();
  }

  if (telaLCD == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Solo:");
    lcd.print(umidade_solo);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Bomba:");
    if (status_bomba == 1) {
      lcd.print("LIGADA ");
    } else {
      lcd.print("DESLIG.");
    }
  }

  else if (telaLCD == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperatura, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("UmidAr:");
    lcd.print(umidade_ar, 0);
    lcd.print("%");
  }

  else if (telaLCD == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Luz:");
    lcd.print(luminosidade);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Lamp:");
    if (status_luz_artificial == 1) {
      lcd.print("LIGADA ");
    } else {
      lcd.print("DESLIG.");
    }
  }

  else if (telaLCD == 3) {
    lcd.setCursor(0, 0);

    if (status_alerta == 1) {
      lcd.print("ALERTA ESTUFA!");
    } else {
      lcd.print("Estufa OK");
    }

    lcd.setCursor(0, 1);
    lcd.print("Cooler:");
    if (status_cooler == 1) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }

    lcd.print(" M:");
    if (mqtt_ok) {
      lcd.print("OK");
    } else {
      lcd.print("OFF");
    }
  }
}

// ========================
// SETUP
// ========================

void setup() {
  Serial.begin(115200);

  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_COOLER, OUTPUT);
  pinMode(PINO_LUZ_ARTIFICIAL, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);

  digitalWrite(PINO_RELE, LOW);
  digitalWrite(PINO_COOLER, LOW);
  digitalWrite(PINO_LUZ_ARTIFICIAL, LOW);
  digitalWrite(PINO_BUZZER, LOW);

  dht.begin();

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Estufa Smart");
  lcd.setCursor(0, 1);
  lcd.print("ESP32 + IoT");
  delay(2000);

  conectarWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

// ========================
// LOOP PRINCIPAL
// ========================

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }

  client.loop();

  // ========================
  // 1. LEITURA DOS SENSORES
  // ========================

  int valor_solo = analogRead(PINO_SENSOR_SOLO);
  int umidade_solo = map(valor_solo, 0, 4095, 0, 100);

  int valor_luz = analogRead(PINO_LDR);
  int luminosidade_pct = map(valor_luz, 4095, 0, 0, 100);

  float temperatura = dht.readTemperature();
  float umidade_ar = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade_ar)) {
    Serial.println("Falha na leitura do DHT22!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro DHT22");
    lcd.setCursor(0, 1);
    lcd.print("Ver sensor");

    tone(PINO_BUZZER, 1000);
    delay(2000);
    noTone(PINO_BUZZER);

    return;
  }

  // ========================
  // 2. REGRA DA IRRIGAÇÃO
  // ========================

  int status_bomba = 0;
  String motivo = "";

  if (umidade_solo < 35) {
    status_bomba = 1;
    motivo = "Emergencia: solo seco";
  }
  else if (umidade_solo >= 35 && umidade_solo < 55) {
    if (temperatura < 30.0 && luminosidade_pct < 60 && umidade_ar < 75.0) {
      status_bomba = 1;
      motivo = "Irrigacao otimizada";
    } else {
      status_bomba = 0;
      motivo = "Bloqueio climatico";
    }
  }
  else {
    status_bomba = 0;
    motivo = "Solo adequado";
  }

  // ========================
  // 3. REGRA DO COOLER
  // ========================

  int status_cooler = 0;

  if (temperatura > 30.0) {
    status_cooler = 1;
  }

  // ========================
  // 4. REGRA DA LUZ ARTIFICIAL
  // ========================

  int status_luz_artificial = 0;

  if (luminosidade_pct < 35) {
    status_luz_artificial = 1;
  }

  // ========================
  // 5. REGRA DO ALERTA
  // ========================

  int status_alerta = 0;

  if (umidade_solo < 25 || temperatura > 35.0) {
    status_alerta = 1;
  }

  // ========================
  // 6. ACIONAMENTO DOS ATUADORES
  // ========================

  digitalWrite(PINO_RELE, status_bomba == 1 ? HIGH : LOW);
  digitalWrite(PINO_COOLER, status_cooler == 1 ? HIGH : LOW);
  digitalWrite(PINO_LUZ_ARTIFICIAL, status_luz_artificial == 1 ? HIGH : LOW);

  if (status_alerta == 1) {
    tone(PINO_BUZZER, 1000);
  } else {
    noTone(PINO_BUZZER);
  }

  bool mqtt_ok = client.connected();

  atualizarLCD(
    umidade_solo,
    temperatura,
    umidade_ar,
    luminosidade_pct,
    status_bomba,
    status_cooler,
    status_luz_artificial,
    status_alerta,
    mqtt_ok
  );

  // ========================
  // 7. ENVIO PARA UBIDOTS
  // ========================

  if (millis() - ultimoEnvio >= intervaloEnvio) {
    ultimoEnvio = millis();

    char payload[500];

    sprintf(
      payload,
      "{\"umidade_solo\": %d, \"temperatura\": %.1f, \"umidade_ar\": %.1f, \"luminosidade\": %d, \"bomba\": %d, \"cooler\": %d, \"luz_artificial\": %d, \"alerta\": %d}",
      umidade_solo,
      temperatura,
      umidade_ar,
      luminosidade_pct,
      status_bomba,
      status_cooler,
      status_luz_artificial,
      status_alerta
    );

    String topic = String("/v1.6/devices/") + DEVICE_LABEL;

    client.publish(topic.c_str(), payload);

    Serial.println("-----------------------------");
    Serial.print("Payload enviado: ");
    Serial.println(payload);

    Serial.print("Motivo irrigacao: ");
    Serial.println(motivo);

    Serial.print("Cooler: ");
    Serial.println(status_cooler == 1 ? "LIGADO" : "DESLIGADO");

    Serial.print("Luz artificial: ");
    Serial.println(status_luz_artificial == 1 ? "LIGADA" : "DESLIGADA");

    Serial.print("Alerta: ");
    Serial.println(status_alerta == 1 ? "ATIVO" : "NORMAL");
  }
}