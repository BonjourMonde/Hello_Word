#include <TimerOne.h>
#define PWMA 9
#define DIR_A 12
#define DIR_B 13
int pos=0;
int TEMP=2;//millisec pour echantionnage
int last_pos=0;
int last_millis=0;
float Vc=0;
int Vmax=10;
float Ac=0;
int eps_v=0;
int eps_i=0;
float PWM_per=8388.608/4;
double vitesse=0;

int Integ_v=0;
int Integ_i=0;
int Ic=0;
int Uc=0;

boolean start=false;

void setup(){
  pinMode(3,INPUT);
  pinMode(2,INPUT);
  attachInterrupt(0,encodeur_A,CHANGE);
  attachInterrupt(1,encodeur_B,CHANGE);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWMA, OUTPUT);
  digitalWrite(PWMA,LOW);
  Timer1.initialize(1000);  //Durée(timer1) en microsecondes (1Khz)
  Timer1.attachInterrupt(cal_vitesse,5000);
  Timer1.stop();

  Serial.begin(115200);
}
int Kpv=10; //P de vitesse
int Kiv=1; // I de vitesse
int Kdv=0; //D de vitesse
int Kpi=5;// P de current
int Kii=1;// I de current
int Kdi=0; //D de current
void loop(){
  /*if(millis()-last_millis>TEMP/2){
   cmd=Pid(cmd,vitesse_req,vitesse);
   }*/
   // if not true, we stop the motor
  if(Serial.available()>0){
    if(Serial.parseInt()==-1){
      start=false;
      Serial.flush();
    }
    // if ture, we set the value PID as received and start the motor
    else{
      Kpv=Serial.parseInt();
      Kiv=Serial.parseInt();
      Kdv=Serial.parseInt();
      Kpi=Serial.parseInt();
      Kii=Serial.parseInt();
      Kdi=Serial.parseInt();
      start=true;
      
    }

  }
  if(start==true){
    movement(0.2,5000,1000);
    eps_v=Vc-vitesse;
    Ic=Kpv*eps_v+Kiv*Integ_v;
    int I=(analogRead(A0)-512);

    eps_i=Ic-I;
    if(millis()-last_millis>TEMP){
      Vc += Ac;
      Vc=constrain(Vc,-Vmax,Vmax);

      Integ_v+=eps_v;
      Integ_i+=eps_i;
      Integ_v=constrain(Integ_v,-200,200);
      Integ_i=constrain(Integ_i,-200,200);
      last_millis=millis();
    }

    //Uc=Vc;
    Uc=Kpi*eps_i+Kii*Integ_i;

    Cmde_Mot(Uc);  
    //Serial.print(Vc);
    
    
    Serial.print(vitesse);Serial.print(" ");
    Serial.print(pos);Serial.print(" ");
    Serial.println(I);
    //Serial.print(constrain(Vc,-1024,1024));
    //Serial.print(",");

    //Serial.print("Position:");   
    //Serial.println(pos);
    // Serial.print();
    // Serial.print(",");
    // Serial.print("  Vitesse:");   
    //Serial.println(vitesse);
    delay(40);
  }
}
//void movement(int value){


/*if(Vc<0){
 if(abs(pos)<1000) Vc=-60;
 if(abs(pos)>=1000&&abs(pos)<2500) Vc=-value/1500*(abs(pos-1000))-6;
 if(abs(pos)>=2500&&abs(pos)<3500) Vc=-value;
 if(abs(pos)>=3500&&abs(pos)<5000) Vc=-value+value/1500*(abs(pos)-3500);
 if(abs(pos)>=5000) Vc=60;
 }
 if(Vc>0){
 if(abs(pos)>=5000) Vc=60;
 if(abs(pos)>=3500&&abs(pos)<5000) Vc=value/1500*(abs(pos)-5000)-6;
 if(abs(pos)>=2500&&abs(pos)<3500) Vc=value;
 if(abs(pos)>=1000&&abs(pos)<2500) Vc=value-value/1500*(abs(pos-2500));
 if(abs(pos)<1000) Vc=-60;
 }*/
// if(abs(pos)>5000&&abs(pos)<5110) Vc=value; // forme rectangle
// if(abs(pos)<2000) Vc=-value; 
//}
void cal_vitesse(){
  vitesse=pos-last_pos;
  last_pos=pos;
}
void encodeur_A(){
  if(digitalRead(3)==digitalRead(2)){
    pos++;
  }
  else {
    pos--;
  }

}
void encodeur_B(){
  if(digitalRead(3)==digitalRead(2)){
    pos--;
  }
  else {
    pos++;
  }
}

void Cmde_Mot_1(int value_1){
  digitalWrite(DIR_A,LOW);
  digitalWrite(DIR_B,HIGH);
  Timer1.pwm(PWMA,value_1,PWM_per);
}
void Cmde_Mot_2(int value_2){
  digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,LOW);
  Timer1.pwm(PWMA,value_2,PWM_per);
}
void Cmde_Mot(int cmd)
{
  if(cmd<0)  Cmde_Mot_1(constrain(abs(cmd),0,1024));
  if(cmd>=0) Cmde_Mot_2(constrain(abs(cmd),0,1024));
}
/*int Pid(int command, int targetValue, int currentValue){             
 float pidTerm = 0;                                                            
 int error=0;                                  
 static int last_error=0;                             
 error = abs(targetValue) - abs(currentValue); 
 pidTerm = (Kp * error) + (Kd * (error - last_error));                            
 last_error = error;
 return constrain(command + pidTerm, 880, 1024);
 }*/
int movement(float Amax,int Xmax,int Xmin){

  if(abs(pos)>Xmax) Ac=Amax; // forme rectangle
  if(abs(pos)<Xmin) Ac=-Amax; 

}
/*if(millis()-lastMillis<=millis1*N){
 for(int j=1;j<M;j++){
 Vc=value*j/M;
 }
 }
 else if(millis()-lastMillis>millis1*N&&millis()-lastMillis<millis2*N){
 Vc=value;
 }
 else if(millis()-lastMillis>millis2*N&&millis()-lastMillis<millis3*N){
 for(int j=1;j<M;j++) Vc=value-value*j/M;
 }
 else if(millis()-lastMillis>millis3*N&&millis()-lastMillis<millis4*N){
 Vc=0;
 
 }
 //2ere phase
 else if(millis()-lastMillis>millis4*N&&millis()-lastMillis<millis5*N){
 for(int j=1;j<M;j++){
 Vc=-value*j/M;
 }
 }
 else if(millis()-lastMillis>millis5*N&&millis()-lastMillis<millis6*N){
 Vc=-value;
 }
 else if(millis()-lastMillis>millis6*N&&millis()-lastMillis<millis7*N){
 for(int j=1;j<M;j++){  
 Vc=value-value*j/M;
 }
 }
 else if(millis()-lastMillis>millis7*N&&millis()-lastMillis<millis8*N){
 Vc=0;
 
 }
 if(millis()-lastMillis==millis8*N)
 lastMillis=millis();*/






