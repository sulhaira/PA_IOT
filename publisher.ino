#define BLYNK_TEMPLATE_ID "TMPL6dpFQe7Y6"
#define BLYNK_TEMPLATE_NAME "posttest2"
#define BLYNK_AUTH_TOKEN "OVCBCdvdy23TkIlcfoLB-TJvP9sNue-4"

#define BLYNK_PRINT Serial



#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

const char* mqtt_server = "broker.emqx.io";


#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Universitas Mulawarman"; //nama hotspot yang digunakan
char pass[] = ""; //password hotspot yang digunakan


#define LED_PIN D2
#define DHTPIN D4          // Mention the digital pin where you connected 
#define DHTTYPE DHT11    // DHT 11  
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
#define WAKTUDHT 1000

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
float nilaiKelembaban;
float nilaiTemperatur;
#define MSG_DHT (0)
char msg[MSG_DHT];


unsigned long timerDHT = 0;


//setup wifi

void setup_wifi() {
  delay(10);//   digitalWrite(LEDPIN1, param.asInt()); // Mengubah nilai LED dari Blynk

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// fungsi untuk menghubungkan ke broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("ruangan/iot_a_1/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// menerima data pesan on off led
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan Diterima [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    
    digitalWrite(LED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    
    digitalWrite(LED_PIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
void setup(){
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DHTPIN,INPUT);
  
  dht.begin();
  // timer.setInterval(2500L, sendSensor);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(){
  Blynk.run();
  timer.run();
  if (!client.connected()) {
    reconnect();
    }

  client.loop();
  
  if ((millis() - timerDHT) > WAKTUDHT) {
    // Update the timer
    timerDHT = millis();

     nilaiKelembaban = dht.readHumidity();
    // Read temperature as Celsius (the default)
    nilaiTemperatur = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(false);

    // Check if any reads failed and exit early (to try again)
    if (isnan(nilaiKelembaban) || isnan(nilaiTemperatur) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    int kelembaban = (int)nilaiKelembaban;
    // Read temperature as Celsius (the default)
    int temperatur = (int)nilaiTemperatur;
    Serial.print("Suhu : ");
    Serial.println(temperatur);

    delay(2000);
    snprintf (msg, MSG_DHT, "%s", itoa(temperatur,msg,10)); //ubah nilai int ke string
    client.publish("ruangan/iot_a_1/lokasi/suhu", msg, true); //publish message suhu
    snprintf (msg, MSG_DHT, "%s", itoa(kelembaban,msg,10)); //ubah nilai int ke string
    Serial.print("Kelembaban : ");
    Serial.println(kelembaban);
    client.publish("ruangan/iot_a_1/lokasi/kelembaban", msg, true); //publish message kelembaban

}

 }


