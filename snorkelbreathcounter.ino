/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
*/
const int numReadings = 10;
int breath = 0;
int readings[numReadings]; // the readings from the analog input

int readIndex = 0; 
// the index of the current reading
int total = 0;
int zeroComp = 0;
#include <SD.h>
#include <Wire.h>
int average = 0;
// the average
#define ledPin 10
#define cardSelect 4
File logfile;
char filename[15];
bool ting = false;
float rateCount = 0.0;
int inhale = 0;
int exhale = 0;
int lowCount = 0;
int highCount = 0;
bool node = true;
#define PERIOD 20000  // period in us

unsigned long last_us = 0L;
unsigned long timeKeep = 0L;


unsigned long timer = 0L;
#define LongPeriod 60000
// the setup routine runs once when you press reset:
void setup() {
   // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
   
  }
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  strcpy(filename, "ANALOG00.TXT");//this section sets up new files for each new reading
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
   logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
  delay(500);
  zeroOut();
}

// the loop routine runs over and over again forever:
void loop() {
  
  // read the input on analog pin 0:
   if (micros () - last_us > PERIOD)
  {
    last_us += PERIOD ;
    sample () ;
  }
}
void sample ()
{
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (3300.0 / 1023.0);
     
  // print out the value you read:
  voltage = voltage - zeroComp;
  //Serial.println(voltage);
  voltage = constrain(voltage, 0, 15);
  int inOut = map(voltage,0,15,0, 20);
  inOut = constrain(inOut,0,20);
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = inOut;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  
  
  if(average >1  && node == true){
    highCount += 1;
  }
    if(highCount > 5 && node == true){
      node = false;
      highCount = 0;
    }
    if( average < 2 && node == false){
      lowCount += 1;
      
  }
  if(lowCount > 5 && node == false){
    breath +=1;
    if(breath % 2 == 0) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
    lowCount = 0;
    if((millis() - timeKeep) < 100) ting = false;
    if((millis() - timeKeep) > 4000) ting = false;
    if(ting){
        rateCount = 60.0/(( millis() - timeKeep)/1000.0);
        rateCount = constrain( rateCount, 10, 120);
        Serial.print(" Rate ");
        Serial.println(rateCount);
        Serial.print("Time");
        Serial.println(millis()/60000);
         Serial.print(" correction factor ");
  Serial.println(zeroComp);
         logfile.print("Rate "); logfile.print(rateCount);
      logfile.print("  TotalBreaths");
      logfile.print("  ");
      logfile.print(breath);
      logfile.print("  Time:");
      logfile.println( millis()/60000);
      logfile.flush();
        ting = false;
      }
    timeKeep = millis();
    ting = true;
    node = true;
  }


}

 void zeroOut(){
  float  total = 0;
  for(int i = 0; i< 10; i++){
    total = total + float(analogRead(A0)* (3300.0/1023.0));
     
  }
  total = total/10;
  zeroComp = total;
 
 }

