#define SD_CHIP_SELECT 5
#define FILENAME "/test.csv"
#define FILE_HASH_BUFFSIZE 32768


extern String modDataString;
extern float dataOffsets[3][6];
extern char *md5str;
extern unsigned long epochSeconds;

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

void writeSensorDataToSD(){
  File dataFile = SD.open(FILENAME, FILE_APPEND);
  if(dataFile){
//    dataFile.print(epochSeconds);
//    dataFile.print(", ");
    dataFile.print(modDataString);
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}

void writeRawSensorDataToSD(float rawdata[3][6][WINDOW_SIZE], int classification){
  String rawDataString = "";
  for (int k =0; k < WINDOW_SIZE; k++){
    rawDataString += String(k); //Write data index
    rawDataString += ", ";
    for (int i =0; i < 3; i++){
      for (int j =0; j < 6; j++){

          rawDataString += String(rawdata[i][j][k] - dataOffsets[i][j]);
          rawDataString += ", ";
      }
    }
    rawDataString += String(classification);
    rawDataString += "\n";
  }
  Serial.print(rawDataString);
  
  File dataFile = SD.open(FILENAME, FILE_APPEND);
  if(dataFile){
    dataFile.print(rawDataString);
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}



void readDataFromSD(){
  File dataFile = SD.open(FILENAME, FILE_READ);
  byte key = 0xE1;

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
        
        xorCipher(fileTransferString, numBytesRead+1, key);
      

        Serial.print(":hash:");
        Serial.println(md5str);
        Serial.print(":data:");
        Serial.write(fileTransferString, numBytesRead);
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

//Cipher reference:
//https://www.geeksforgeeks.org/xor-cipher/
void xorCipher(char* data, int dataLen, byte key) {
  for (int i = 0; i < dataLen; i++) {
    data[i] = data[i] ^ key; 
  }
}
