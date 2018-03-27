String str; 
char c;

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  str = "";
  for(int i=0; i<10;i++){
    c = Serial1.read(); 
    str += c;
    
  }
  Serial.println(str);
  delay(100);
}
