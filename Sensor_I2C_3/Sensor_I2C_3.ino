#include <ICM_20948.h>
#include <SPI.h>
//#include <SD.h>

#define SERIAL_PORT Serial
#define WIRE_PORT Wire

#define AD0 1

unsigned long startCycleMillis, stopCycleMillis;

ICM_20948_I2C currentICM;
enum State {
  SENSOR1_COLLECTION,
  SENSOR2_COLLECTION,
  SENSOR3_COLLECTION,
  STOPPED,
  PRINT,
  SD_WRITE
};
State state = State::STOPPED;

float data[3][6]; //sensor, readings
float dataOffsets[3][6];


void setup()
{
  SERIAL_PORT.begin(115200);
  
  while (!SERIAL_PORT);

  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  initializeSensors();
  delay(50);
  calibrateSensors(300);

//   Serial.print("Initializing SD card...");
//
//  // see if the card is present and can be initialized:
//  if (!SD.begin(8)) { //Sparkfun SD Shield chipSelect = 8
//    Serial.println("Card failed, or not present");
//    // don't do anything more:
//    while (1);
//  }
//  Serial.println("card initialized.");
//  
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
      collectSensorData();
      switchSensorTo(State::SENSOR2_COLLECTION);
      break;
    case State::SENSOR2_COLLECTION:
      collectSensorData();
      switchSensorTo(State::SENSOR3_COLLECTION);
      break;
    case State::SENSOR3_COLLECTION:
      collectSensorData();
      state = State::PRINT;      
      break;
    case State::SD_WRITE:
      //write data to the SD card
//      switchSensorTo(State::SENSOR1_COLLECTION);
//      writeSensorDataToSD();
//      stopCycleMillis = millis();
//      float samplingHz = 1.0/((stopCycleMillis-startCycleMillis)/1000.0);
//      Serial.println(samplingHz);
      break;
    case State::PRINT:
      printSensorData();
      switchSensorTo(State::SENSOR1_COLLECTION);
      break;    
  };
}

void handleUserInput(){
  String input = Serial.readString();
  if (input == "c"){
    calibrateSensors(300);
  }
  else if(input == "r"){
    initializeSensors();
  }
}

void initializeSensors(){
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);

  bool initialized = false;
  while (!initialized)
  {
    currentICM.begin(WIRE_PORT, AD0);

    SERIAL_PORT.print(F("Initialization current sensor: "));
    SERIAL_PORT.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }

  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);

  initialized = false;
  while (!initialized)
  {
    currentICM.begin(WIRE_PORT, AD0);

    SERIAL_PORT.print(F("Initialization current sensor: "));
    SERIAL_PORT.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);

  initialized = false;
  while (!initialized)
  {
    currentICM.begin(WIRE_PORT, AD0);

    SERIAL_PORT.print(F("Initialization current sensor: "));
    SERIAL_PORT.println(currentICM.statusString());
    if (currentICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
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
    default:
      Serial.println("NOT IN A STATE");
      break;
  }
}

void switchSensorTo(State newState) {
  switch (newState) {
    case State::SENSOR1_COLLECTION:
//            Serial.println("Switched to sensor1");
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      break;
    case State::SENSOR2_COLLECTION:
//            Serial.println("Switched to sensor2");
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      break;
    case State::SENSOR3_COLLECTION:
//            Serial.println("Switched to sensor3");
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      break;
  };
  state = newState;
  delay(7);
}

void collectSensorData() {
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
      data[0][0] = currentICM.magX();  
      data[0][1] = currentICM.magY();  
      data[0][2] = currentICM.magZ();  
      data[0][3] = currentICM.accX();  
      data[0][4] = currentICM.accY();  
      data[0][5] = currentICM.accZ(); 
      break;
    case State::SENSOR2_COLLECTION:
      data[1][0] = currentICM.magX();  
      data[1][1] = currentICM.magY();  
      data[1][2] = currentICM.magZ();  
      data[1][3] = currentICM.accX();  
      data[1][4] = currentICM.accY();  
      data[1][5] = currentICM.accZ(); 
      break;
    case State::SENSOR3_COLLECTION:
      data[2][0] = currentICM.magX();  
      data[2][1] = currentICM.magY();  
      data[2][2] = currentICM.magZ();  
      data[2][3] = currentICM.accX();  
      data[2][4] = currentICM.accY();  
      data[2][5] = currentICM.accZ(); 
      break;
  }
}

void printSensorData() {
  SERIAL_PORT.print("MagX1: ");
  printFormattedFloat(data[0][0] - dataOffsets[0][0], 5, 2);
  SERIAL_PORT.print(", MagY1: ");
  printFormattedFloat(data[0][1] - dataOffsets[0][1], 5, 2);
  SERIAL_PORT.print(", MagZ1: ");
  printFormattedFloat(data[0][2] - dataOffsets[0][2], 5, 2);
  
  SERIAL_PORT.print(", AccX1: ");
  printFormattedFloat(data[0][3] - dataOffsets[0][3], 5, 2);
  SERIAL_PORT.print(", AccY1: ");
  printFormattedFloat(data[0][4] - dataOffsets[0][4], 5, 2);
  SERIAL_PORT.print(", AccZ1: ");
  printFormattedFloat(data[0][5] - dataOffsets[0][5], 5, 2);


  
  SERIAL_PORT.print(", MagX2: ");
  printFormattedFloat(data[1][0] - dataOffsets[1][0], 5, 2);
  SERIAL_PORT.print(", MagY2: ");
  printFormattedFloat(data[1][1] - dataOffsets[1][1], 5, 2);
  SERIAL_PORT.print(", MagZ2: ");
  printFormattedFloat(data[1][2] - dataOffsets[1][2], 5, 2);
  
  SERIAL_PORT.print(", AccX2: ");
  printFormattedFloat(data[1][3] - dataOffsets[1][3], 5, 2);
  SERIAL_PORT.print(", AccY2: ");
  printFormattedFloat(data[1][4] - dataOffsets[1][4], 5, 2);
  SERIAL_PORT.print(", AccZ2: ");
  printFormattedFloat(data[1][5] - dataOffsets[1][5], 5, 2);


  
  SERIAL_PORT.print(", MagX3: ");
  printFormattedFloat(data[2][0] - dataOffsets[2][0], 5, 2);
  SERIAL_PORT.print(", MagY3: ");
  printFormattedFloat(data[2][1] - dataOffsets[2][1], 5, 2);
  SERIAL_PORT.print(", MagZ3: ");
  printFormattedFloat(data[2][2] - dataOffsets[2][2], 5, 2);
  
  SERIAL_PORT.print(", AccX3: ");
  printFormattedFloat(data[2][3] - dataOffsets[2][3], 5, 2);
  SERIAL_PORT.print(", AccY3: ");
  printFormattedFloat(data[2][4] - dataOffsets[2][4], 5, 2);
  SERIAL_PORT.print(", AccZ3: ");
  printFormattedFloat(data[2][5] - dataOffsets[2][5], 5, 2);
  
  Serial.println("");
}

//void writeSensorDataToSD(){
//  String dataString = "";
//  for(int i=0; i<3; i++){
//    for(int j=0; j<6; j++){
//      dataString += String(data[i][j] - dataOffsets[i][j]);
//      if(i != 2 && j != 5){
//        dataString += ",";
//      }
//      else{
//        dataString += "\n";
//      }
//    }  
//  }
//  File dataFile = SD.open("datalog.csv", FILE_WRITE);
//  if(dataFile){
//    dataFile.println(dataString);
//    dataFile.close();
//  }
//}

void calibrateSensors(int iterations){
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 6; j++){
      dataOffsets[i][j] = 0;
    }  
  }
  for(int i = 0; i < iterations*3; i++){
   switch (state) {
      case State::STOPPED:
        state = State::SENSOR1_COLLECTION;
        break;
      case State::SENSOR1_COLLECTION:
        collectSensorData();
        dataOffsets[0][0] += data[0][0];
        dataOffsets[0][1] += data[0][1];
        dataOffsets[0][2] += data[0][2];
        dataOffsets[0][3] += data[0][3];
        dataOffsets[0][4] += data[0][4];
        dataOffsets[0][5] += data[0][5];

        switchSensorTo(State::SENSOR2_COLLECTION);
        break;
      case State::SENSOR2_COLLECTION:
        collectSensorData();
        dataOffsets[1][0] += data[1][0];
        dataOffsets[1][1] += data[1][1];
        dataOffsets[1][2] += data[1][2];
        dataOffsets[1][3] += data[1][3];
        dataOffsets[1][4] += data[1][4];
        dataOffsets[1][5] += data[1][5];
        switchSensorTo(State::SENSOR3_COLLECTION);
        break;
      case State::SENSOR3_COLLECTION:
        collectSensorData();
        dataOffsets[2][0] += data[2][0];
        dataOffsets[2][1] += data[2][1];
        dataOffsets[2][2] += data[2][2];
        dataOffsets[2][3] += data[2][3];
        dataOffsets[2][4] += data[2][4];
        dataOffsets[2][5] += data[2][5];
        switchSensorTo(State::SENSOR1_COLLECTION);
        break;
    };  
  }
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 6; j++){
      dataOffsets[i][j] = dataOffsets[i][j] / iterations;
//      Serial.print(dataOffsets[i][j]);
//      Serial.print(", ");
    }  
  }
//  Serial.println("");
  switchSensorTo(State::SENSOR1_COLLECTION);
  Serial.println("Sensors Calibrated");
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  float aval = abs(val);
  if (val < 0)
  {
    SERIAL_PORT.print("-");
  }
  else
  {
    SERIAL_PORT.print(" ");
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
      SERIAL_PORT.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    SERIAL_PORT.print(-val, decimals);
  }
  else
  {
    SERIAL_PORT.print(val, decimals);
  }
}
