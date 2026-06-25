// ============================================
// ARDUINO MEGA — FINAL 7 KONDISI
// SEMUA RODA HANYA MAJU ATAU BERHENTI
// ============================================

#define FL_RPWM  4
#define FL_LPWM  5
#define FR_RPWM  6
#define FR_LPWM  7
#define RL_RPWM  8
#define RL_LPWM  9
#define RR_RPWM  10
#define RR_LPWM  11

// --- SPEED ---
#define SPEED_FWD        25    // maju normal
#define SPEED_PIVOT      40    // belok biasa — roda luar
#define SPEED_SHARP      60    // belok tajam — roda luar
#define SPEED_ROTATION   80   // rotasi — roda luar

// --- MOTOR CONTROL (hanya maju atau berhenti) ---
void motorFL(int s) {
  s = constrain(s, 0, 255);    // tidak ada nilai negatif
  analogWrite(FL_RPWM, s);
  analogWrite(FL_LPWM, 0);
}
void motorFR(int s) {
  s = constrain(s, 0, 255);
  analogWrite(FR_RPWM, 0);
  analogWrite(FR_LPWM, s);
}
void motorRL(int s) {
  s = constrain(s, 0, 255);
  analogWrite(RL_RPWM, s);
  analogWrite(RL_LPWM, 0);
}
void motorRR(int s) {
  s = constrain(s, 0, 255);
  analogWrite(RR_RPWM, 0);
  analogWrite(RR_LPWM, s);
}

// --- GERAKAN ROBOT ---
void stopAll() {
  motorFL(0); motorFR(0); motorRL(0); motorRR(0);
}

// Semua roda maju sama rata
void maju() {
  motorFL(SPEED_FWD); motorFR(SPEED_FWD);
  motorRL(SPEED_FWD); motorRR(SPEED_FWD);
}

// Kiri berhenti, kanan jalan
void belokKiri() {
  motorFL(0);          motorFR(SPEED_PIVOT);
  motorRL(0);          motorRR(SPEED_PIVOT);
}

// Kanan berhenti, kiri jalan
void belokKanan() {
  motorFL(SPEED_PIVOT); motorFR(0);
  motorRL(SPEED_PIVOT); motorRR(0);
}

// Kiri berhenti, kanan lebih kencang
void kiriTajam() {
  motorFL(0);          motorFR(SPEED_SHARP);
  motorRL(0);          motorRR(SPEED_SHARP);
}

// Kanan berhenti, kiri lebih kencang
void kananTajam() {
  motorFL(SPEED_SHARP); motorFR(0);
  motorRL(SPEED_SHARP); motorRR(0);
}

// Kiri berhenti, kanan kecepatan penuh
void rotasiKiri() {
  motorFL(0);             motorFR(SPEED_ROTATION);
  motorRL(0);             motorRR(SPEED_ROTATION);
}

// Kanan berhenti, kiri kecepatan penuh
void rotasiKanan() {
  motorFL(SPEED_ROTATION); motorFR(0);
  motorRL(SPEED_ROTATION); motorRR(0);
}

// --- EKSEKUSI PERINTAH ---
void jalankanCmd(char cmd) {
  switch (cmd) {
    case 'F': maju();        Serial.println("→ MAJU");            break;
    case 'A': belokKiri();   Serial.println("→ BELOK KIRI");      break;
    case 'B': belokKanan();  Serial.println("→ BELOK KANAN");     break;
    case 'C': kiriTajam();   Serial.println("→ KIRI TAJAM");      break;
    case 'D': kananTajam();  Serial.println("→ KANAN TAJAM");     break;
    case 'Q': rotasiKiri();  Serial.println("→ ROTASI KIRI");     break;
    case 'E': rotasiKanan(); Serial.println("→ ROTASI KANAN");    break;
    case 'S': stopAll();     Serial.println("→ STOP");            break;
    default:  stopAll();                                           break;
  }
}

// --- SETUP ---
void setup() {
  pinMode(FL_RPWM, OUTPUT); pinMode(FL_LPWM, OUTPUT);
  pinMode(FR_RPWM, OUTPUT); pinMode(FR_LPWM, OUTPUT);
  pinMode(RL_RPWM, OUTPUT); pinMode(RL_LPWM, OUTPUT);
  pinMode(RR_RPWM, OUTPUT); pinMode(RR_LPWM, OUTPUT);

  Serial.begin(9600);
  Serial2.begin(9600);

  stopAll();
  Serial.println("=== MEGA READY ===");
}

// --- LOOP ---
void loop() {
  if (Serial2.available()) {
    char cmd = Serial2.read();
    Serial.print("CMD ESP32: "); Serial.println(cmd);
    jalankanCmd(cmd);
  }

  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd != '\n' && cmd != '\r') {
      Serial.print("CMD SERIAL: "); Serial.println(cmd);
      jalankanCmd(cmd);
    }
  }
}