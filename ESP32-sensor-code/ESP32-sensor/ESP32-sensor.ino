#include <ICM_20948.h>
#include <SPI.h>
#include <SD.h>
#include "driver/rtc_io.h" //This is needed for deep sleep wakeup pin configuration

#define SENSOR1_AD0_PIN 15
#define SENSOR2_AD0_PIN 2
#define SENSOR3_AD0_PIN 4
#define WAKEUP_PIN GPIO_NUM_13

#define SD_CHIP_SELECT 5  //Sparkfun SD Shield chip select = 8

#define DELETE_FILE true
#define FILENAME "/test.csv"

#define WINDOW_SIZE 50

#define COLLECTION_MODE true

#define AD0 1

unsigned long startCycleMillis, stopCycleMillis;
float samplingHz = 0.0;
int dataIndex = 0;
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
  SLEEP
};

State state = State::STOPPED;

float data[3][6][WINDOW_SIZE]; //sensorNum, readings
float dataOffsets[3][6];


void setup()
{
  Serial.begin(115200);
  
  while(!Serial){};

  Wire.begin();
  Wire.setClock(400000);

  pinMode(SENSOR1_AD0_PIN, OUTPUT);
  pinMode(SENSOR2_AD0_PIN, OUTPUT);
  pinMode(SENSOR3_AD0_PIN, OUTPUT);

  initializeSensors();
//  calibrateSensors(50);

   Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  if(DELETE_FILE){
    SD.remove(FILENAME);
    if(!SD.exists(FILENAME)){
      Serial.println("File was deleted sucessfully");
    }
  }

  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, HIGH);
  rtc_gpio_pullup_dis(WAKEUP_PIN);
  rtc_gpio_pulldown_en(WAKEUP_PIN);
}

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
      startCycleMillis = millis();
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
      }else{
        dataIndex++;
        if(dataIndex % WINDOW_SIZE == 0){ //If our data window is full
          state = State::SD_WRITE;
        }
      }   
      break;
    case State::SD_WRITE:
      writeSensorDataToSD();
      stopCycleMillis = millis();
      samplingHz = 1.0/((stopCycleMillis - startCycleMillis) / 1000.0);
      Serial.println(samplingHz);
      switchSensorTo(State::SENSOR1_COLLECTION);
      break;
    case State::PRINT:
      printSensorData(0);
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
      esp_deep_sleep_start();
  };
}

void handleUserInput(){
  String input = Serial.readString();
  if (input == "c"){
    calibrateSensors(100);
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
    else
    {
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
    else
    {
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
    else
    {
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

void writeSensorDataToSD(){
  String dataString = "";
  for(int k=0; k<WINDOW_SIZE; k++){
    for(int i=0; i<3; i++){
      for(int j=0; j<6; j++){
          dataString += String(data[i][j][k] - dataOffsets[i][j]);
          if(i != 2 || j != 5){
            dataString += ",";
          }
        }
      }  
    }
  File dataFile = SD.open(FILENAME, FILE_APPEND);
  if(dataFile){
    dataFile.println(dataString);
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}

void readDataFromSD(){
  File dataFile = SD.open(FILENAME);
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
      Serial.print(", ");
    }  
  }
//  Serial.println("");
  switchSensorTo(State::SENSOR1_COLLECTION);
  Serial.println("Sensors Calibrated");
}

//This function is from the ICM starter code
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
