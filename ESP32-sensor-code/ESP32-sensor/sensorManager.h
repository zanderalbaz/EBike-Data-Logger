#define SENSOR1_AD0_PIN 15
#define SENSOR2_AD0_PIN 2
#define SENSOR3_AD0_PIN 4

#define AD0 1

//extern enum State;
//extern state State;
extern float dataOffsets[3][6];

ICM_20948_I2C currentICM;

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
