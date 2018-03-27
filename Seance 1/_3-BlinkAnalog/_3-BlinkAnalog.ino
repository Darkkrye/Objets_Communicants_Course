void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
    // Clignotement de la led : 
    // La valeur 16 équivaut à la constante LOW du digitaleWrite()
    // Idem pour 255 avec High
    analogWrite(LED_BUILTIN, 16);   
    delay(1000);
    analogWrite(LED_BUILTIN, 255);   
    delay(1000);
    
         
}
