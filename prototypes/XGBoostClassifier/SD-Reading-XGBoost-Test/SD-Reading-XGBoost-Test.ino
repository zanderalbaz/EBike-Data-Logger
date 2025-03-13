#include <SPI.h>
#include <SD.h>
#include "XGBClassifier.h"
Eloquent::ML::Port::XGBClassifier classifier;
File dataCSV;
float data[9];
int label;
void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("SD Init");

  if(!SD.begin(8)){
    Serial.println("SD Init Failed :(");
    while(1);  
  }
  Serial.println("SD Init Successful :)");
  dataCSV = SD.open("testData.csv", FILE_READ);

  if(dataCSV){
    Serial.println("testData.csv:");
    int lineNum = 0;
    while(dataCSV.available()){
      String line = dataCSV.readStringUntil('\n');
      lineNum++;
      if(lineNum == 1){
        continue;  
      }
      
      char lineCharArray[line.length() +1];
      line.toCharArray(lineCharArray, sizeof(lineCharArray));

      int valNum = 0;
      char* val = strtok(lineCharArray, ",");
      while(val != NULL){
          data[valNum] = atof(val);
          valNum++;
          val = strtok(NULL, ",");
          if(valNum == 9){
            label = atoi(val);
          }
      }
      printAndClassifyData();
    }
 
    dataCSV.close();
    
  }else {
    Serial.println("Could not open file");  
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

void printAndClassifyData(){
    for(int j=0; j<9; j++){ //for each sensor val in data instance
      Serial.print(data[j]);
      if(j != 8){
        Serial.print(", \t");
      }   
    }
    int result = classifier.predict(data);
    Serial.print(" \tPrediction, True: ");
    Serial.print(result);
    Serial.print(", ");
    Serial.println(label);
}
