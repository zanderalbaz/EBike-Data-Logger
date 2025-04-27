#define SD_CHIP_SELECT 5
#define FILENAME "/test.csv"
#include <MD5.h>
#include <SD.h>
#define FILE_HASH_BUFFSIZE 32768
/*
 * This code and the MD5 library is found here: https://github.com/tzikis/ArduinoMD5/blob/master/examples/MD5_Hash/MD5_Hash.ino

This is en example of how to use my MD5 library. It provides two
easy-to-use methods, one for generating the MD5 hash, and the second
one to generate the hex encoding of the hash, which is frequently used.
*/
void setup()
{
    Serial.begin(115200);
    initializeSD();
    delay(1000);
//    readDataFromSD();
//  writeSensorDataToSD();
  Serial.println("Starting Loop");
}

void loop()
{
  Serial.println("This is noise");
  while(Serial.available()){
    handleUserInput();
  }
  delay(300);
}

void handleUserInput(){
  String input = Serial.readString();
  if(input == "t"){
    readDataFromSD();  
  }  
}
void initializeSD(){
  Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CHIP_SELECT)) {
      Serial.println("Card failed, or not present");
      while(true);
  }
  Serial.println("card initialized.");
}


void readDataFromSD(){
  File dataFile = SD.open(FILENAME, FILE_READ);

  if(dataFile){
     Serial.print(":start:");
     while(dataFile.available()){
        int bytes_available = dataFile.available();
        int bytesToRead = 0;
        if(bytes_available < FILE_HASH_BUFFSIZE){
          bytesToRead = bytes_available;
        }
        else {
          bytesToRead = FILE_HASH_BUFFSIZE;
        }
        char* fileTransferString = (char*) malloc(FILE_HASH_BUFFSIZE+1);
        int numBytesRead = dataFile.readBytes(fileTransferString, bytesToRead);
        fileTransferString[numBytesRead] = '\0';
        unsigned char* hash=MD5::make_hash(fileTransferString);
        char *md5str = MD5::make_digest(hash, 16);
        Serial.print(":hash:");
        Serial.println(md5str);
        Serial.print(":data:");
        Serial.println(fileTransferString);
        Serial.println("\r\n\r\n");
      
        free(hash);
        free(md5str);
        free(fileTransferString);
     }
     dataFile.close();
     Serial.print(":end:");
  }
  else{
      Serial.println("Error opening file to read");  
  }
}

void writeSensorDataToSD(){
  File dataFile = SD.open(FILENAME, FILE_APPEND);
  if(dataFile){
    
//    dataFile.print(epochSeconds);
//    dataFile.print(", ");
    for(int i =0; i < 1000; i++){
      dataFile.print("-0.16, -14.11, 14.99, 4.23, -0.55, -62.16, 2.94, 8.84, 0.12, -2.63, 23.17, 3.47, -1.62, -457.24, 264.93, 93.68, -7.49, -298.66, 147.14, 48.73, -1.14, -511.72, 951.66, 173.86, -0.62, -14.30, 8.35, 3.72, -0.51, -1.92, 1.08, 0.73, -0.12, -2.50, 1.55, 0.81, 3.03, -136.95, 261.97, 62.11, -11.31, -100.74, 13.52, 19.28, 2.91, -202.65, 402.82, 71.24, -0.44, -11.97, 7.08, 3.19, -0.39, -1.86, 1.44, 0.80, -0.37, -1.67, 1.18, 0.74, -0.38, -190.56, 213.74, 60.48, -6.92, -54.44, 13.92, 11.18, 2.12, -32.34, 114.63, 24.85, 2\n");
    }
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}


void removeSDFile(){
  SD.remove(FILENAME);
  if(!SD.exists(FILENAME)){
    Serial.println("File was deleted sucessfully");
  }
}
