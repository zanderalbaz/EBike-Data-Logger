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


void removeSDFile(){
  SD.remove(FILENAME);
  if(!SD.exists(FILENAME)){
    Serial.println("File was deleted sucessfully");
  }
}
