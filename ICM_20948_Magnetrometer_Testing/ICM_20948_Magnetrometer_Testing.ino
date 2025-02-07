/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 *
 * Please see License.md for the license information.
 *
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include <ICM_20948.h> // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU

//#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2     // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL_SENSOR1 1
#define AD0_VAL_SENSOR2 0
#define CALIBRATE 'c'


#ifdef USE_SPI
ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object
#else
ICM_20948_I2C myICM1; // Otherwise create an ICM_20948_I2C object
ICM_20948_I2C myICM2;
#endif
float ICM1_Mag_X_offset,ICM1_Mag_Y_offset,ICM1_Mag_Z_offset = 0;
float ICM1_Acc_X_offset,ICM1_Acc_Y_offset,ICM1_Acc_Z_offset = 0;
float ICM2_Mag_X_offset,ICM2_Mag_Y_offset,ICM2_Mag_Z_offset = 0;
int wait_tries_sensor1, wait_tries_sensor2 = 0;
char serial_command = 0;

void calibrate_sensors(){
  //Standardize baseline for Sensors
  ICM2_Mag_X_offset = 0;
  ICM2_Mag_Y_offset = 0;
  ICM2_Mag_Z_offset = 0;
  ICM1_Acc_X_offset = 0;
  ICM1_Acc_Y_offset = 0;
  ICM1_Acc_Z_offset = 0;
  ICM1_Mag_X_offset = 0;
  ICM1_Mag_Y_offset = 0;
  ICM1_Mag_Z_offset = 0;
  int iterations = 300;
  int successful_iterations = 0;
  while(successful_iterations < iterations){
    if (myICM1.dataReady() && myICM2.dataReady())
    {
      myICM1.getAGMT();
      myICM2.getAGMT();
      ICM1_Mag_X_offset += myICM1.magX();
      ICM1_Mag_Y_offset += myICM1.magY();
      ICM1_Mag_Z_offset += myICM1.magZ();

      ICM1_Acc_X_offset += myICM1.accX();
      ICM1_Acc_Y_offset += myICM1.accY();
      ICM1_Acc_Z_offset += myICM1.accZ();
      
      ICM2_Mag_X_offset += myICM2.magX();
      ICM2_Mag_Y_offset += myICM2.magY();
      ICM2_Mag_Z_offset += myICM2.magZ();
      printFormattedFloat(ICM2_Mag_X_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM2_Mag_Y_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM2_Mag_Z_offset, 5, 2);
  Serial.print("\n");
      successful_iterations++;
    }
    
  }
  ICM1_Mag_X_offset = ICM1_Mag_X_offset / (successful_iterations * 1.0);
  ICM1_Mag_Y_offset = ICM1_Mag_Y_offset / (successful_iterations * 1.0);
  ICM1_Mag_Z_offset = ICM1_Mag_Z_offset / (successful_iterations * 1.0);
  
  ICM1_Acc_X_offset = ICM1_Acc_X_offset / (successful_iterations * 1.0);
  ICM1_Acc_Y_offset = ICM1_Acc_Y_offset / (successful_iterations * 1.0);
  ICM1_Acc_Z_offset = ICM1_Acc_Z_offset / (successful_iterations * 1.0);
  
  ICM2_Mag_X_offset = ICM2_Mag_X_offset / (successful_iterations * 1.0);
  ICM2_Mag_Y_offset = ICM2_Mag_Y_offset / (successful_iterations * 1.0);
  ICM2_Mag_Z_offset = ICM2_Mag_Z_offset / (successful_iterations * 1.0);
  
  Serial.print("Sensor Offsets: ");
  printFormattedFloat(ICM1_Mag_X_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM1_Mag_Y_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM1_Mag_Z_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM1_Acc_X_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM1_Acc_Y_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM1_Acc_Z_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM2_Mag_X_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM2_Mag_Y_offset, 5, 2);
  Serial.print(", ");
  printFormattedFloat(ICM2_Mag_Z_offset, 5, 2);
  Serial.print("\n");
  return;
}

void setup()
{

  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT)
  {
  };

#ifdef USE_SPI
  SPI_PORT.begin();
#else
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
#endif

  //myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  bool initialized = false;
  while (!initialized)
  {

#ifdef USE_SPI
    myICM.begin(CS_PIN, SPI_PORT);
#else
    myICM1.begin(WIRE_PORT, AD0_VAL_SENSOR1);
    myICM2.begin(WIRE_PORT, AD0_VAL_SENSOR2);

#endif

    SERIAL_PORT.print(F("Initialization of sensor 1 returned: "));
    SERIAL_PORT.println(myICM1.statusString());
    SERIAL_PORT.print(F("Initialization of sensor 2 returned: "));
    SERIAL_PORT.println(myICM2.statusString());
    if (myICM1.status != ICM_20948_Stat_Ok || myICM2.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
  delay(1000);
  calibrate_sensors();

}

void loop()
{
    if(Serial.available())
  {
    serial_command = Serial.read();
    switch (serial_command){
      case CALIBRATE:
        delay(300);
        calibrate_sensors();
        break;   
    }
  }

  if (myICM1.dataReady())
  {
    myICM1.getAGMT();    
    SERIAL_PORT.print("MagX1: ");
    printFormattedFloat(myICM1.magX() - ICM1_Mag_X_offset, 5, 2);
    SERIAL_PORT.print(", MagY1: ");
    printFormattedFloat(myICM1.magY() - ICM1_Mag_Y_offset, 5, 2);
    SERIAL_PORT.print(", MagZ1: ");
    printFormattedFloat(myICM1.magZ() - ICM1_Mag_Z_offset, 5, 2);
    
    SERIAL_PORT.print(", AccX1: ");
    printFormattedFloat(myICM1.accX() - ICM1_Acc_X_offset, 5, 2);
    SERIAL_PORT.print(", AccY1: ");
    printFormattedFloat(myICM1.accY() - ICM1_Acc_Y_offset, 5, 2);
    SERIAL_PORT.print(", AccZ1: ");
    printFormattedFloat(myICM1.accZ() - ICM1_Acc_Z_offset, 5, 2);
    Serial.print(", ");
    delay(20);
  }
  else
  {
//    SERIAL_PORT.println("Sensor 1 waiting for data");
    delay(500);
    wait_tries_sensor1++;
    if(wait_tries_sensor1 >= 10){
      wait_tries_sensor1 = 0;
      myICM1.begin(WIRE_PORT, AD0_VAL_SENSOR1);
    }
  }
  delay(20);
  if (myICM2.dataReady())
  {
    myICM2.getAGMT();   
    SERIAL_PORT.print("MagX2: ");
    printFormattedFloat(myICM2.magX()- ICM2_Mag_X_offset, 5, 2);
    SERIAL_PORT.print(", MagY2: ");
    printFormattedFloat(myICM2.magY()- ICM2_Mag_Y_offset, 5, 2);
    SERIAL_PORT.print(", MagZ2: ");
    printFormattedFloat(myICM2.magZ()- ICM2_Mag_Z_offset, 5, 2);
    Serial.print("\n");
    delay(20);
  }
  else
  {
    SERIAL_PORT.println("Sensor 2 waiting for data");
    delay(500);
    wait_tries_sensor2++;
    if(wait_tries_sensor2 >= 10){
      wait_tries_sensor2 = 0;
      myICM2.begin(WIRE_PORT, AD0_VAL_SENSOR1);
    }
  }
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
