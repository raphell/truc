
#include <Wire.h>
#include "rgb_lcd.h"
#include "DHT.h"
#include <math.h>
#include "Digital_Light_TSL2561.h"
#include "Digital_Light_ISL29035.h"

//INDICATION DES PORTS SUR LESQUELS SONT BRANCHEES LES ENTREES/SORTIES
#define PINtemp A1    
#define PINlum A0

#define PINledR 2
#define PINledV 4

#define PINrelai 8
#define PINrelaiChauffe 7


//LCD
rgb_lcd lcd;

// Définit le type de capteur (pour la librairie)
#define DHTTempHum DHT11   // DHT 11 TEMPERATURE & HUMIDITE

//Temperature
DHT dht(PINtemp, DHTTempHum);


int tailleDonnees = 8;    //taille des données envoyées par Processing
char val[8];              //tableau pour stocker les valeurs envoyées par Processing

const int tempEnvoi = 10 ;            //le temps entre 2 envoi (en minute) de valeurs vers Processing
const int nbrIteration = 20 ; //tempEnvoi*60 ;  //le nombre de valeurs qui seront stockée pour ensuite en faire la moyenne (60 prises de valeur pour 1 minutes, la valeur 20 n'est laque pour la présentation)

float moyTemp[nbrIteration] ;     //tableaux pour stocker les valeurs, et ensuite en faire la moyenne
float moyHum[nbrIteration] ;      //
float moyLum[nbrIteration] ;      //
int indiceMoyenne = 0;  //sert d'indice pour savoir quand faire la moyenne et envoyer les valeurs vers Processing

int resTemp ;
int resHum ;
int resLum ;

float diffTemp = 1;
float diffHum = 0;    //différence max entre valeur théorique et valeur réelle
float diffLum = 0;

float humTh = 25;
float tempTh = 20;    //VALEURS THEORIQUES
float lumTh = 50;

float hum = 0;
float temp = 0;
float lum = 0;

bool ledV = false;
bool thPret = false ;
bool diffPret = false ;



////////////////////////////////////////     SETUP      /////////////////////////////////////////////

void setup() {

  indiceMoyenne = 0 ;

  // PORT SERIE
  Serial.begin(9600);

  // LED
  pinMode(PINledR, OUTPUT);
  pinMode(PINledV, OUTPUT);

  // RELAI
   pinMode(PINrelai, OUTPUT);
  
  // ECRAN LCD
  lcd.begin(16, 2);   //indique le nombre de colonne et de ligne de l'écran
  
  //démarrage CAPTEURS
  Wire.begin();
  dht.begin();
  
  //Serial.println("init terminé");     //j'ai mis en commentaire tous les serial.print pour ne pas perturber l'envoi des valeurs des capteurs vers Processing
  lcd.setCursor(0,0) ;
  lcd.print("DEMARRAGE") ;
  lcd.setCursor(0,1) ;
  lcd.print("IMMEDIAT") ;
}



//////////////////////////////////////////////     LOOP      //////////////////////////////////////////////////////////////////////////////

void loop() {
  
  boolean printReception = false;
  int i=0;
  while (Serial.available()) // RECUPERATION DES VALEURS TRANSMISES PAR PROCESSING
  {                          //
    val[i] = Serial.read();  //
    i=i+1;                   //
    printReception = true ;
  }
  
  if (printReception){                 //affichages message des valeurs rentrées
    lcd.clear();
    lcd.setCursor(0,0) ;
    if (val[0]=='t'){
      lcd.print("New val. theo.") ; 
    }
    else if(val[0]=='d'){
      lcd.print("New val. diff.") ;
    }
    
    i=1 ;
    lcd.setCursor(0,1);
    while(i<8){                       //affichage des valeurs rentrées 
      if (i==1){
        lcd.print("T:");
      }
      if (i==3){
        lcd.print(" L:");
      }
      if (i==5){
        lcd.print(" H:");
      }
      lcd.print(val[i]);              //
      i=i+1;                          //
    }
    delay(3000);
    printReception = false ;                  //on convertit les valeurs rentrées pour qu'elles soient réutilisées par le programme arduino
    if(val[0]=='t'){                                  //pour les valeurs théoriques
      tempTh = (val[1]-'0')*10 + val[2]-'0' ;
      humTh = (val[3]-'0')*10 + val[4]-'0' ;
      lumTh = (val[5]-'0')*100 + (val[6]-'0')*10 + val[7]-'0' ;
      lcd.clear();
      lcd.setCursor(0,0) ;
      lcd.print("Valeurs th.") ;
      lcd.setCursor(0,1) ;
      lcd.print("enregistrees") ;
      thPret = true ;
    }
    else if(val[0]=='d'){                              //puis les valeurs de différence
      diffTemp = (val[1]-'0')*10 + val[2]-'0' ;
      diffHum = (val[3]-'0')*10 + val[4]-'0' ;
      diffLum = (val[5]-'0')*100 + (val[6]-'0')*10 + val[7]-'0' ;
      lcd.clear();
      lcd.setCursor(0,0) ;
      lcd.print("Valeurs de diff.") ;
      lcd.setCursor(0,1) ;
      lcd.print("enregistrees") ;
      diffPret = true ;
    }
    delay(2000);
  }
  
  
  ////////////////////////////////////// RECUPERE LES VALEURS DES CAPTEURS
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  lum = analogRead(PINlum); 

  //////////////////////////////////////////////////On fait une MOYENNE
  moyTemp[indiceMoyenne]=temp ;
  moyHum[indiceMoyenne]=hum ;
  moyLum[indiceMoyenne]=lum ;
  indiceMoyenne= indiceMoyenne+1;
  if (indiceMoyenne==nbrIteration) {            //quand le tableau est plein
    for (int k=0 ; k<nbrIteration ; k=k+1){
      resTemp = resTemp + moyTemp[k] ;
      resHum = resHum + moyHum[k] ;
      resLum = resLum + moyLum[k] ;
    }
    resTemp = resTemp/nbrIteration ;
    resHum = resHum/nbrIteration ;
    resLum = resLum/nbrIteration ;
    indiceMoyenne = 0;
    
    lcd.clear();
    lcd.setCursor(0,0) ;
    lcd.print("Envoi des valeurs") ;
    delay(400) ;

    Serial.print(resTemp);
    Serial.print(",");
    Serial.print(resHum);
    Serial.print(",");
    Serial.println(resLum);
    /////////////////////////////////////////////ECRIRE ENVOI DES RESULTATS
  }


//AFFICHE DES VALEURS SUR ECRAN LCD
  lcd.clear();
  lcd.setCursor(0,0);   //température
  lcd.print("T:");
  lcd.print(temp);
  lcd.setCursor(8,0);   //humidité
  lcd.print(" H: ");   
  lcd.print(hum);
  lcd.setCursor(0,1);   //luminosité
  lcd.print("Lum :");
  lcd.print(lum);
  lcd.display();

  /*
  //AFFICHE DANS LA CONSOLE, mis en commentaire car utile juste pour les tests, et perturbe l'envoi des valeurs vers Processing
  Serial.print("Humidité: ");
  Serial.write(int(hum));
  Serial.print("Température : ");
  Serial.write(int(temp));
  Serial.print("Luminosité : ");
  Serial.write(int(lum));
*/


//GESTION DES LED ET DU RELAI ENFONCTION DES VALEURS RECOLTEES//////////////////////////////////

  // on verifie que l'utilisateur a rentré les valeurs, sinon on lui demande
  if (!(diffPret and thPret)){
    delay(2000);
    lcd.clear() ;
    lcd.setCursor(0,0);
    lcd.print("Rentrez valeurs") ;
    lcd.setCursor(0,1) ;
    lcd.print("theo. et de diff") ;
    delay(1500);
    lcd.setCursor(0,1) ;
    lcd.print("sur Processing  ") ;
    delay(1500);
  }
  else {                    //ON GERE LES LED ET LE RESTE EN FONCTION DES CAPTEURS
     if ((abs(tempTh-temp)>diffTemp)||(abs(humTh-hum)> diffHum)||(abs(lumTh-lum)> diffLum)){  //si l'une des valeurs est trop éloignée de sa valeur théorique
        //passer la backlight de l'écran lcd en rouge ?
        ledV = true;
        if (digitalRead(PINledR)==LOW){
            digitalWrite(PINledR, HIGH);    //allume la LED rouge
        }
        if (digitalRead(PINledV)==HIGH){
            digitalWrite(PINledV, LOW);   //éteint la LED verte
        }
        if ( (lumTh-lum)> diffLum){   //si la luminosité est trop basse
          digitalWrite(PINrelai,HIGH);    //on active le relai pour allumer la LAMPE
        }
        else if ((lumTh-lum)<= diffLum){ //quand la luminosité est assez forte on éteint la lampe
          digitalWrite(PINrelai,LOW);
        }
        if ( (tempTh-temp)> diffTemp){   //si la température est trop basse
          digitalWrite(PINrelaiChauffe,HIGH);    //on active le relai pour allumer la RESISTANCE
        }
        else if ((tempTh-temp)<= diffTemp){    //quand il fait assez chaud on arrrete la résistance
          digitalWrite(PINrelaiChauffe,LOW);
        }
        delay(700);
    }
    else {      //si tout va bien
        if (digitalRead(PINrelaiChauffe)==HIGH){
            digitalWrite(PINrelaiChauffe, LOW);   //on éteint le relai pour résistance
        }
        if (digitalRead(PINrelai)==HIGH){
            digitalWrite(PINrelai, LOW);   //on éteint le relai pour lumière
        }
        if (digitalRead(PINledR)==HIGH){
            digitalWrite(PINledR, LOW);   //on éteint la LED rouge
        }
        if (digitalRead(PINrelai)==HIGH){
          digitalWrite(PINrelai, LOW);
        }
        if (digitalRead(PINledV)==LOW && ledV==true){
            digitalWrite(PINledV, HIGH);    // on allume la LED verte pour 10 secondes
            delay(700);
            digitalWrite(PINledV,LOW);
            ledV = false;
        }
        else {
          delay(700);
        }
    }
  }

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
