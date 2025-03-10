#include <ICM_20948.h>
#include <SPI.h>
#include <SD.h>
#include "XGBClassifier.h"
Eloquent::ML::Port::XGBClassifier classifier;
#include "driver/rtc_io.h" //This is needed for deep sleep wakeup pin configuration

#include <cmath>

#define SENSOR1_AD0_PIN 15
#define SENSOR2_AD0_PIN 2
#define SENSOR3_AD0_PIN 4
#define WAKEUP_INT_PIN GPIO_NUM_13

#define SD_CHIP_SELECT 5  //Sparkfun SD Shield chip select = 8

#define DELETE_FILE false
#define FILENAME "/test.csv"

#define WINDOW_SIZE 50
#define CALIBRATION_ITERATIONS 50

#define COLLECTION_MODE false

#define AD0 1

#define WOM_threshold 50

unsigned long startCycleMillis, stopCycleMillis;
unsigned long startSetupMillis, stopSetupMillis;

float samplingHz = 0.0;
int dataIndex = 0;

String modDataString = "";
float modData[72]; //can change all doubles to floats

ICM_20948_I2C currentICM;

enum State {
  SENSOR1_COLLECTION,
  SENSOR2_COLLECTION,
  SENSOR3_COLLECTION,
  STOPPED,
  PRINT,
  SD_WRITE,
  IDLE,
  SD_READ,
  SLEEP,
  PREPROCESSING,
  CLASSIFICATION,
  CALIBRATION
};

State state = State::STOPPED;

float data[3][6][WINDOW_SIZE]; //sensorNum, feature, instances
float dataOffsets[3][6];



///////////////////////////////////////
//
//                SETUP
//
///////////////////////////////////////

void setup()
{
  startSetupMillis = millis();
  pinMode(SENSOR1_AD0_PIN, OUTPUT);
  pinMode(SENSOR2_AD0_PIN, OUTPUT);
  pinMode(SENSOR3_AD0_PIN, OUTPUT);

  //Stop holding pins (after sleep)
  esp_sleep_enable_ext0_wakeup(WAKEUP_INT_PIN, HIGH);
  gpio_hold_dis(WAKEUP_INT_PIN);  
  gpio_hold_dis((gpio_num_t)SENSOR1_AD0_PIN);
  gpio_hold_dis((gpio_num_t)SENSOR2_AD0_PIN);
  gpio_hold_dis((gpio_num_t)SENSOR3_AD0_PIN);

  gpio_deep_sleep_hold_dis();

  Serial.begin(115200);
  
  while(!Serial);

  Wire.begin();
  Wire.setClock(400000);


  initializeSensors();
  initializeSD();
  if(DELETE_FILE){
    removeSDFile();
  }
  
  State state = State::SLEEP;
  
  stopSetupMillis = millis();
  Serial.print("Setup Time (ms): ");
  Serial.println(stopSetupMillis - startSetupMillis);
}

///////////////////////////////////////
//
//                LOOP
//
///////////////////////////////////////

void loop()
{
  while(Serial.available()){
    handleUserInput();
  }
  switch (state) {
    case State::STOPPED:
      state = State::SENSOR1_COLLECTION;
      break;
    case State::SENSOR1_COLLECTION:
//      startCycleMillis = millis();
      collectSensorData(dataIndex);
      switchSensorTo(State::SENSOR2_COLLECTION);
      break;
    case State::SENSOR2_COLLECTION:
      collectSensorData(dataIndex);
      switchSensorTo(State::SENSOR3_COLLECTION);
      break;
    case State::SENSOR3_COLLECTION:
      collectSensorData(dataIndex);
      if(COLLECTION_MODE){
        state = State::PRINT;   
      }
      else{
        dataIndex++;
//        stopCycleMillis = millis();
//        samplingHz = 1.0/((stopCycleMillis - startCycleMillis) / 1000.0);
//        Serial.println(samplingHz);        
        if(dataIndex % WINDOW_SIZE == 0){ //If our data window is full
            dataIndex = 0;
            state = State::CALIBRATION;
        }
        else{
           state = State::PRINT;   
        }
      }   
      break;
    case State::CALIBRATION:
      calibrateSensors(CALIBRATION_ITERATIONS);
      state = State::PREPROCESSING;
      break;
    case State::PREPROCESSING:
      preprocessData();
      state = State::CLASSIFICATION;
      break;
    case State::CLASSIFICATION:
      classifyData();
      state = State::SD_WRITE;
      break;
    case State::SD_WRITE:
      writeSensorDataToSD();
      state = State::SLEEP;
      break;
    case State::PRINT:
      if(COLLECTION_MODE){
        printSensorData(0);
      }
      else{
        printSensorData(dataIndex-1);
      }
      switchSensorTo(State::SENSOR1_COLLECTION);
      break;   
    case State::IDLE:
      delay(7);
      break;
    case State::SD_READ:
      readDataFromSD(); 
      state = State::IDLE;
      break;
    case State::SLEEP:
      Serial.println("Entering Deep Sleep");
      switchSensorTo(State::SENSOR2_COLLECTION);
      //Hold AD0 pins high during sleep ref
      //https://electronics.stackexchange.com/questions/350158/esp32-how-to-keep-a-pin-high-during-deep-sleep-rtc-gpio-pull-ups-are-too-weak

      //Lock AD0 pins 
      gpio_hold_en((gpio_num_t)SENSOR1_AD0_PIN);  
      gpio_hold_en((gpio_num_t)SENSOR2_AD0_PIN);  
      gpio_hold_en((gpio_num_t)SENSOR3_AD0_PIN);  
      gpio_deep_sleep_hold_en();

       //The code below is from WakeOnMotion example sketch
      currentICM.cfgIntActiveLow(false);  // Active high to allow interrupt to wakeup the ESP32
      currentICM.cfgIntOpenDrain(false); // Push-pull, though open-drain would also work thanks to the pull-up resistors on the breakout
      currentICM.cfgIntLatch(true);      // Latch the interrupt until cleared
      Serial.print(F("cfgIntLatch returned: "));
      Serial.println(currentICM.statusString());
      
      currentICM.WOMThreshold(WOM_threshold); // set WoM threshold
      Serial.print(F("Set threshold returned: "));
      Serial.println(currentICM.statusString());
      
      currentICM.WOMLogic(true, 1); // enable WoM Logic mode 1
      
      currentICM.intEnableWOM(true); // enable interrupts on WakeOnMotion
      Serial.print(F("intEnableWOM returned: "));
      Serial.println(currentICM.statusString());
      
      currentICM.WOMThreshold(WOM_threshold); // set WoM threshold - just in case...
      Serial.print(F("Set threshold returned: "));
      Serial.println(currentICM.statusString());
      
      //END EXAMPLE SKETCH CODE
      
      esp_deep_sleep_start();
      break;
  };
}

void initializeSD(){
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    while(true);
  }
  Serial.println("card initialized.");
}

void removeSDFile(){
  SD.remove(FILENAME);
  if(!SD.exists(FILENAME)){
    Serial.println("File was deleted sucessfully");
  }
}

//I THINK THIS WORKS??!?!?
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
  /*
  for(int m = 0; m<72; m++){
      Serial.print(modData[m]);
      Serial.print(", ");
  }
*/
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




void handleUserInput(){
  String input = Serial.readString();
  if (input == "c"){
    calibrateSensors(CALIBRATION_ITERATIONS);
  }
  else if(input == "r"){
    initializeSensors();
  }
  else if(input == "i"){
    state = State::IDLE;
  }
  else if(input == "t"){
    state = State::SD_READ;  
  }
  else if(input == "s"){
    state = State::SLEEP;
  }
  else if(input == "b"){
    switchSensorTo(State::SENSOR1_COLLECTION);
  }
}

void initializeSensors(){
  digitalWrite(SENSOR1_AD0_PIN, HIGH);
  digitalWrite(SENSOR2_AD0_PIN, LOW);
  digitalWrite(SENSOR3_AD0_PIN, LOW);

  bool initialized = false;
  while (!initialized)
  {
    currentICM.begin(Wire, AD0);
    

    Serial.print(F("Initialization current sensor: "));
    Serial.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(20);
    }
    else{
      initialized = true;
    }
  }

  digitalWrite(SENSOR1_AD0_PIN, LOW);
  digitalWrite(SENSOR2_AD0_PIN, HIGH);
  digitalWrite(SENSOR3_AD0_PIN, LOW);

  initialized = false;
  while (!initialized)
  {
    currentICM.begin(Wire, AD0);

    Serial.print(F("Initialization current sensor: "));
    Serial.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(20);
    }
    else{
      initialized = true;
    }
  }

  digitalWrite(SENSOR1_AD0_PIN, LOW);
  digitalWrite(SENSOR2_AD0_PIN, LOW);
  digitalWrite(SENSOR3_AD0_PIN, HIGH);

  initialized = false;
  while (!initialized)
  {
    currentICM.begin(Wire, AD0);

    Serial.print(F("Initialization current sensor: "));
    Serial.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(20);
    }
    else{
      initialized = true;
    }
  }
}

void printState(){
  switch (state){
    case State::SENSOR1_COLLECTION:
      Serial.println("SENSOR1 COLLECTION");
      break; 
    case State::SENSOR2_COLLECTION:
      Serial.println("SENSOR2 COLLECTION");
      break;
    case State::SENSOR3_COLLECTION:
      Serial.println("SENSOR3 COLLECTION");
      break;
    case State::PRINT:
      Serial.println("PRINT");
      break; 
    case State::STOPPED:
      Serial.println("STOPPED");
      break; 
    case State::SD_WRITE:
      Serial.println("SD WRITE");
    default:
      Serial.println("NOT IN A STATE");
      break;
  }
}

void switchSensorTo(State newState) {
  switch (newState) {
    case State::SENSOR1_COLLECTION:
//            Serial.println("Switched to sensor1");
      digitalWrite(SENSOR1_AD0_PIN, HIGH);
      digitalWrite(SENSOR2_AD0_PIN, LOW);
      digitalWrite(SENSOR3_AD0_PIN, LOW);
      break;
    case State::SENSOR2_COLLECTION:
//            Serial.println("Switched to sensor2");
      digitalWrite(SENSOR1_AD0_PIN, LOW);
      digitalWrite(SENSOR2_AD0_PIN, HIGH);
      digitalWrite(SENSOR3_AD0_PIN, LOW);
      break;
    case State::SENSOR3_COLLECTION:
//            Serial.println("Switched to sensor3");
      digitalWrite(SENSOR1_AD0_PIN, LOW);
      digitalWrite(SENSOR2_AD0_PIN, LOW);
      digitalWrite(SENSOR3_AD0_PIN, HIGH);
      break;
  };
  state = newState;
  delay(7);
}

void collectSensorData(int index) {
  int attempts = 0;
  while (!currentICM.dataReady()) {
    if (currentICM.statusString() == "Data Underflow") {
//      Serial.println("Data Underflow");
      attempts++;
      if(attempts >= 5){
        return;
      }
      delay(5);
    }
    else{
      Serial.println(currentICM.statusString());
    }
  };
  currentICM.getAGMT();
  switch(state){
    case State::SENSOR1_COLLECTION:
      data[0][0][index] = currentICM.magX();  
      data[0][1][index] = currentICM.magY();  
      data[0][2][index] = currentICM.magZ();  
      data[0][3][index] = currentICM.accX();  
      data[0][4][index] = currentICM.accY();  
      data[0][5][index] = currentICM.accZ(); 
      break;
    case State::SENSOR2_COLLECTION:
      data[1][0][index] = currentICM.magX();  
      data[1][1][index] = currentICM.magY();  
      data[1][2][index] = currentICM.magZ();  
      data[1][3][index] = currentICM.accX();  
      data[1][4][index] = currentICM.accY();  
      data[1][5][index] = currentICM.accZ(); 
      break;
    case State::SENSOR3_COLLECTION:
      data[2][0][index] = currentICM.magX();  
      data[2][1][index] = currentICM.magY();  
      data[2][2][index] = currentICM.magZ();  
      data[2][3][index] = currentICM.accX();  
      data[2][4][index] = currentICM.accY();  
      data[2][5][index] = currentICM.accZ(); 
      break;
  }
}

void printSensorData(int index) {
  Serial.print("MagX1: ");
  printFormattedFloat(data[0][0][index] - dataOffsets[0][0], 5, 2);
  Serial.print(", MagY1: ");
  printFormattedFloat(data[0][1][index] - dataOffsets[0][1], 5, 2);
  Serial.print(", MagZ1: ");
  printFormattedFloat(data[0][2][index] - dataOffsets[0][2], 5, 2);
  
  Serial.print(", AccX1: ");
  printFormattedFloat(data[0][3][index] - dataOffsets[0][3], 5, 2);
  Serial.print(", AccY1: ");
  printFormattedFloat(data[0][4][index] - dataOffsets[0][4], 5, 2);
  Serial.print(", AccZ1: ");
  printFormattedFloat(data[0][5][index] - dataOffsets[0][5], 5, 2);


  
  Serial.print(", MagX2: ");
  printFormattedFloat(data[1][0][index] - dataOffsets[1][0], 5, 2);
  Serial.print(", MagY2: ");
  printFormattedFloat(data[1][1][index] - dataOffsets[1][1], 5, 2);
  Serial.print(", MagZ2: ");
  printFormattedFloat(data[1][2][index] - dataOffsets[1][2], 5, 2);
  
  Serial.print(", AccX2: ");
  printFormattedFloat(data[1][3][index] - dataOffsets[1][3], 5, 2);
  Serial.print(", AccY2: ");
  printFormattedFloat(data[1][4][index] - dataOffsets[1][4], 5, 2);
  Serial.print(", AccZ2: ");
  printFormattedFloat(data[1][5][index] - dataOffsets[1][5], 5, 2);


  
  Serial.print(", MagX3: ");
  printFormattedFloat(data[2][0][index] - dataOffsets[2][0], 5, 2);
  Serial.print(", MagY3: ");
  printFormattedFloat(data[2][1][index] - dataOffsets[2][1], 5, 2);
  Serial.print(", MagZ3: ");
  printFormattedFloat(data[2][2][index] - dataOffsets[2][2], 5, 2);
  
  Serial.print(", AccX3: ");
  printFormattedFloat(data[2][3][index] - dataOffsets[2][3], 5, 2);
  Serial.print(", AccY3: ");
  printFormattedFloat(data[2][4][index] - dataOffsets[2][4], 5, 2);
  Serial.print(", AccZ3: ");
  printFormattedFloat(data[2][5][index] - dataOffsets[2][5], 5, 2);
  
  Serial.println("");
}

void writeSensorDataToSD(){  //will have to re-write this...
  /*
  String dataString = "";
  for(int k = 0; k < WINDOW_SIZE; k++){
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 6; j++){
          dataString += String(data[i][j][k] - dataOffsets[i][j]);
          if(i != 2 || j != 5){
            dataString += ",";
          }
      }
    }  
    dataString += "\n";
  }*/
  File dataFile = SD.open(FILENAME, FILE_APPEND);
  if(dataFile){
    dataFile.print(modDataString);
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}




void readDataFromSD(){
  File dataFile = SD.open(FILENAME, FILE_READ);
  if(dataFile){
     while(dataFile.available()){
        Serial.write(dataFile.read()); 
     }
     dataFile.close();
  }
  else{
      Serial.println("Error opening file to read");  
  }
}

void calibrateSensors(int iterations){
  int calibrationIndex = 0;
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 6; j++){
      dataOffsets[i][j] = 0;
    }  
  }
  state = State::SENSOR1_COLLECTION;
  for(int i = 0; i < iterations*3; i++){
   switch (state) {
      case State::SENSOR1_COLLECTION:
        collectSensorData(calibrationIndex);
        dataOffsets[0][0] += data[0][0][0];
        dataOffsets[0][1] += data[0][1][0];
        dataOffsets[0][2] += data[0][2][0];
        dataOffsets[0][3] += data[0][3][0];
        dataOffsets[0][4] += data[0][4][0];
        dataOffsets[0][5] += data[0][5][0];

        switchSensorTo(State::SENSOR2_COLLECTION);
        break;
      case State::SENSOR2_COLLECTION:
        collectSensorData(calibrationIndex);
        dataOffsets[1][0] += data[1][0][0];
        dataOffsets[1][1] += data[1][1][0];
        dataOffsets[1][2] += data[1][2][0];
        dataOffsets[1][3] += data[1][3][0];
        dataOffsets[1][4] += data[1][4][0];
        dataOffsets[1][5] += data[1][5][0];
        switchSensorTo(State::SENSOR3_COLLECTION);
        break;
      case State::SENSOR3_COLLECTION:
        collectSensorData(calibrationIndex);
        dataOffsets[2][0] += data[2][0][0];
        dataOffsets[2][1] += data[2][1][0];
        dataOffsets[2][2] += data[2][2][0];
        dataOffsets[2][3] += data[2][3][0];
        dataOffsets[2][4] += data[2][4][0];
        dataOffsets[2][5] += data[2][5][0];
        switchSensorTo(State::SENSOR1_COLLECTION);
        break;
    };  
//    for(int i = 0; i < 3; i++){
//      for(int j = 0; j < 6; j++){
//        Serial.print(data[i][j]);
//        Serial.print(", ");
//      }  
//    }
//    Serial.println("");
//    delay(1);
  }
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 6; j++){
      dataOffsets[i][j] = dataOffsets[i][j] / iterations;
      Serial.print(dataOffsets[i][j]);
      if(j != 5 || i != 2){
        Serial.print(", ");
      }
    }  
  }
//  Serial.println("");
  Serial.println("\nSensors Calibrated");
}

//This function is from the ICM_20948 starter code
void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  float aval = abs(val);
  if (val < 0)
  {
    Serial.print("-");
  }
  else
  {
    Serial.print(" ");
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      Serial.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    Serial.print(-val, decimals);
  }
  else
  {
    Serial.print(val, decimals);
  }
}
