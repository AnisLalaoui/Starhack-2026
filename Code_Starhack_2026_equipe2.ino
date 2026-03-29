#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Stepper.h>

RTC_DS1307 rtc;

// =========================
// LCD
// =========================
LiquidCrystal lcd(2, 12, 13, A0, A1, A2);

// =========================
// PINS
// =========================
const int BUZZER_PIN = 3;

// Clavier partiel
const int KEYPAD_ROW4_PIN = 4;   // fil 4 du clavier
const int KEYPAD_STAR_PIN = 5;   // fil 5 du clavier = colonne de *
const int KEYPAD_HASH_PIN = 6;   // fil 7 du clavier = colonne de #

const int SERVO_PIN = 7;
const int LED_RED_PIN = A3;

// =========================
// SERVO TRAPPE
// =========================
Servo trappeServo;
const int ANGLE_FERME = 0;
const int ANGLE_OUVERT = 90;

// =========================
// STEPPER ULN2003
// =========================
const int stepsPerRevolution = 2048;
const int stepsFor90Deg = 524;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

// =========================
// HORAIRES
// =========================
struct Rappel {
  int heure;
  int minute;
};

Rappel rappelsMedicaments[] = {
  //{heure, minutes},
  {9, 47},
  {9, 48},
  {9,49},
  {9,50}
};

const int NB_RAPPELS = sizeof(rappelsMedicaments) / sizeof(rappelsMedicaments[0]);
Rappel rappelRemplissage = {21, 37};

// Evite plusieurs déclenchements dans la même minute
int lastTriggeredHour = -1;
int lastTriggeredMinute = -1;

// =========================
// ETAT SYSTEME
// =========================
bool trappeOuverte = false;
bool systemeActif = true;
bool oldHashPressed = false;
bool lcdAllume = true;

// =========================
// OUTILS
// =========================
void ledOn() {
  digitalWrite(LED_RED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_RED_PIN, LOW);
}

void toutOff() {
  noTone(BUZZER_PIN);
  ledOff();
}

bool estHeureMedicament(int h, int m) {
  for (int i = 0; i < NB_RAPPELS; i++) {
    if (rappelsMedicaments[i].heure == h && rappelsMedicaments[i].minute == m) {
      return true;
    }
  }
  return false;
}

// =========================
// LCD
// =========================
void ecranOn() {
  if (!lcdAllume) {
    lcd.display();
    lcdAllume = true;
  }
}

void ecranOff() {
  if (lcdAllume) {
    lcd.noDisplay();
    lcdAllume = false;
  }
}

void afficherHeure(DateTime now) {
  if (!lcdAllume) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Heure: ");

  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');

  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');

  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  lcd.setCursor(0, 1);
  if (systemeActif) {
    lcd.print("MedBot ON");
  } else {
    lcd.print("MedBot OFF");
  }
}

void afficherMessage(const char* l1, const char* l2) {
  if (!lcdAllume) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

// =========================
// TRAPPE
// =========================
void ouvrirTrappe() {
  if (trappeOuverte) return;

  for (int pos = ANGLE_FERME; pos <= ANGLE_OUVERT; pos += 1) {
    trappeServo.write(pos);
    delay(30);
  }

  trappeOuverte = true;
}

void fermerTrappe() {
  if (!trappeOuverte) return;

  for (int pos = ANGLE_OUVERT; pos >= ANGLE_FERME; pos -= 2) {
    trappeServo.write(pos);
    delay(15);
  }

  trappeOuverte = false;
}

// =========================
// MOTEUR PAS A PAS
// =========================
void tournerPlateau90Deg() {
  afficherMessage("Distribution", "En cours...");
  myStepper.step(stepsFor90Deg);
  delay(500);
}

// =========================
// LECTURE STABLE DES TOUCHES
// =========================
bool toucheStable(int colPin) {
  digitalWrite(KEYPAD_ROW4_PIN, LOW);
  delay(3);

  bool lecture1 = (digitalRead(colPin) == LOW);
  delay(15);
  bool lecture2 = (digitalRead(colPin) == LOW);

  digitalWrite(KEYPAD_ROW4_PIN, HIGH);

  return lecture1 && lecture2;
}

bool etoileAppuyee() {
  if (toucheStable(KEYPAD_STAR_PIN)) {
    delay(120);
    return true;
  }
  return false;
}

bool diezAppuye() {
  if (toucheStable(KEYPAD_HASH_PIN)) {
    delay(120);
    return true;
  }
  return false;
}

void verifierToggleSysteme() {
  bool hashPressed = diezAppuye();

  if (hashPressed && !oldHashPressed) {
    systemeActif = !systemeActif;
    toutOff();

    if (!systemeActif) {
      fermerTrappe();
      ecranOff();
    } else {
      ecranOn();
      afficherMessage("Systeme ON", "Pret");
      delay(1000);
    }

    delay(250);
  }

  oldHashPressed = hashPressed;
}

// =========================
// SONS / ALERTES
// =========================
void alarmeDouce() {
  ledOn();
  tone(BUZZER_PIN, 900);

  for (int i = 0; i < 20; i++) {
    if (etoileAppuyee()) {
      noTone(BUZZER_PIN);
      ledOff();
      return;
    }
    delay(10);
  }

  noTone(BUZZER_PIN);
  ledOff();

  for (int i = 0; i < 28; i++) {
    if (etoileAppuyee()) return;
    delay(10);
  }
}

void alarmeMoyenne() {
  ledOn();
  tone(BUZZER_PIN, 1200);

  for (int i = 0; i < 16; i++) {
    if (etoileAppuyee()) {
      noTone(BUZZER_PIN);
      ledOff();
      return;
    }
    delay(10);
  }

  noTone(BUZZER_PIN);
  ledOff();

  for (int i = 0; i < 12; i++) {
    if (etoileAppuyee()) return;
    delay(10);
  }
}

void alarmeUrgente() {
  ledOn();
  tone(BUZZER_PIN, 1600);

  for (int i = 0; i < 10; i++) {
    if (etoileAppuyee()) {
      noTone(BUZZER_PIN);
      ledOff();
      return;
    }
    delay(10);
  }

  noTone(BUZZER_PIN);
  ledOff();

  for (int i = 0; i < 7; i++) {
    if (etoileAppuyee()) return;
    delay(10);
  }
}

// =========================
// MELODIE REMPLISSAGE
// =========================
bool jouerMelodieRemplissageInterruptible() {
  int notes[] = {523, 659, 784, 659};
  int durees[] = {120, 120, 160, 160};

  for (int i = 0; i < 4; i++) {
    if (etoileAppuyee()) {
      toutOff();
      return true;
    }

    ledOn();
    tone(BUZZER_PIN, notes[i]);

    unsigned long t0 = millis();
    while (millis() - t0 < (unsigned long)durees[i]) {
      if (etoileAppuyee()) {
        toutOff();
        return true;
      }
      delay(5);
    }

    noTone(BUZZER_PIN);
    ledOff();

    t0 = millis();
    while (millis() - t0 < 40UL) {
      if (etoileAppuyee()) {
        toutOff();
        return true;
      }
      delay(5);
    }
  }

  return false;
}

// =========================
// ALARME MEDICAMENT
// =========================
void lancerAlarmeMedicament() {
  unsigned long debut = millis();
  bool confirme = false;

  ecranOn();
  afficherMessage("Medicament", "Trappe ouverte");
  ouvrirTrappe();

  while (!confirme) {
    verifierToggleSysteme();
    if (!systemeActif) {
      toutOff();
      return;
    }

    if (etoileAppuyee()) {
      confirme = true;
      break;
    }

    unsigned long tempsEcoule = millis() - debut;

    if (tempsEcoule < 20000UL) {
      afficherMessage("Premier rappel", "* pour arreter");
      alarmeDouce();
    }
    else if (tempsEcoule < 40000UL) {
      afficherMessage("Deuxieme rappel", "* pour arreter");
      alarmeMoyenne();
    }
    else {
      afficherMessage("Dernier rappel", "* pour arreter");
      alarmeUrgente();
    }
  }

  toutOff();
  afficherMessage("Confirmation OK", "");
  delay(5000);

  afficherMessage("Fermeture...", "");
  fermerTrappe();
  delay(3000);

  tournerPlateau90Deg();

  afficherMessage("Pilule suivante", "Prete");
  delay(1500);
}

// =========================
// RAPPEL REMPLISSAGE
// =========================
void lancerRappelRemplissage() {
  bool confirme = false;

  ecranOn();

  while (!confirme) {
    verifierToggleSysteme();
    if (!systemeActif) {
      toutOff();
      return;
    }

    afficherMessage("Rappel remplissage", "* quand complete");

    if (jouerMelodieRemplissageInterruptible()) {
      confirme = true;
      break;
    }

    delay(120);
  }

  toutOff();
  afficherMessage("Stock rempli", "A demain");
  delay(2000);
}

// =========================
// SETUP
// =========================
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  pinMode(KEYPAD_ROW4_PIN, OUTPUT);
  digitalWrite(KEYPAD_ROW4_PIN, HIGH);

  pinMode(KEYPAD_STAR_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_HASH_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  Wire.begin();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.display();
  lcdAllume = true;

  trappeServo.attach(SERVO_PIN);
  trappeServo.write(ANGLE_FERME);
  trappeOuverte = false;

  myStepper.setSpeed(8);

  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC non detecte");
    Serial.println("Erreur: DS1307 non detecte");
    while (1);
  }

  // Enleve les // UNE seule fois pour regler l'heure
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (!rtc.isrunning()) {
    Serial.println("RTC arrete, reglage auto");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  toutOff();
  afficherMessage("Systeme", "pret");
  delay(1500);
}

// =========================
// LOOP
// =========================
void loop() {
  verifierToggleSysteme();

  DateTime now = rtc.now();

  int h = now.hour();
  int m = now.minute();
  int s = now.second();

  Serial.print("Heure: ");
  if (h < 10) Serial.print('0');
  Serial.print(h);
  Serial.print(':');
  if (m < 10) Serial.print('0');
  Serial.print(m);
  Serial.print(':');
  if (s < 10) Serial.print('0');
  Serial.println(s);

  if (systemeActif) {
    afficherHeure(now);
  }

  if (!systemeActif) {
    delay(200);
    return;
  }

  if (h == lastTriggeredHour && m == lastTriggeredMinute) {
    delay(250);
    return;
  }

  if (estHeureMedicament(h, m)) {
    lastTriggeredHour = h;
    lastTriggeredMinute = m;
    lancerAlarmeMedicament();
  }

  if (h == rappelRemplissage.heure && m == rappelRemplissage.minute) {
    lastTriggeredHour = h;
    lastTriggeredMinute = m;
    lancerRappelRemplissage();
  }

  delay(250);
}
