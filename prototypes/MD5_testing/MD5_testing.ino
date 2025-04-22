#define SD_CHIP_SELECT 5
#define FILENAME "/test.csv"
#include <MD5.h>
#include <SD.h>
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
  //initialize serial
//  Serial.begin(9600);
//  //give it a second
//  delay(1000);
//  //generate the MD5 hash for our string
//  unsigned char* hash=MD5::make_hash("zander alba");
//  //generate the digest (hex encoding) of our hash
//  char *md5str = MD5::make_digest(hash, 16);
//  free(hash);
//  //print it on our serial monitor
//  Serial.println(md5str);
//  //Give the Memory back to the System if you run the md5 Hash generation in a loop
//  free(md5str);
}

void loop()
{
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
//  String fileTransferString = "";
  if(dataFile){
     while(dataFile.available()){
//      fileTransferString.concat(dataFile.read());
        Serial.write(dataFile.read()); 
     }
     //generate the MD5 hash for our string
//     int transferLen = fileTransferString.length() + 1;
//     char charArrayFile[transferLen];
//     fileTransferString.toCharArray(charArrayFile,transferLen);
//     unsigned char* hash=MD5::make_hash(charArrayFile);
//     //generate the digest (hex encoding) of our hash
//     md5str = MD5::make_digest(hash, 16);
//     
//     free(hash);      
//     Serial.print(fileTransferString);
     dataFile.close();
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
