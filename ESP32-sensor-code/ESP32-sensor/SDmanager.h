#define SD_CHIP_SELECT 5
#define FILENAME "/test.csv"

extern String modDataString;
extern float dataOffsets[3][6];
extern char *md5str;

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
    dataFile.print(modDataString);
    dataFile.close();
  }
  else{
    Serial.println("Error opening file to write");  
  }
}

void writeRawSensorDataToSD(float rawdata[3][6][WINDOW_SIZE], int classification){
  String rawDataString = "";
  for (int k =1; k < WINDOW_SIZE; k++){
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
  String fileTransferString = "";
  if(dataFile){
     while(dataFile.available()){
      fileTransferString.concat(dataFile.read());
//        Serial.write(dataFile.read()); 
     }
     //generate the MD5 hash for our string
     int transferLen = fileTransferString.length() + 1;
     char charArrayFile[transferLen];
     fileTransferString.toCharArray(charArrayFile,transferLen);
     unsigned char* hash=MD5::make_hash(charArrayFile);
     //generate the digest (hex encoding) of our hash
     md5str = MD5::make_digest(hash, 16);
     free(hash);      
     Serial.print(fileTransferString);
     dataFile.close();
  }
  else{
      Serial.println("Error opening file to read");  
  }
}
