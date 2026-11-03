#include <SparkFun_APDS9960.h>
#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define led 13
#define dist_min 15
SparkFun_APDS9960 apds = SparkFun_APDS9960();
SoftwareSerial mySoftwareSerial(10, 11);  
DFRobotDFPlayerMini myDFPlayer;

bool modulo_pronto = false;
int cor_tocando = 0;  
uint8_t proximity_data = 0;
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mySoftwareSerial.begin(9600);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  Serial.println(("iniciando"));

  
  if (myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(("iniciando módulo de som"));
    myDFPlayer.setTimeOut(5000);
    myDFPlayer.volume(30);
    modulo_pronto= true;
  } else {
    Serial.println(("erro módulo de som"));
  }

  
  if (apds.init()) {
    Serial.println(("iniciando apds"));
  } else {
    Serial.println(("erro apds"));
  }

  
  apds.setAmbientLightGain(AGAIN_1X);
  apds.enableLightSensor(false);

  apds.setProximityGain(PGAIN_1X);
  apds.enableProximitySensor(false);

  Serial.println("tudo certo");
  delay(1000);
}

void loop() {
  if (apds.readProximity(proximity_data)) {

  
    if (proximity_data >= dist_min) { 
      digitalWrite(led, LOW);

      
      apds.readAmbientLight(ambient_light);
      apds.readRedLight(red_light);
      apds.readGreenLight(green_light);
      apds.readBlueLight(blue_light);

      
      float total = (float)red_light + (float)green_light + (float)blue_light;

      if (total > 0) {
        
        float pct_R = (float)red_light / total;
        float pct_G = (float)green_light / total;
        float pct_B = (float)blue_light / total;

        Serial.print(("Prox: "));
        Serial.print(proximity_data);
        Serial.print(("R%: "));
        Serial.println(pct_R * 100);
        Serial.print(("G%: "));
        Serial.println(pct_G * 100);
        Serial.print(("B%: "));
        Serial.println(pct_B * 100);

       
        float margem = 0.10; 

        
        if (pct_R > (pct_G + margem) && pct_R > (pct_B + margem)) {  
          Serial.println(("vermelho"));
          if (modulo_pronto && cor_tocando != 1) {
            myDFPlayer.play(1);
            cor_tocando = 1;
          }
        }
        
        else if (pct_G > (pct_R + margem) && pct_G > (pct_B + margem)) {
          Serial.println(("verde"));
          if (modulo_pronto && cor_tocando!= 2) {
            myDFPlayer.play(2);
           cor_tocando = 2;
          }
        }
        
        else if (pct_B > (pct_R + margem) && pct_B > (pct_G + margem)) {
          Serial.println(("azul"));
          if (modulo_pronto && cor_tocando != 3) {
            myDFPlayer.play(3);
            cor_tocando = 3;
          }
        }
        
        else {
          Serial.println(("cor não identificada-pare"));
          if (modulo_pronto && cor_tocando != 0) {  
            myDFPlayer.stop();
            cor_tocando = 0;
          }
        }
      }

    } else {
      
      digitalWrite(led, HIGH);

      if (modulo_pronto && cor_tocando != 0) {  
        Serial.println(("Objeto removido -> Parando musica."));
        myDFPlayer.stop();
        cor_tocando = 0;
      }
    }
  }

  
  delay(200);
}