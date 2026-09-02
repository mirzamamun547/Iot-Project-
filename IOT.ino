/*******************************************************
 Smart Alcohol & Accident Detection System 
 Roll 2207091  2207107
*******************************************************/

#define BLYNK_TEMPLATE_ID "TMPL63yrm_NjQ"
#define BLYNK_TEMPLATE_NAME "Accident Detection"
#define BLYNK_AUTH_TOKEN "judbo_NAI2zKTmsAhCOkndHwFFPG6rFY"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>
#include <HardwareSerial.h>


char ssid[] = "Student's LAB";
char pass[] = "kuet@cse321";

#define MQ3_PIN 36
#define ALERT_LED_PIN 13
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

const int rs = 12, en = 14, d4 = 27, d5 = 26, d6 = 25, d7 = 33;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Adafruit_MPU6050 mpu;

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

BlynkTimer timer;

int alcoholValue = 0;
const int alcoholThreshold = 1700;
bool alcoholDetected = false;

bool crashDetected = false;

float oldX = 0;
float oldY = 0;
float oldZ = 0;

float deltaX = 0;
float deltaY = 0;
float deltaZ = 0;

float magnitude = 0;
double impactAngle = 0;

float sensitivity = 15.0;

int vibration = 0;
const int devibrate = 5;

bool gpsFix = false;
double gpsLatitude = 0;
double gpsLongitude = 0;
double gpsSpeed = 0;

void readMQ3()
{
  long total = 0;

  for (int i = 0; i < 10; i++)
  {
    total += analogRead(MQ3_PIN);
    delay(5);
  }

  alcoholValue = total / 10;
  alcoholDetected = (alcoholValue > alcoholThreshold);
}

void readGPS()
{
  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid())
  {
    gpsFix = true;

    gpsLatitude = gps.location.lat();
    gpsLongitude = gps.location.lng();

    if (gps.speed.isValid())
      gpsSpeed = gps.speed.kmph();
  }
  else
  {
    gpsFix = false;
  }
}

void updateCrashDetection()
{
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  if (vibration > 0)
    vibration--;

  deltaX = a.acceleration.x - oldX;
  deltaY = a.acceleration.y - oldY;
  deltaZ = a.acceleration.z - oldZ;

  magnitude = sqrt(
    deltaX * deltaX +
    deltaY * deltaY +
    deltaZ * deltaZ
  );

  oldX = a.acceleration.x;
  oldY = a.acceleration.y;
  oldZ = a.acceleration.z;

  if (magnitude >= sensitivity && vibration == 0)
  {
    crashDetected = true;
    vibration = devibrate;

    double xRatio = deltaX / (magnitude == 0 ? 1 : magnitude);
    double yRatio = deltaY / (magnitude == 0 ? 1 : magnitude);

    xRatio = constrain(xRatio, -1.0, 1.0);
    yRatio = constrain(yRatio, -1.0, 1.0);

    double arcX = acos(xRatio);
    double arcY = acos(yRatio);

    impactAngle =
      (atan2(arcY, arcX) * 180.0 / M_PI) + 180.0;

    Serial.println("***** CRASH DETECTED *****");
    Serial.print("Magnitude: ");
    Serial.println(magnitude);
    Serial.print("Impact Angle: ");
    Serial.println(impactAngle);
  }
  else if (vibration == 0)
  {
    crashDetected = false;
  }
}

void showAccidentScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("ACCIDENT ALERT! ");

  lcd.setCursor(0, 1);

  if (gpsFix)
  {
    lcd.print("GPS ");
    lcd.print(gpsLatitude, 2);
  }
  else
  {
    lcd.print("GPS: NO FIX     ");
  }
}

void showAlcoholScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("ALCOHOL HIGH!   ");

  lcd.setCursor(0, 1);

  if (gpsFix)
  {
    lcd.print("GPS ");
    lcd.print(gpsLatitude, 2);
  }
  else
  {
    lcd.print("GPS: NO FIX     ");
  }
}

void showGPSScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("GPS SEARCHING   ");

  lcd.setCursor(0, 1);
  lcd.print("WAIT FOR FIX    ");
}

void showSafeScreen()
{
  lcd.setCursor(0, 0);

  lcd.print("Alcohol:");
  lcd.print(alcoholValue);
  lcd.print("    ");

  lcd.setCursor(0, 1);

  if (gpsFix)
  {
    lcd.print("GPS ");
    lcd.print(gpsSpeed, 1);
    lcd.print("Km/h ");
  }
  else
  {
    lcd.print("GPS WAIT        ");
  }
}

void updateLCD()
{
  static int lastScreen = -1;

  int screen = 0;

  if (crashDetected || vibration > 0)
    screen = 1;
  else if (alcoholDetected)
    screen = 2;
  else if (!gpsFix)
    screen = 3;

  if (screen != lastScreen)
  {
    lcd.clear();
    lastScreen = screen;
  }

  switch (screen)
  {
    case 1:
      digitalWrite(ALERT_LED_PIN, HIGH);
      showAccidentScreen();
      break;

    case 2:
      digitalWrite(ALERT_LED_PIN, HIGH);
      showAlcoholScreen();
      break;

    case 3:
      digitalWrite(ALERT_LED_PIN, LOW);
      showGPSScreen();
      break;

    default:
      digitalWrite(ALERT_LED_PIN, LOW);
      showSafeScreen();
      break;
  }
}

void updateBlynk()
{
  if (!Blynk.connected())
    return;

  Blynk.virtualWrite(V0, alcoholValue);
  Blynk.virtualWrite(V1, crashDetected);
  Blynk.virtualWrite(V2, gpsLatitude);
  Blynk.virtualWrite(V3, gpsLongitude);
  Blynk.virtualWrite(V4, gpsSpeed);
  Blynk.virtualWrite(V5, magnitude);
  Blynk.virtualWrite(V6, impactAngle);
}

void sendData()
{
  readMQ3();
  readGPS();
  updateCrashDetection();
  updateLCD();
  updateBlynk();

  Serial.println("--------------------------");

  Serial.print("Alcohol: ");
  Serial.println(alcoholValue);

  Serial.print("Alcohol Detected: ");
  Serial.println(alcoholDetected);

  Serial.print("Crash: ");
  Serial.println(crashDetected);

  Serial.print("Magnitude: ");
  Serial.println(magnitude);

  Serial.print("Impact Angle: ");
  Serial.println(impactAngle);

  Serial.print("GPS Fix: ");
  Serial.println(gpsFix);

  Serial.print("Latitude: ");
  Serial.println(gpsLatitude, 6);

  Serial.print("Longitude: ");
  Serial.println(gpsLongitude, 6);

  Serial.print("Speed: ");
  Serial.print(gpsSpeed, 1);
  Serial.println(" km/h");

  Serial.println("--------------------------");
}

void checkConnection()
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  if (!Blynk.connected())
    Blynk.connect();
}

void setup()
{
  Serial.begin(115200);

  pinMode(ALERT_LED_PIN, OUTPUT);
  digitalWrite(ALERT_LED_PIN, LOW);

  lcd.begin(16, 2);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART ACCIDENT");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM BOOTING");

  delay(2000);

  lcd.clear();

  Wire.begin(21, 22);

  if (!mpu.begin())
  {
    Serial.println("MPU6050 ERROR!");

    lcd.setCursor(0, 0);
    lcd.print("MPU6050 ERROR!");

    lcd.setCursor(0, 1);
    lcd.print("CHECK SENSOR");

    delay(3000);
  }
  else
  {
    Serial.println("MPU6050 OK!");

    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  oldX = a.acceleration.x;
  oldY = a.acceleration.y;
  oldZ = a.acceleration.z;

  gpsSerial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX_PIN,
    GPS_TX_PIN
  );

  lcd.clear();
  lcd.print("CONNECTING WIFI");

  Blynk.begin(
    BLYNK_AUTH_TOKEN,
    ssid,
    pass
  );

  timer.setInterval(500L, sendData);

  lcd.clear();
  lcd.print("SYSTEM READY");

  Serial.println("SYSTEM READY!");

  delay(1000);

  lcd.clear();
}

void loop()
{
  Blynk.run();
  timer.run();
  checkConnection();
}
