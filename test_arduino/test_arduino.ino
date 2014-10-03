const int pin=0; //analogue pin 0 sur arduino
int mesure=0;

void setup()   { 

Serial.begin(115200); 

} 

void loop(){ 
  
mesure=analogRead(pin);
Serial.print(mesure);
Serial.print(" ");
Serial.print(mesure)
Serial.print(" ")
Serial.println(mesure);
delay(100);

} 
