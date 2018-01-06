/*
 ____ ____ ____ ____ 
||R |||M |||C |||A ||
||__|||__|||__|||__||
|/__\|/__\|/__\|/__\|

*/
#include <SoftwareSerial.h> //Partie Bluetooth, pour la communicaion avec l'emetteur/recepteur bluetooth, supervisé par Simon
#include <ArduinoJson.h> //Partie Bluetooth, pour structurer les données envoyées, supervisé par Simon https://arduinojson.org/
#include <VirtualWire.h> //Partie Reception télécommande, pour recevoir les commandes, Supervisé par Léo
#include "Ultrasonic.h"  //Partie Detection de la distance, librarie pour utiliser le module à ultrason, Supervisé par Lucas

#define sensorPin 2 //Broche d'entrée du capteur de température, Analog2, (CAN)
#define DEBUG 0 // Si à 0, n'envoit par usb au moniteur série, si à , envoit les mesures au moniteur série, sur l'arduino IDE
#define rateLimitMs 1000 // Limite la cadence d'envoit des mesures à l'ordinateur par Bluetooth, 1000mS = 1s, envoit les données toutes les 1 secondes

const int broche_rx = 7; // broche utilisée pour recevoir les commandes par la télécommande
const int broche_sig = 5; // broche utilisée par le capteur à ultrason pour mésurée la distance, et le temps de latence entre une pulsation
long RangeInCentimeters; // Variable utilisé pour conserver la distance mesurée par le capteur à ultrason
SoftwareSerial hc06(2, 6); //TX RX,  Déclare une nouvelle instance SoftwareSerial, sur les broches TX: 2, RX:6, utilisé pour communiquer avec le module bluetooth
Ultrasonic ultrasonic(5); // Déclare une nouvelle instance Ultrasonic, utilisé pour communiquer avec le capteur à ultrason et mesurer la distance sur la broche 5.
unsigned long lastSendTime[3]; // Déclare un nouveau tableau vide composé de 3 case stockant un nombre  qui doivent être entier et positifs. Ce tableau est utilisé pour comparer le temps d'envoit d'un certain type de donné (temperature, direction, battery) et donc de limiter l'envoit des mesures par bluetooth  à 1 mesure toute les 1 secondes
int distance; //Sert à conserver la distance en cm globalement, mesurée par le capteur à ultrason.
int commandeReceived = 0; //Sert à conserver la commande recu par le recepteur 433mhz et diriger le robot. Si 0: ne rien faire, 1: avance, 2: reculer, 3: tourner vers la gauche, 4: tourner vers la droite

void setup() {
  Serial.begin(9600); // Initialiser la liaison avec une vitesse de transmission de 9600 bits par secondes, utilisé pour communiquer avec le moniteur serie du Arduino IDE

  Serial.println("power on"); // On envoit au moniteur série "power on", pour montrer que le robot à démarrer

  //initialisation moteur

  // moteur 1, droit
  pinMode(12, OUTPUT); // Sens de rotation 1, on paramètre en mode sortie la broche 12, utilisée pour changer de sens de rotation le moteur 1
  pinMode(9, OUTPUT); //frein du moteur 1, on paramètre en mode sortie la broche 9, utilisée pour freiner le moteur 1

  //moteur 2, gauche
  pinMode(13, OUTPUT); //Sens de rotation 2,  on paramètre en mode sortie la broche 13, utilisée pour changer de sens de rotation le moteur 2
  pinMode(8, OUTPUT);  // frein du moteur 2, on paramètre en mode sortie la broche 8, utilisée pour freiner le moteur 2

  //desactive les freins
  digitalWrite(9, LOW); // on desactive les freins pour le moteur 1 en envoyant 0 a la broche 9
  digitalWrite(8, LOW); // on desactive les freins pour le moteur 2 en envoyant 0 a la broche 8
  setupReceiver(); // On appelle la fonction setupReceiver() pour initialiser le recepteur pour la télécommande
  setupBluetooth(); // On appelle la fonction setupBluetooth(), pour initialiser le bluetooth
}
void setupReceiver() { // setupReceiver() pour initialiser le recepteur pour la telecommande

  vw_set_rx_pin(broche_rx); //On configurer la broche de reception (broche 7) pour recevoir les commandes depuis la télecommande, cette broche est relier au OUT du module Recepteur 433Mhz
  vw_setup(500); // Commencer la liaison avec une vitesse de reception de 500 bits par secondes, utilise pour recevoir les commandes depuis la telecommandes
  vw_rx_start(); // On commence à ecouter sur la broche pour recevoir les commandes
}
void setupBluetooth() { // setupBluetooth(), pour initialiser le bluetooth
  hc06.begin(9600); // Initialiser la liaison avec une vitesse de transmission de 9600 bits par secondes, utilisé pour communiquer avec le module bluetooth en liaison UART et donc envoyer les mesures à l'odinateur
  hc06.print("AT+NAMERMCA"); // On envoit au module bluetooth le message "AT+NAMERMCA", ce message est une commande pour le module bluetooth (tout message qui commence par AT est une commande pour configurer le module), cette commande est NAME, elle configure le nom afficher au autres périphériques bluetooth, ici "RMCA". Il ya d'autre type de commande, comme PIN, VERSION etc...
}
int UltraSonicSensor() { // Fonction pour mesurer la distance avec le capteur à ultrason

  RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // Mesure la distance et la converse dans la variable RangeInCentimeters en cm.
  if (DEBUG) { // Si DEBUG = 1
    Serial.print(RangeInCentimeters);//0~400cm, On envoit la distance au moniteur serie Arduino IDE
    Serial.println(" cm"); // On envoit ensuite " cm" pour fait un retour à la ligne.
  }

  return RangeInCentimeters; // On retourne la valeur en cm qui à été mesurée
}
void sendInfo(String key, String data) { // Fontion utilisée pour envoyer les mesures par bluetooth. 2 paramètre: ((String)key: le type de mesure (temperature, battery, direction), (String)data: la mesure à envoyer) 

  int placeArray = 0; // Cette variable est un nombre entier utiliser pour determiner dans quelle index du tableau lastSendTime il faut conserver le temps en ms quand la mesure a été envoyé, permet de limiter l'envoit des mesures par bluetooth  à 1 mesure toute les 1 secondes
  if (key == "temperature") // si le type de donnée est égal à "température", l'emplacement dans le tableau lastSendTime sera à 0
    placeArray = 0;
  else if (key == "battery") // si le type de donnée est égal à "battery", l'emplacement dans le tableau lastSendTime sera à 1
    placeArray = 1;
  else if (key == "direction") // si le type de donnée est égal à "direction", l'emplacement dans le tableau lastSendTime sera à 2
    placeArray = 2;
  if ( millis() - lastSendTime[placeArray] > rateLimitMs) { // Si le temps en ms depuis le démarrage du programme retourné par la fonction millis() moins le dernier temps d'envoit pour un certain type de donné est supérieur au dernier à 1000ms, alors :
    StaticJsonBuffer<200> jsonBuffer; // On créer une nouvelle mémoire tampon de taille 200 caractères pour structurer en JSON les mesures envoyées. Ca permet de stocker une valeur fixée de caractères dans la mémoire, et donc de fonctionner avec des appareils possendant peut de mémoire RAM
    JsonObject& root = jsonBuffer.createObject(); // On créer un nouveau objet JSON, utilisé pour structurer les mesures
    root["type"] = key; // On ajouter la clef type, pour le type de mesure, et on lui affecte la variable key
    root["data"] = data; // On ajouter la clef data, pour la valeur de la mesure, et on lui affecte la variable data
    root.printTo(hc06); // On envoit l'objet root en texte au module bluetooth par liaison UART et donc à l'ordinateur
    lastSendTime[placeArray] = lastSendTime[placeArray] + rateLimitMs; // Afin de limiter l'envoit de donné, on affecte au tableau lastSendTime à l'index du type de donné: le dernier temps d'envoit + 1000ms (variable rateLimitMs), afin de limiter à 1 mesure par seconde.
  }
}

float readTemp() { // Fonction utilisée pour mesurer la température
  int reading = analogRead(sensorPin); // On lit nombre à la sortie du CAN relier au capteur de témpérature            
                                                
/*                  V
                      pe
       V   =  N x  ----------
        e             n
                     2 -1
*/
  sendInfo("direction", String(reading)); // On envoit par bluetooth avec la fonction sendInfo() la mesure du nombre en sortie du CAN afin de l'afficher sur l'ordinateur
  // On multiplie par la tension max qui peut être convertit, ici 5v
  float voltage = reading * 5.0;
  // On divise par 2^(10)^= 1024, 10: nombre de bit du CAN sur l'arduino Uno. On obtient donc le voltage
  voltage /= 1024.0;
  
  sendInfo("battery", String(voltage)); // On envoit par bluetooth avec la fonction sendInfo() la mesure du voltage obtenu à partir du calcul afin de l'afficher sur l'ordinateur
  
  float temperatureC = (voltage * 44.444) - 61.111; //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  if (DEBUG) { // Si debug = 1
    Serial.print(temperatureC); // Envoyer la température au moniteur série Arduino IDE
    Serial.println(" degrees C"); // Envoyer l'unité et inserer une nouvelle au moniteur série Arduino IDE
  }
  return temperatureC; // On retourne la valeur en °C qui à été mesurée
}
void interpreterCommande(int valeur) { // Fonction utilisé pour commander les moteurs en fonction d'une commande. 1 Paramètre: (int) valeur: la valeur de la commande (0,1,2,3,4)

  if (valeur == 1 && distance > 10) { // Si la commande (valeur) est égale à 1 et la distance est supérieur à 10cm,  avancer
    digitalWrite(12, LOW); // Sens de rotation du moteur 1 en position avancer

    analogWrite(3, 255); // Envoi 0xFF sur la broche 3, utilisé pour moduler la vitesse du moteur 1. 255=vitesse maximal

    digitalWrite(13, LOW); // Sens de rotation du moteur 2 en position avancer

    analogWrite(11, 255); // Envoi 0xFF sur la broche 11, utilisé pour moduler la vitesse du moteur 2. 255=vitesse maximal
  } else if (valeur == 2) { // Si la commande (valeur) est égale à 2, reculer

    digitalWrite(12, HIGH); // Sens de rotation du moteur 1 en position reculer
    analogWrite(3, 255); // Envoi 0xFF sur la broche 3, utilisé pour moduler la vitesse du moteur 1. 255=vitesse maximal

    digitalWrite(13, HIGH); // Sens de rotation du moteur 2 en position reculer
    analogWrite(11, 255); // Envoi 0xFF sur la broche 11, utilisé pour moduler la vitesse du moteur 1. 255=vitesse maximal

  } else if (valeur == 3) { // Si la commande (valeur) est égale à 3, tourner à gauche

    digitalWrite(12, HIGH); // Sens de rotation du moteur 1 en position reculer
    analogWrite(3, 255); // Envoi 0xFF sur la broche 3, utilisé pour moduler la vitesse du moteur 1. 255=vitesse maximal

    digitalWrite(13, LOW); // Sens de rotation du moteur 2 en position avancer
    analogWrite(11, 255);  // Envoi 0xFF sur la broche 11, utilisé pour moduler la vitesse du moteur 2. 255=vitesse maximal

  } else if (valeur == 4) { // Si la commande (valeur) est égale à 4, tourner à droite

    digitalWrite(12, LOW); // Sens de rotation du moteur 2 en position avancer
    analogWrite(3, 255);  // Envoi 0xFF sur la broche 3, utilisé pour moduler la vitesse du moteur 1. 255=vitesse maximal

    digitalWrite(13, HIGH); // Sens de rotation du moteur 1 en position reculer
    analogWrite(11, 255); // Envoi 0xFF sur la broche 11, utilisé pour moduler la vitesse du moteur 2. 255=vitesse maximal

  } else { // Sinon, arreter le robot.
    analogWrite(3, 0); // Envoi 0x0 sur la broche 3, utilisé pour moduler la vitesse du moteur 1. 0=vitesse minimal
    analogWrite(11, 0); // Envoi 0x0 sur la broche 11, utilisé pour moduler la vitesse du moteur 2. 0=vitesse minimal

  }
}
void loop() {
  if (hc06.available())
  { //{"type":"commande", data:1}
    String inData = hc06.readStringUntil('\n');
    Serial.println("data: " + inData);
    StaticJsonBuffer<200> receiveBuffer;
    JsonObject& root = receiveBuffer.parseObject(inData);
    if (root.success()) {
      Serial.println("Sucess parsing");
      const String type = root["type"];
      int valeur = root["data"];
      Serial.println(type);
      if (type == "commande") {
        commandeReceived = valeur;
        Serial.println("Received command:" + String(valeur));
        interpreterCommande(valeur);
      }
    } else {
      Serial.println("Failed to parse object");
    }
  }
  //capteur ultrason
  sendInfo("temperature", String(readTemp()));
  //emmission de l'ultrason
  pinMode(broche_sig, OUTPUT);
  digitalWrite(broche_sig, LOW);
  delayMicroseconds(2);
  digitalWrite(broche_sig, HIGH);
  delayMicroseconds(5);
  digitalWrite(broche_sig, LOW);
  //recepetion de l'ultrason

  distance = UltraSonicSensor();


  byte taille_message = sizeof(float);
  float commandFm;
  // On attend de recevoir un message
  if (vw_get_message((byte *) &commandFm, &taille_message)) {
    // On copie le message

    sendInfo("direction", String(commandFm));
    commandeReceived = (int) commandFm;
    //Commande des moteurs

  }

  interpreterCommande(commandeReceived);
}
