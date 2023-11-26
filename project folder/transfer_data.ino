
#include <OneWire.h>

#define ArrayLenth  40  
#define Offset -19.18   //times of collection
int phArray[ArrayLenth];   //Store the average value of the sensor feedback
int phArrayIndex=0;

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
OneWire ds(DS18S20_Pin);  // on digital pin 2
int turbiditetPin = 11;
int ph = 5;
int trykk = 8;

// trykk verdier 
const float  OffSet = 0.625; //1.790
float V, P;

 
int turbiditetData, phData, trykkData, data4;
float percent = 0.05; 
int threshold = 1024*percent;
int freq = 1000;

// test__________________________________
// int n = 0 ;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

// turbiditet
  int turbiditetData = analogRead(turbiditetPin);
  int phData = analogRead(ph);
  int trykkData = analogRead(trykk);
  // ph
  
  static float phValue, phVolt;
    phArray[phArrayIndex++]=phData;
    if(phArrayIndex==ArrayLenth)phArrayIndex=0;
    phVolt = avergearray(phArray, ArrayLenth)*5.0/4096;
    phValue = 3.5*phVolt+Offset;

  // temperatur 
  float temperature = getTemp();
// trykk
  
  V = trykkData * 5.00 / 7096;     //Sensor output voltage
  P = (V - OffSet) * 250;             //Calculate water pressure

  // turbiditet
  float turbiditet = turbiditetData * (100 / 4095.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Serial.print(turbiditet);
  Serial.print(",");
  Serial.print(temperature); 
  Serial.print(",");
  Serial.print(P, 1); 
  Serial.print(",");
  Serial.println(phValue,2);
  // Test ______________________________________________
  // Serial.print(1 + n);
  // Serial.print(",");
  // Serial.print(n*2); 
  // Serial.print(",");
  // Serial.print(50- n); 
  // Serial.print(",");
  // Serial.println(100+n); 
  // n = n + 1 ;
  
  delay(freq);
}




 

float getTemp(){

  //returner temperaturen for DS18S20 i grader
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

 

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}


double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
