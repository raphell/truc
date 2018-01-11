import processing.serial.*;

boolean debut = true ;
int jourDebut=1 ;


PrintWriter fichierTxt ;
String[] infosTxt ;
String nomFichTxt;
int compteurTxt ;
int ligneMax ;

int tailleValeurs = 8;

char valeurTh[] = new char[tailleValeurs] ;
char valeurDiff[] = new char[tailleValeurs] ;

int i = 0;
int j = 0;

boolean afficher = false ;

int action = 0; //1 pour rentrer des valeurs théoriques, 2 pour rentrer la différence de valeur
Serial monPort; // Objet responsable de la lecture de valeurs sur le port serie


void setup() {
  size(displayWidth, displayHeight);
  
  
  quad(50,50,50,470,904,470,904,50);
  quad(50,520,50,940,904,940,904,520);
  quad(1000,520,1000,940,1854,940,1854,520);
  
  
  for (int i =0 ; i<=420 ; i=i+70){    //axe Y Temp
    line(50,470-i,40,470-i);
    stroke(125,125,125,125);
    line(904,470-i,40,470-i);
    stroke(0,0,0);
  }
  for (int i =0 ; i<=420 ; i=i+35){
    line(50,470-i,45,470-i);
  }
  
  for (int i =0 ; i<=420 ; i=i+42){    //axe Y Hum
    line(50,940-i,40,940-i);
    stroke(125,125,125,125);
    line(904,940-i,40,940-i);
    stroke(0,0,0);
  }
  for (int i =0 ; i<=420 ; i=i+21){
    line(50,940-i,45,940-i);
  }
  
  for (int i =0 ; i<=420 ; i=i+105){    //axe Y Lum
    line(1000,940-i,990,940-i);
    line(1850,940-i,990,940-i);
  }
  for (int i =0 ; i<=420 ; i=i+21){
    line(1000,940-i,995,940-i);
    stroke(125,125,125,125);
    line(1854,940-i,1000,940-i);
    stroke(0,0,0);
  }
  
  fill(0);
  text("Humidité :", 50, 510);
  text("Température :", 50, 40);
  text("Luminosité :", 1000, 510);
 
  
  int d = day() ;
  int mo = month() ;
  int y = year() ;
  String nomFichTxt = "ValeursSerre-"+d+"/"+mo+"/"+y;
  fichierTxt = createWriter(nomFichTxt);
 
  /*
  
  //Affichage des ports serie disponibles pour trouver la carte Arduino
  println("Ports serie disponibles :");
  String [] listePorts = Serial.list(); 
  for (int i=0; i<listePorts.length;i++) { 
      println("Port ",i," : ",listePorts[i]);
    } 
 
  // on choisit le port sur lequel se trouve l'Arduino
  String nomPort = listePorts[0] ; 
  monPort = new Serial(this, nomPort, 9600);
  monPort.bufferUntil('\n');

  */
  
      // 3) TO DO : OUVRIR UN FICHIER POUR Y STOCKER LES VALEURS QUI SERONT LUES
   println("Tapez \"t\" pour rentrer les valeurs théoriques");
   println("Tapez \"d\" pour rentrer les valeurs de différences acceptées");
}




void keyPressed(){
  if (action==1){    //on rentre les nouvelles valeurs théoriques
    if(i==0){
      valeurTh[i] = 't';
      valeurTh[i+1] = key;
      i=i+2;
      println(valeurTh[1]);
    }
    else if(i<tailleValeurs){
      valeurTh[i] = key;
      println(valeurTh[i]);
      i=i+1;
    }
    else if(i==tailleValeurs){
      println("juste avant envoi");
      j = 0;
      while (j<8){
        monPort.write(valeurTh[j]);      //envoyer valeurs sur arduino
        print(valeurTh[j]);
        j=j+1;
      }         
      action = 0;
      i=0;
      println("  Saisie terminée");
    }
  }
  else if (action==2){                  //on rentre les nouvelles valeurs pour la différence acceptée
    if(i==0){
      valeurDiff[i] = 'd';
      valeurDiff[i+1] = key;
      println(valeurDiff[1]);
      i=i+2;
    }
    else if(i<tailleValeurs){
      valeurDiff[i] = key;
      println(valeurDiff[i]);
      i=i+1;
    }
    else if(i==tailleValeurs){
      println("Valeurs envoyées :");
      j = 0;
      while (j<8){
        monPort.write(valeurDiff[j]);      //envoyer valeurs sur arduino
        print(valeurDiff[j]);
        j=j+1;
      }         
      action = 0;
      i=0;
      println("  Saisie terminée");
    }
  }
  
  if(key=='t'){           //choix de l'action: 't' pour valeurs théoriques, 'd' pour différence acceptée
    action=1;
    println("Rentrez vos valeurs théoriques sous la forme \"TTHHLLL\"");
    println("TT : le nombre à deux chiffres de la température");
    println("HH : le nombre à deux chiffres de l'humidité");
    println("LLL : le nombre à trois chiffres de la luminosité");
  }
  else if(key=='d'){
    action=2;
    println("Rentrez vos valeurs de différences acceptées sous la forme \"TTHHLLL\"");
    println("rentrez la valeur maximal si vous ne souhaitez pas que cette variable soit prise en compte");
  }
  else if (key=='p'){
    afficher = true ;
  }
}

////////////////////////////////RECEPTION ET STOCKAGE DES VALEURS D ARDUINO
void serialEvent (Serial monPort){
  println("VALEURS RECUPEREES");
  String valeurArduiStr = monPort.readStringUntil('\n');
  valeurArduiStr = valeurArduiStr.substring(0,9) ;
  valeurArduiStr = valeurArduiStr+"," ;
  int m = minute() ;
  int h = hour() ;
  int d = day() ;
  int mo = month() ;
  int y = year() ;
  
  if (d-jourDebut==7){
    nomFichTxt = y+"/"+mo+"/"+"ValeursSerre-"+d;
    fichierTxt = createWriter(nomFichTxt);
    jourDebut = d ;
  }
  
  String StringAStocker = valeurArduiStr +h+","+m+","+d+","+mo+","+y ;
  println(StringAStocker);
  
  println("PRET A STOCKER") ;
  fichierTxt.println(StringAStocker);  //on écrit les valeurs sous forme d'une ligne texte dans le fichier texte
  fichierTxt.flush() ;
  
}


void draw() {
  
  int temp ,hum ,lum;
  int h ,m ,d ,mo ,y ,jourAff ;
  float x1, x2, yt, yh, yl ;
  infosTxt=loadStrings(nomFichTxt);
  ligneMax = infosTxt.length -1;
  println(ligneMax) ;
  
  for (int j=0 ; j<ligneMax ; j++){
    String valeurs[] = split(infosTxt[j],",");
    println("RELEVE : "+j) ;
    temp = int(valeurs[0]) ;
    hum = int(valeurs[1]);
    lum = int(valeurs[2]);
    h = int(valeurs[3]);
    m = int(valeurs[4]);
    d = int(valeurs[5]);
    mo = int(valeurs[3]);
    y = int(valeurs[7]);
    
    String truc = "t:"+temp+" h:"+hum+" l:"+lum ;
    println(truc);
    
    if (debut) {
      jourDebut = d ;
      debut = false ;
    }
    
    if (d>=jourDebut){                
      jourAff = d - jourDebut ;
    }
    else{
      if (mo==1||mo==3||mo==5||mo==7||mo==8||mo==10||mo==12){
        jourAff = 31 +d -jourDebut  ;
      }
      else if(mo==2){
        jourAff = 28 +d -jourDebut  ;
      }
      else{
        jourAff = 30 +d -jourDebut  ;
      }
    }
    
    
    x1 = 50 + jourAff*122 + h*(122/24);     
    x2 = 1000 + jourAff*122 + h*(122/24);
    yt = temp*7 ;
    yh = hum*4.2 ;
    yl = lum*0.42 ;
    
    print(yt);
    
    ellipse(x1,470-yt, 2, 2);     //affiche le point de temp
    ellipse(x1,940-yh, 2, 2);     //affiche le point de hum
    ellipse(x2,940-yl, 2, 2);     //affiche le point de lum
    
  }

}