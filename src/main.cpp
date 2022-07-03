#include <Stepper.h>
#include <ArduinoJson.h>
// wifi part 
#include <WiFi.h>
// const char* WIFI_SSID = "POCO"; 
// const char* WIFI_PASS = "jhonier1234";
// const char* WIFI_SSID = "Vides Preciado"; 
// const char* WIFI_PASS = "familiavp";
const char* WIFI_SSID = "AlejandroS20"; 
const char* WIFI_PASS = "AlejandroS20";

WiFiServer server(80); 
String peticion;


const int LED = 23; 


#define stp3 26
#define dir3 25
#define stp2 33
#define dir2 32 
#define stp1 18
#define dir1 19
#define m0 12
#define m1 14
#define m2 27

// #define stp3 17  
// #define dir3 16
// #define stp2 4
// #define dir2 0 
// #define stp1 2
// #define dir1 15
// #define m0 19
// #define m1 18
// #define m2 5

int Micros = 32; 
int Estado_1 = 0;
int Estado_2 = 0;
int Estado_3 = 0;

int max_speed = 1000; 
int max_accel = 200; 

float Actual_Pos1 = 0;
float Actual_Pos2 = 0;
float Actual_Pos3 = 0;
float Pos1 = 0;
float Pos2 = 0;
float Pos3 = 0;

float microConst = 1;

int Punto = 1;
#include <AccelStepper.h>

//Creacion de los motores como objetos
AccelStepper Brazo_1(1,stp1,dir1);
AccelStepper Brazo_2(1,stp2,dir2);
AccelStepper Brazo_3(1,stp3,dir3); 

void set_config(){
  //Declaracion de velocidades y acelariones de los motores
  Brazo_1.setMaxSpeed(max_speed);
  Brazo_1.setAcceleration(max_accel);
  Brazo_2.setMaxSpeed(max_speed);
  Brazo_2.setAcceleration(max_accel);
  Brazo_3.setMaxSpeed(max_speed);
  Brazo_3.setAcceleration(max_accel);
}

void SetHome(){
  //Establecer la posición actual como posición cero 
  Brazo_1.setCurrentPosition(0); 
  Brazo_2.setCurrentPosition(0);
  Brazo_3.setCurrentPosition(0);
  //Se reconfigura las velocidades y aceleración 
  // ya que setCurrentPosition las establece cero 
  set_config();
}

void MicroStepping()
{
  switch (Micros)
  {
    case 1:
      digitalWrite(m0,LOW);
      digitalWrite(m1,LOW);
      digitalWrite(m2,LOW);   
      microConst = 200; 
    break;
    case 2:
      digitalWrite(m0,HIGH);
      digitalWrite(m1,LOW);
      digitalWrite(m2,LOW);
      microConst = 400;
    break;
    case 4:
      digitalWrite(m0,LOW);
      digitalWrite(m1,HIGH);
      digitalWrite(m2,LOW);
      microConst = 800;
    break;
    case 8:
      digitalWrite(m0,HIGH);
      digitalWrite(m1,HIGH);
      digitalWrite(m2,LOW);
      microConst = 1600;
    break;
    case 16:
      digitalWrite(m0,LOW);
      digitalWrite(m1,LOW);
      digitalWrite(m2,HIGH);
      microConst = 3200;
    break;
    case 32:
      digitalWrite(m0,HIGH);
      digitalWrite(m1,LOW);
      digitalWrite(m2,HIGH);
      microConst = 6400;
    break;
    default:
      digitalWrite(m0,LOW);
      digitalWrite(m1,LOW);
      digitalWrite(m2,LOW);
      microConst = 200;
    break;
  }
  
}

void Pos_Brazo_1()
{
  switch(Estado_1)
  {
    case 0:
      
      if(Pos1 != Actual_Pos1)
      {
        Actual_Pos1 = Pos1;
        Estado_1 = 1;
      }
    break;
    
    case 1:
      float stepReal_1 = (microConst/360);
      stepReal_1 = Actual_Pos1*stepReal_1;
      Brazo_1.moveTo(2*stepReal_1);
      
      Serial.print("Brazo 1 :" );
      Serial.println(Actual_Pos1);
      Estado_1 = 0;
    break;
  }
}

void Pos_Brazo_2()
{
  switch(Estado_2)
  {
    case 0:
      
      if(Pos2 != Actual_Pos2)
      {
        Actual_Pos2 = Pos2;
        Estado_2 = 1;
      }
    break;
    
    case 1:
      float stepReal_2 = (microConst/360);
      stepReal_2 = Actual_Pos2*stepReal_2;
      Brazo_2.moveTo(2*stepReal_2);
      Serial.print("Brazo 2 :" );
      Serial.println(Actual_Pos2);
      Estado_2 = 0;
    break;
  }
}

void Pos_Brazo_3()
{
  switch(Estado_3)
  {
    case 0:
      
      if(Pos3 != Actual_Pos3)
      {
        Actual_Pos3 = Pos3;
        Estado_3 = 1;
      }
    break;
    
    case 1:
      float stepReal_3 = (microConst/360);
      stepReal_3 = Actual_Pos3*stepReal_3;
      Brazo_3.moveTo(2*stepReal_3);
      Serial.print("Brazo 3 :" );
      Serial.println(Actual_Pos3);
      Estado_3 = 0;
    break;
  }
}


void MoveAll(){
  MicroStepping();  
  Pos_Brazo_1();
  Brazo_1.run();
  Pos_Brazo_2();
  Brazo_2.run();
  Pos_Brazo_3();
  Brazo_3.run();
}

int ToGoAll(){
  if (Brazo_1.distanceToGo()==0){
     if (Brazo_2.distanceToGo()==0){
        if ( Brazo_3.distanceToGo()==0){
          // Debugging si llegó a la posición requerida
          Serial.println("ToGoAll 0"); 
          return 0; 
        }
     } 
  }
  return 1; 
}


void CuartoDeMilla(){
  int Direccion;

  switch (Punto)
  {
    case 1:
      Pos1 = 0;
      Pos2 = 0;
      Pos3 = 0;
      if(!Brazo_1.distanceToGo() && !Brazo_2.distanceToGo() && !Brazo_3.distanceToGo())
      {
        Direccion = 1;
        Punto = 2;
      }
    break;
    case 2:
      Pos1 = 0;
      Pos2 = 0;
      Pos3 = 0;
      if(!Brazo_1.distanceToGo() && !Brazo_2.distanceToGo() && !Brazo_3.distanceToGo())
      {
        if(Direccion)
        {
          Punto = 3;
        }else
        {
          Punto = 1;
        }
      }
    break;
    case 3:
      
      if(!Brazo_1.distanceToGo() && !Brazo_2.distanceToGo() && !Brazo_3.distanceToGo())
      {
        if(Direccion)
        {
          Punto = 4;
          
        }else
        {
          Punto = 2;
        }
      }
    break;
    case 4:
     
      if(!Brazo_1.distanceToGo() && !Brazo_2.distanceToGo() && !Brazo_3.distanceToGo())
      {
        Direccion = 0;
        Punto = 3;
      }
    break;
  }
}

void setup() {
  Serial.begin(9600); 
  Serial.print("Conectando a "); 
  Serial.println(WIFI_SSID); 

  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  while(WiFi.status() != WL_CONNECTED){
    
    delay(500);
    Serial.print(".");
    
    }
  Serial.println("Conexión exitosa a "+ (String)WIFI_SSID) ;
  Serial.print("Dirreción IP: "); 
  Serial.println(WiFi.localIP()); 
  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");

  server.begin(); 
  
  set_config();
  //declaracion de pines de microstepping e inicializacion en 0 (FULL STEP)
  pinMode(m0,OUTPUT);
  pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
  digitalWrite(m0,LOW);
  digitalWrite(m1,LOW);
  digitalWrite(m2,LOW);
  
  SetHome(); 
  MicroStepping();
}

void loop() {

  StaticJsonDocument<400> doc;
  WiFiClient client = server.available();
  if(client){
    
    Serial.println(F("New client"));
    Serial.println(client.remoteIP());
    

    while(client.connected()){

      if(client.available()){
        
        
        while (1){
          String c = client.readStringUntil('\r\n\r'); 
          peticion += c; 
          
          if (c.length()<3) {
            break; 
          }
        }
        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          client.stop();
          return;
        }

        if (peticion.indexOf("python") >= 0){
          Serial.println("From python GUI");
        }

      
        client.println(F("HTTP/1.1 200 OK"));
        if (1){
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

        } 
        // else{
        //           // Write response headers cuando enviamos un Json          
        //   client.println(F("Content-Type: application/json"));
        //   client.println(F("Connection: close"));
        //   client.print(F("Content-Length: "));
        //   client.println(measureJsonPretty(doc));
        //   client.println();

        // }

        // la respuesta HTTP termina con una linea en blanco
        client.println();
        client.stop();
        break;
         
        
      }  
    }
    //  Limpiamos header
    peticion = "";
    
    
    // Extract values
    int Micros_doc = doc["Micros"]; 
    int doc_speed = doc["speed"];
    int doc_accel = doc["accel"];

    if (doc_speed != max_speed){
      max_speed = doc_speed;
      set_config();
    } 
    if (doc_accel != max_accel){
      max_accel = doc_accel;
      set_config(); 
    }

    JsonArray M1 = doc["M1"];
    JsonArray M2 = doc["M2"];
    JsonArray M3 = doc["M3"];
    
    // Serial.print("speed: ");
    // Serial.println(max_speed); 
    // Serial.print("accel: ");
    // Serial.println(max_accel);       
    // Pos1 = M1; 
    // Pos2 = M2; 
    // Pos3 = M3; 
    Serial.print("micros json: ");
    Serial.println(Micros_doc);
    
    Serial.println("");  
    serializeJson(doc, Serial);
    Serial.println(""); 

    if (Micros_doc == -1) {
      SetHome(); 
    }
    // else if(Micros_doc == -2){
    //   Punto = 1;

    // } 
    else{
      Punto = 1;
      Micros = Micros_doc; 

    }      
    int n = M1.size() -1; 
    // Serial.print(n); 
    for (int i = 0; i <= n ; i++) {
      Pos1 = M1[i]; 
      Pos2 = M2[i]; 
      Pos3 = M3[i];
      Serial.println(Pos1); 
      MoveAll();
      MoveAll();
      while(ToGoAll()){
        Serial.print("."); 
        MoveAll(); 
      }
    }
  }

  
  
  MoveAll(); 
  
  
  
}



