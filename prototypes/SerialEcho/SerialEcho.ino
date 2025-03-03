String serialBuffer = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  while(Serial.available()){
    serialBuffer = Serial.readString();
    Serial.println(serialBuffer);
    serialBuffer = "";
  }

}
