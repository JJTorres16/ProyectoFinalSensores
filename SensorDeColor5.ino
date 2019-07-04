/*PROYECTO FINAL DE LA MATERIA DE SENSORES Y TRANSDUCTORES PARA MECATRÓNICA.
 * MULTIPLEXACIÓN PARA FILTROS DE COLOR y DE ESCALA DE FRECUENCIA.
 *  
 *         S2 | S3 | FOTODIODO           S0 | S1 | FRECUENCIA DE SALIDA
 *         
 *         0     0      rojo              0    0     corriente cortada   
 *         0     1      azul              0    1     2% de la frecuencia.
 *         1     0     sin filtro         1    0     20% de la frecuencia.
 *         1     1      verde             1    1     100% de la frecuencia.
 *         
 *         Para trabajar con microcontroladores se recomienda utilizar 
 *         unda frecuencia del 20%. Por lo tanto S0-> 1 y S1 -> 0.
*/

#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

//Creamos un objeto LCD.
LiquidCrystal_I2C lcd(0x27,16,2); // si no te sale con esta direccion  puedes usar (0x3f,16,2) || (0x27,16,2)  ||(0x20,16,2)

//....... CREACIÓN DE LAS CARIABLES GLOBALES.......//

/*Declaración de los pines del sensor*/
#define S0 4
#define S1 3
#define S2 6
#define S3 5
#define OE 2

/*Pines del push button para el control de las lecturas*/
#define LED 13
#define SWT 12

/*Variables del programa*/
byte salidas [5] = {S0,S1,S2,S3,OE};
const int out = 7; //La salida de nuestro sensor.
int ControlFrecuencia = 2;
unsigned long duracion_pulso;
int RGB_ROJO=0, RGB_AZUL=0, RGB_VERDE=0;
int frecuencia_rojo=0, frecuencia_azul=0, frecuencia_verde=0;
int FreFinal_rojo=0, FreFinal_azul=0, FreFinal_verde=0;
int Val_Rojo=0, Val_Azul=0, Val_Verde=0;
int contador_medida=0;
char LabView_Rojo[50],LabView_Verde[50],LabView_Azul[50];
int Lab_Rojo,Lab_Verde,Lab_Azul;

boolean estado;
int cuenta_SWT = 0;

void setup() {
  // Establecemos la comuniación serial.
  Serial.begin(9600);
  
  //Configuramos las salidas.
  for(int index = 0; index < 5; index++){
      pinMode(salidas[index], OUTPUT);
      delay (5);
  }
  
   //Configuramos la entrada.
  pinMode(out,INPUT);
  pinMode(LED,OUTPUT);
  pinMode(SWT,INPUT);
 
  //Elegimos la escala de nuestra frecuencia.
  EscalaFrecuencia();
  delay(100);


  //Configuración del cristal líquido por medio del protocolo I2C.
   lcd.init();
   lcd.backlight();
   lcd.clear();

   digitalWrite(OE,LOW);
   digitalWrite(estado,LOW);

   lcd.print("Listo");
   delay(2000);
   lcd.clear();
}

void loop() {

  //delay(100);
  //Serial.write(RGB_VERDE);
  //Serial.write(RGB_AZUL);
  
  //Control de habilitación del sensor
  estado = digitalRead(SWT);

  if(estado == HIGH){
    delay(100);
    digitalWrite(LED,HIGH);
    delay(5);
    cuenta_SWT++;
    }
    
  if(cuenta_SWT == 1){
  // Elegimos el filtro rojo:
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  //Leyendo la frecuencia de salida del sensor.
  frecuencia_rojo = (pulseIn(out,HIGH));
  //frecuencia_rojo += pulseIn(out,HIGH);
  delay(100);

  // Elegimos el filtro verde:
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  //Leyendo la frecuencia de salida del sensor.
  frecuencia_verde = (pulseIn(out,HIGH));
  //frecuencia_verde += pulseIn(out,HIGH);
  delay (100);

  // Elegimos el filtro azul:
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);

  //Leyendo la frecuencia de salida del sensor.
  frecuencia_azul = (pulseIn(out,HIGH));
  //frecuencia_azul += pulseIn(out,HIGH);
  delay(100);

 PromedioValores();
  }
}

void EscalaFrecuencia (){
  switch(ControlFrecuencia){

    case 0:{
      digitalWrite(S0,LOW);
      digitalWrite(S1,LOW);
      Serial.println("Corriente cortada");
      Serial.println(" ");
      break;
      }
     case 1:{
      digitalWrite(S0,LOW);
      digitalWrite(S1,HIGH);
      Serial.println("Frecuencia al 2%");
      Serial.println(" ");
      break;
      }
     case 2: {
      digitalWrite(S0,HIGH);
      digitalWrite(S1,LOW);
      //Serial.println("Frecuencia al 20%");
      Serial.println(" ");
      break;
      }
      case 3: {
      digitalWrite(S0,HIGH);
      digitalWrite(S1,HIGH);
      Serial.println("Frecuencia al 100%");
      Serial.println(" ");
      break;
      }
    }
  }

  void EscalaRGB(){

    lcd.clear();
    
    RGB_ROJO = map(Val_Rojo,10,180,255,0);
    if(RGB_ROJO < 0){
      RGB_ROJO = 0;
      }
    if(RGB_ROJO > 255){
      RGB_ROJO = 255;
      }  
    Lab_Rojo = RGB_ROJO;  
    String R = (String)(RGB_ROJO);
    lcd.setCursor(0,0);
    lcd.print("R=");
    lcd.setCursor(2,0);
    lcd.print(R);
    delay(100);

     RGB_VERDE = map(Val_Verde,17,185,255,0);
    if(RGB_VERDE < 0){
      RGB_VERDE = 0;
      }
    if(RGB_VERDE > 255){
      RGB_VERDE = 255;
      }
    String G = (String)(RGB_VERDE);
    Lab_Verde = RGB_VERDE;
    lcd.setCursor(6,0);
    lcd.print("G=");
    lcd.setCursor(8,0);
    lcd.print(G);
    delay(100);

     RGB_AZUL = map(Val_Azul,0,139,255,0);
    if(RGB_AZUL < 0){
      RGB_AZUL = 0;
      }
    if(RGB_AZUL > 255){
      RGB_AZUL = 255;
      }  
     Lab_Azul = RGB_AZUL; 
     lcd.setCursor(0,1);
     lcd.print("B=");  
    String B = (String)(RGB_AZUL);
    lcd.setCursor(2,1);
    lcd.print(B);
    delay(100);

    Enviar_Labview();

    delay(3000);
    }  

  void PromedioValores(){
    FreFinal_rojo += frecuencia_rojo;
    delay(100);
    FreFinal_verde += frecuencia_verde;
    delay(100);
    FreFinal_azul += frecuencia_azul;
    delay(100);

    contador_medida++;

    if(contador_medida == 10){
      Val_Rojo = FreFinal_rojo/10;
      delay(100);
      Val_Verde = FreFinal_verde/10;
      delay(100);
      Val_Azul = FreFinal_azul/10;
      delay(100);

      EscalaRGB();
      delay(100);

      FreFinal_rojo=0;
      delay(5);
      FreFinal_verde=0;
      delay(5);
      FreFinal_azul=0;
      delay(5);
          
      digitalWrite(LED,LOW);
      delay(5);
      cuenta_SWT = 0;
      delay(5);
      contador_medida=0;
      }
    }

    void Enviar_Labview(){
      Serial.print(Lab_Rojo);
      Serial.print(",");
      Serial.print(Lab_Verde);
      Serial.print(",");
      Serial.println(Lab_Azul);
      delay(100);
      }
