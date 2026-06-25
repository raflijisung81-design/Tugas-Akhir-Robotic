/*************************************************
 * ESP32 FINAL
 * LINE FOLLOWER + TCS3200 + BLYNK
 *************************************************/

#define BLYNK_TEMPLATE_ID   "TMPL6Aufh5uZD"
#define BLYNK_TEMPLATE_NAME "Robot Penyaji Makanan"
#define BLYNK_AUTH_TOKEN    "n9cpgKx-5GRY8F1W-SYgKwHVbk-MOWwf"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HardwareSerial.h>

/******** WIFI ********/
char ssid[] = "Infinix NOTE 12 2023";
char pass[] = "12345678";

/******** CD4051 ********/
#define SEL_A 25
#define SEL_B 26
#define SEL_C 27
#define LINE_OUT 34

/******** TCS3200 ********/
#define S0 18
#define S1 19
#define S2 21
#define S3 22
#define TCS_OUT 23

/******** SERIAL KE MEGA ********/
#define RXD2 16
#define TXD2 4

HardwareSerial ARD(2);

/******** THRESHOLD SENSOR GARIS ********/
int THR[8] =
{
   962,
  1840,
  2098,
  2378,
  2483,
  1890,
  1776,
  2082
};

int data_sensor[8];

char lastCmd = 0;

/******** TARGET WARNA ********/
volatile int targetWarna = 0;
bool targetTercapai = false;

/*************************************************
 * BLYNK
 *************************************************/
BLYNK_WRITE(V1)
{
  targetWarna = param.asInt();

  if(targetWarna != 0)
    targetTercapai = false;

  Serial.print("Target Warna = ");
  Serial.println(targetWarna);
}

/*************************************************
 * KIRIM KE MEGA
 *************************************************/
void kirim(char cmd)
{
  if(cmd == lastCmd) return;

  lastCmd = cmd;

  ARD.write(cmd);

  Serial.print("CMD = ");
  Serial.println(cmd);
}

/*************************************************
 * BACA CD4051
 *************************************************/
void sensing()
{
  digitalWrite(SEL_A, LOW);
  digitalWrite(SEL_B, LOW);
  digitalWrite(SEL_C, LOW);
  delayMicroseconds(50);
  data_sensor[0] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, HIGH);
  digitalWrite(SEL_B, LOW);
  digitalWrite(SEL_C, LOW);
  delayMicroseconds(50);
  data_sensor[1] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, LOW);
  digitalWrite(SEL_B, HIGH);
  digitalWrite(SEL_C, LOW);
  delayMicroseconds(50);
  data_sensor[2] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, HIGH);
  digitalWrite(SEL_B, HIGH);
  digitalWrite(SEL_C, LOW);
  delayMicroseconds(50);
  data_sensor[3] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, LOW);
  digitalWrite(SEL_B, LOW);
  digitalWrite(SEL_C, HIGH);
  delayMicroseconds(50);
  data_sensor[4] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, HIGH);
  digitalWrite(SEL_B, LOW);
  digitalWrite(SEL_C, HIGH);
  delayMicroseconds(50);
  data_sensor[5] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, LOW);
  digitalWrite(SEL_B, HIGH);
  digitalWrite(SEL_C, HIGH);
  delayMicroseconds(50);
  data_sensor[6] = analogRead(LINE_OUT);

  digitalWrite(SEL_A, HIGH);
  digitalWrite(SEL_B, HIGH);
  digitalWrite(SEL_C, HIGH);
  delayMicroseconds(50);
  data_sensor[7] = analogRead(LINE_OUT);
}

/*************************************************
 * POSISI GARIS
 *************************************************/
int posisiGaris(bool s[])
{
  int bobot[8] =
  {
    -350,
    -250,
    -150,
    -50,
     50,
     150,
     250,
     350
  };

  long jumlah = 0;
  int aktif = 0;

  for(int i=0;i<8;i++)
  {
    if(s[i])
    {
      jumlah += bobot[i];
      aktif++;
    }
  }

  if(aktif == 0)
    return 9999;

  return jumlah / aktif;
}

/*************************************************
 * TCS3200
 *************************************************/
int readRed()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  return pulseIn(TCS_OUT, LOW);
}

int readGreen()
{
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  return pulseIn(TCS_OUT, LOW);
}

int readBlue()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  return pulseIn(TCS_OUT, LOW);
}

String deteksiWarna()
{
  int R = readRed();
  int G = readGreen();
  int B = readBlue();

  Serial.print("R=");
  Serial.print(R);
  Serial.print(" G=");
  Serial.print(G);
  Serial.print(" B=");
  Serial.println(B);

  if(R > G && G > B)
    return "BIRU";

  if(G > B && B > R)
    return "MERAH";

  if(B > R && R > G)
    return "HIJAU";

  if(B > G && G > R)
    return "KUNING";

  return "UNKNOWN";
}

void cekTargetWarna()
{
  if(targetWarna == 0)
    return;

  String warna = deteksiWarna();

  Serial.print("Target = ");

  switch(targetWarna)
  {
    case 1: Serial.print("MERAH"); break;
    case 2: Serial.print("HIJAU"); break;
    case 3: Serial.print("BIRU"); break;
    case 4: Serial.print("KUNING"); break;
  }

  Serial.print(" | Terbaca = ");
  Serial.println(warna);

  if(targetWarna == 1 && warna == "MERAH")
  {
    Serial.println("TARGET MERAH DITEMUKAN");
    kirim('S');
    targetTercapai = true;
  }

  else if(targetWarna == 2 && warna == "HIJAU")
  {
    Serial.println("TARGET HIJAU DITEMUKAN");
    kirim('S');
    targetTercapai = true;
  }

  else if(targetWarna == 3 && warna == "BIRU")
  {
    Serial.println("TARGET BIRU DITEMUKAN");
    kirim('S');
    targetTercapai = true;
  }

  else if(targetWarna == 4 && warna == "KUNING")
  {
    Serial.println("TARGET KUNING DITEMUKAN");
    kirim('S');
    targetTercapai = true;
  }
}

/*************************************************
 * SETUP
 *************************************************/
void setup()
{
  Serial.begin(115200);

  ARD.begin(9600, SERIAL_8N1, RXD2, TXD2);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  pinMode(SEL_A, OUTPUT);
  pinMode(SEL_B, OUTPUT);
  pinMode(SEL_C, OUTPUT);

  pinMode(LINE_OUT, INPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(TCS_OUT, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("ROBOT READY");
}

/*************************************************
 * LOOP
 *************************************************/
void loop()
{
  Blynk.run();

  if(targetTercapai)
  {
    kirim('S');
    return;
  }

  cekTargetWarna();

  sensing();

  bool s[8];
  int hitam = 0;

  for(int i=0;i<8;i++)
  {
    s[i] = (data_sensor[i] > THR[i]);

    if(s[i]) hitam++;
  }

  if(hitam == 0)
  {
    kirim('S');
    return;
  }

  if(hitam >= 6)
  {
    kirim('F');
    return;
  }

  int pos = posisiGaris(s);

  if(pos < -250)
       kirim('Q');
  else if(pos < -120)
       kirim('C');
  else if(pos < -40)
       kirim('A');
  else if(pos <= 40)
       kirim('F');
  else if(pos <= 120)
       kirim('B');
  else if(pos <= 250)
       kirim('D');
  else
       kirim('E');

  delay(10);
}