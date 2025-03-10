#define SD_CHIP_SELECT 5
#define FILENAME "/test.csv"

extern String modDataString;

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

void writeSensorDataToSD(){  //will have to re-write this...
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
