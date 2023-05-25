
#define BLYNK_TEMPLATE_ID "TMPL6dpFQe7Y6"
#define BLYNK_TEMPLATE_NAME "posttest2"
#define BLYNK_AUTH_TOKEN "OVCBCdvdy23TkIlcfoLB-TJvP9sNue-4"

#define BLYNK_PRINT Serial

// #include <WiFi.h>
#include <PubSubClient.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = BLYNK_AUTH_TOKEN;

// Update these with values suitable for your network.
const char* ssid = "Universitas Mulawarman";
const char* password = ""; 
const char* mqtt_server = "broker.emqx.io"; // broker gratisan

//LED pin

#define LEDPIN1 D4 //Hijau
#define LEDPIN2 D2 //Kuning
#define LEDPIN3 D0 //Merah
#define BUZZER_PIN D7
BlynkTimer timer;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;aS

int Hum; //int kelembaban
int Temp; //int suhu


//Blynk
BLYNK_WRITE(V1) { // V2 adalah pin LED yang sudah kita atur pada Blynk
  digitalWrite(LEDPIN1, param.asInt()); // Mengubah nilai LED dari Blynk
}

BLYNK_WRITE(V2) { // V2 adalah pin LED yang sudah kita atur pada Blynk
  digitalWrite(LEDPIN2, param.asInt()); // Mengubah nilai LED dari Blynk
}
BLYNK_WRITE(V3) { // V2 adalah pin LED yang sudah kita atur pada Blynk
  digitalWrite(LEDPIN3, param.asInt()); // Mengubah nilai LED dari Blynk
}

//atur wifi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

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

// Fungsi untuk menerima data
void callback(char* topic, byte* payload, unsigned int length) {
   if(strcmp(topic, "ruangan/iot_a_1/lokasi/kelembaban") == 0)
   {
        String hum = ""; 
        for (int i = 0; i < length; i++) {
          hum += (char)payload[i];
        }
        Hum  = hum.toInt(); 
        Serial.print("Kelembapan [");
        Serial.print(Hum);
        Serial.println("] ");
       
   }
   else if(strcmp(topic, "ruangan/iot_a_1/lokasi/suhu") == 0)
   {
        
        String temp = ""; 
        for (int i = 0; i < length; i++) {
          temp += (char)payload[i];
        }
        Temp  = temp.toInt();
        Serial.print("Suhu [");
        Serial.print(Temp);
        Serial.println("] ");
   };
   }

// fungsi untuk mengubungkan ke broker
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
      // Once connected, publish an announcement...
      client.subscribe("ruangan/iot_a_1/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, password);
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883); // setup awal ke server mqtt
  client.setCallback(callback); //panggil fungsi menerima pesan
}

void loop() {
  Blynk.run();
  timer.run();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Blynk.virtualWrite(V4, Hum); //tulis nilai kelembaban ke blynk
  Blynk.virtualWrite(V0, Temp); //tulis nilai suhu ke blynk

//kondisi kualitas udara di ruangan
  if ( Temp < 20 && Hum < 40)
    {
      digitalWrite(LEDPIN1, HIGH);
      digitalWrite(LEDPIN2, LOW);
      digitalWrite(LEDPIN3, LOW);
    }
    else if (Temp > 30 && Hum > 75){
    digitalWrite(LEDPIN3, HIGH);
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN2, LOW);
    tone(BUZZER_PIN, 200);
}
// Suhu rendah
  else if (Temp < 20 && 40 < Hum < 75 )
    {
    digitalWrite(LEDPIN1, HIGH);
    digitalWrite(LEDPIN2, LOW);
    digitalWrite(LEDPIN3, LOW);
} 
//suhu sedang
else if (20 < Temp < 30 && Hum < 40 )
    {
    digitalWrite(LEDPIN1, HIGH);
    digitalWrite(LEDPIN2, LOW);
    digitalWrite(LEDPIN3, LOW);
    }    
    else if (20 < Temp < 30 &&  Hum > 75 )
    {
    digitalWrite(LEDPIN2, HIGH);
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN3, LOW);
    }
    // suhu tinggi       
    else if ( Temp > 30 && 40 < Hum < 75 )
    {
    digitalWrite(LEDPIN2, HIGH);
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN3, LOW); 
    }

    //
    else if (20 < Temp < 30 && 40 < Hum < 75 )
    {
    digitalWrite(LEDPIN2, HIGH);
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN3, LOW);
    //tone(BUZZER_PIN, 1000, 50);
    //delay(5000);
    //noTone(BUZZER_PIN);
    }
}