#include "XGBClassifier.h"
#include <string>
extern float data[3][6][WINDOW_SIZE]; //WINDOW_SIZE
extern float dataOffsets[3][6];
//extern String modDataString;
extern float modData[72];
extern unsigned long epochSeconds;
extern String modDataString = "";

Eloquent::ML::Port::XGBClassifier classifier;
void sort(float arr[], int n) { //need to send it each column
    float temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] > arr[j]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

float computeUpperQuartile(float offset, float arr[], int len){
  float value1 = arr[37] - offset;
  float value2 = arr[38] - offset;
  float q3 = (value1+value2)/2;
  return q3;
}


float computeLowerQuartile(float offset, float arr[], int len){
  float value1 = arr[12] - offset;
  float value2 = arr[13] - offset;
  float q1 = (value1+value2)/2;
  return q1;
}
void preprocessData(){
  Serial.println("Preprocessing Data");
  modDataString += String(epochSeconds);
  modDataString += ", ";
  //subtract offsets from data!!!!!!!
  int modDataIndex = 0;
  for(int i = 0; i < 3; i++){ //sensor ID (123)
      for(int j = 0; j < 6; j++){ //dimension (Acc XYZ, Mag XYZ)
        //reset for each sensor/dimension
        double sum = 0;
        sort(data[i][j],50); //send each column in
        double offset = dataOffsets[i][j]; 
        double q3 = computeUpperQuartile(offset, data[i][j], 50);
        double q1 = computeLowerQuartile(offset, data[i][j], 50);
        for(int k = 0; k < WINDOW_SIZE; k++){
          sum += (data[i][j][k] - dataOffsets[i][j]);
        }
        double avg = sum/WINDOW_SIZE;
        
        //format is: mean, q1, q3, std
        
        modDataString += String(avg);
        modData[modDataIndex] = avg;
        modDataIndex++;
        modDataString += (", ");
        modDataString += String(q1);
        modData[modDataIndex] = q1;
        modDataIndex++;
        modDataString += (", ");
        modDataString += String(q3);
        modData[modDataIndex] = q3;
        modDataIndex++;
        modDataString += (", ");
        
        
        //STANDARD DEVIATION
        double toSum = 0;
        double summation = 0;
        for(int l = 0; l < WINDOW_SIZE; l++){
                double mid = (data[i][j][l] - dataOffsets[i][j])-avg;
                toSum = mid*mid;
                summation += toSum;
        }
        double toSqrt = summation/WINDOW_SIZE;
        double stdev = sqrt(toSqrt);
        modDataString += String(stdev); //int to string??
        modDataString += (", ");
        modData[modDataIndex] = stdev;
        modDataIndex++;
      }
//      modDataString += (", ");
 }
  Serial.println("Data Processed"); //pressing "t" does not do anything
  //Serial.println(modDataString); //NEED TO WRITE TO THE CSV
}

void classifyData(){
  Serial.println("Classifying Data");
  int result = classifier.predict(modData); //make sure you pass the preporcessed data to it
  //Serial.print(" \tPrediction, True: ");
  modDataString += String(result);
  modDataString += '\n';
  //Serial.print(result);
  //Serial.print(", ");
  //Serial.println(label);
  Serial.print("Predicted label: ");
  Serial.println(result);
  Serial.println(modDataString);
}
