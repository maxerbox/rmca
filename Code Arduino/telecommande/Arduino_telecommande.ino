#include <VirtualWire.h>

 int joyPin1 = 0;                 // Definition de l'axe x du jostick
 int joyPin2 = 1;                 //  Definition de l'axe y du jostick
 int x = 0;                  //initialisation de la variable de l'axe X
 int y = 0;                  //initialisation de la variable de l'axe Y
 float valeur = 0;          //initialisation de la valeur de commande

 void setup() {
   
    Serial.begin(9600); // definition du baudrate du serial
    vw_setup(500); // initialisation de la librarie virtual wire
  
 }


 void loop() {
  // lecture des entre du joystick
  x = analogRead(joyPin1);            
  y = analogRead(joyPin2); 
  valeur = 0; //definition de la valeur de commande a 0
 
  delay(200); //Attente pour eviter de surcharge l'emetteur en envoie
  
 //Association des valeur du joystick avec la valeur de commande
  //Pour les donnee pour l'axe X (avant, arriere)
  if (x < 500){
      Serial.println("avancer");
      valeur = 1;
    }else if( x > 510){
       Serial.println("reculer");
       valeur = 2;
      }else{
       }
    //Pour les donnee pour l'axe y (droite, gauche)
  if (y < 500){
      Serial.println("gauche ");
      valeur = 3;
    }else if( y > 510){
       Serial.println("droite");
       valeur = 4;
      }else{
       }   
  
  vw_send((byte *) &valeur, sizeof(valeur)); // On envoie le message
  vw_wait_tx(); // On attend la fin de l'envoi
    
 }
