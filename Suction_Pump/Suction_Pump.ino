const int TRIGGER_PIN = 2;
const int ECHO_PIN = 3;
const int RELAY_PIN = 4;
const int BUZZER1_PIN = 9;
const int BUZZER2_PIN = 10;
const int LDR_PIN = A0;

// Pengaturan Sensitivitas (Bisa Diubah Sesuai Kebutuhan)
const int BATAS_JARAK_AKTIVASI = 20;
const int AMBANG_BATAS_CAHAYA = 850;

// Pengaturan Waktu
const long INTERVAL_BACA_SENSOR = 100;
const long INTERVAL_ALARM_BIP = 300;
const long JEDA_TOGGLE = 500;
const int DURASI_INDIKATOR_BIP = 100;

// State Machine
enum AlatState {
  SIAGA,
  ALARM_PENUH
};
AlatState currentState = SIAGA;

// Variabel Global
long distance = 0;
int nilaiLDR = 0;
unsigned long waktuSebelumnya_sensor = 0;
unsigned long waktuSebelumnya_alarm = 0;
unsigned long waktuTerakhirToggle = 0;
bool isRelayActive = false;
bool objekTerdeteksiSebelumnya = false;
bool sedangBip = false;
unsigned long waktuMulaiBip = 0;

// Fungsi untuk menghindari kompilasi
void kontrolRelay(bool aktif);
void ubahState(AlatState newState);
void bacaSensor();
void jalankanStateSiaga();
void jalankanStateAlarmPenuh();
void cetakStatusDebug();

// Fungsi Setup
void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  kontrolRelay(false);
  Serial.println("--- Sistem Suction Pump Siap ---");
}

// Fungsi Loop Utama
void loop() {
  bacaSensor();

  if (nilaiLDR >= AMBANG_BATAS_CAHAYA) {
    if (currentState != ALARM_PENUH) {
      Serial.println(">>> EVENT: Overflow Terdeteksi!");
      ubahState(ALARM_PENUH);
    }
  } else {
    if (currentState != SIAGA) {
      Serial.println(">>> EVENT: Kondisi Normal Kembali");
      ubahState(SIAGA);
    }
  }
  
  switch (currentState) {
    case SIAGA:
      jalankanStateSiaga();
      break;
    case ALARM_PENUH:
      jalankanStateAlarmPenuh();
      break;
  }
}

// Logika Setiap State
void jalankanStateSiaga() {
  digitalWrite(BUZZER2_PIN, LOW);
  if (sedangBip && (millis() - waktuMulaiBip > DURASI_INDIKATOR_BIP)) {
    digitalWrite(BUZZER1_PIN, LOW);
    sedangBip = false;
  }
  bool objekTerdeteksiSekarang = (distance > 0 && distance <= BATAS_JARAK_AKTIVASI);
  if (objekTerdeteksiSekarang && !objekTerdeteksiSebelumnya) {
    if (millis() - waktuTerakhirToggle > JEDA_TOGGLE) {
      isRelayActive = !isRelayActive;
      kontrolRelay(isRelayActive);
      waktuTerakhirToggle = millis();
      digitalWrite(BUZZER1_PIN, HIGH);
      sedangBip = true;
      waktuMulaiBip = millis();
      Serial.print(">>> EVENT: TOGGLE! Relay -> ");
      Serial.println(isRelayActive ? "ON" : "OFF");
    }
  }
  objekTerdeteksiSebelumnya = objekTerdeteksiSekarang;
}

void jalankanStateAlarmPenuh() {
  kontrolRelay(false);
  isRelayActive = false;
  digitalWrite(BUZZER1_PIN, LOW);
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya_alarm >= INTERVAL_ALARM_BIP) {
    waktuSebelumnya_alarm = waktuSekarang;
    digitalWrite(BUZZER2_PIN, !digitalRead(BUZZER2_PIN));
  }
}


// Fungsi Bantu
void bacaSensor() {
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya_sensor >= INTERVAL_BACA_SENSOR) {
    waktuSebelumnya_sensor = waktuSekarang;
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    long durasi = pulseIn(ECHO_PIN, HIGH, 25000);
    distance = durasi / 58.2;
    nilaiLDR = analogRead(LDR_PIN);
    cetakStatusDebug();
  }
}

void kontrolRelay(bool aktif) {
  if (aktif) {
    digitalWrite(RELAY_PIN, LOW);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void ubahState(AlatState newState) {
  if (currentState != newState) {
    currentState = newState;
  }
}

void cetakStatusDebug() {
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | LDR: ");
  Serial.print(nilaiLDR);
  Serial.print(" | State: ");
  if (currentState == SIAGA) {
    Serial.print("SIAGA");
  } else {
    Serial.print("ALARM");
  }
  Serial.print(" | Relay: ");
  Serial.println(isRelayActive ? "ON" : "OFF");
}