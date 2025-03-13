#include "XGBClassifier.h"
extern int WINDOW_SIZE;
extern float data[3][6][50]; //WINDOW_SIZE
extern String modDataString;
extern float modData[72];

Eloquent::ML::Port::XGBClassifier classifier;

void preprocessData(){
  Serial.println("Preprocessing Data");
  //subtract offsets from data!!!!!!!
  int modDataIndex = 0;

  for(int i = 0; i < 3; i++){ //sensor ID (123)
      for(int j = 0; j < 6; j++){ //dimension (Acc XYZ, Mag XYZ)
        //reset for each sensor/dimension
        double sum = 0;
        double maxVal = 0;
        double minVal = 9999999999999; //what value?
        for(int k = 0; k < WINDOW_SIZE; k++){
          sum += (data[i][j][k] - dataOffsets[i][j]);
          if(maxVal<(data[i][j][k] - dataOffsets[i][j])){
              maxVal = (data[i][j][k] - dataOffsets[i][j]);
          }
          if(minVal>(data[i][j][k] - dataOffsets[i][j])){
              minVal = (data[i][j][k] - dataOffsets[i][j]);
          }
          else{
            continue;
          }
        }
        double avg = sum/WINDOW_SIZE;
        
        //format is: mean, min, max, std
        
        modDataString += String(avg);
        modData[modDataIndex] = avg;
        modDataIndex++;
        modDataString += (", ");
        modDataString += String(minVal);
        modData[modDataIndex] = minVal;
        modDataIndex++;
        modDataString += (", ");
        modDataString += String(maxVal);
        modData[modDataIndex] = maxVal;
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
        modData[modDataIndex] = stdev;
        modDataIndex++;
      }
      modDataString += (", ");
 }
  Serial.println("Data Processed"); //pressing "t" does not do anything
  //Serial.println(modDataString); //NEED TO WRITE TO THE CSV
}

void classifyData(){
  Serial.println("Classifying Data");
  int result = classifier.predict(modData); //make sure you pass the preporcessed data to it
  //Serial.print(" \tPrediction, True: ");
  modDataString += String(result);
  //Serial.print(result);
  //Serial.print(", ");
  //Serial.println(label);
  Serial.print("Predicted label: ");
  Serial.println(result);
  Serial.println(modDataString);
}
