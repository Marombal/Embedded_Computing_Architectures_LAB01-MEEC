#include <Arduino.h>

#define S1_pin 2
#define S2_pin 3
#define LED1 6
#define LED2 7
#define LED3 8 
#define LED4 9
#define LED5 10
#define LED6 11
#define LED7 12

int LED[6];
int LEDp;

typedef struct{
  int state, state_new;
  unsigned long tes, tis;
} fsm_t;

int S1, S2, prevS1, prevS2, save_tis = 0, save_tes = 0, flag = 0, flag_K = 0;

int K;

int mod2, mod3;

int last_state;

unsigned long interval_LED;

fsm_t fsm[10];

unsigned long interval, loop_micros, last_cycle;

void set_state(fsm_t &fsm, int state_new){
  if(fsm.state != state_new){
    fsm.state = state_new;
    fsm.tis = 0;                  /* tis = time in state    */
    fsm.tes = millis();           /* tes = time enter state */
  } 
}

void setup() {
  // put your setup code here, to run once:
  pinMode(S1_pin, INPUT); 
  pinMode(S2_pin, INPUT); 
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);

  for(int i = 0; i < 10; i++) set_state(fsm[i], 0);

  Serial.begin(115200);
  interval = 10; 
  K = 6;

  interval_LED = 2000;

  mod2 = 1;
  mod3 = 0;
  last_state = 0;
  
  for(int i = 0; i < K; i++) LED[i] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long now = millis();
  if(now - last_cycle > interval){
    loop_micros = micros();
    last_cycle = now;
    
    // Input Reading and update of "prev" variables
    prevS1 = S1;
    prevS2 = S2;
    S1 = !digitalRead(S1_pin);
    S2 = !digitalRead(S2_pin);

    unsigned long cur_time = millis(); // Only call millis once
    fsm[0].tis = cur_time - fsm[0].tes;
    if(fsm[0].state == 1){
      fsm[0].tis = cur_time - fsm[0].tes + save_tis;
    }
    fsm[1].tis = cur_time - fsm[1].tes;
    fsm[5].tis = cur_time - fsm[5].tes;
    fsm[2].tis = cur_time - fsm[2].tes;
    fsm[3].tis = cur_time - fsm[3].tes;
    fsm[4].tis = cur_time - fsm[4].tes;
    fsm[6].tis = cur_time - fsm[6].tes;
    fsm[7].tis = cur_time - fsm[7].tes;
    fsm[8].tis = cur_time - fsm[8].tes;
    fsm[9].tis = cur_time - fsm[9].tes;
    
    // State Machine 0 Process (Main)
    if((fsm[0].state == 0) && S2 && !prevS2){
      fsm[0].state_new = 1;
    }
    else if((fsm[0].state == 1) && S2 && !prevS2){
      save_tis = fsm[0].tis;
      save_tes = fsm[0].tes;
      fsm[0].state_new = 2;
    }
    else if((fsm[0].state == 1) && S1 && !prevS1){
      fsm[0].state_new = 6;
    }
    else if((fsm[0].state == 1) && fsm[0].tis > interval_LED){
      fsm[0].state_new = 1;
      fsm[0].tis = 0;
      fsm[0].tes = millis();
      save_tis = 0;
      Serial.print("K--");
      K--;
      flag_K = 1;
    }
    else if((fsm[0].state == 1) && K == 0){
      fsm[0].state_new = 4;
    }
    else if((fsm[0].state == 2) && S2 && !prevS2){
      flag = 1;
      fsm[0].state_new = 1;
    }
    else if((fsm[0].state == 2) && fsm[0].tis > 1000){
      fsm[0].state_new = 3;
    }
    else if((fsm[0].state == 2) && S1 && !prevS1){
      fsm[0].state_new = 6;
    }
    else if((fsm[0].state == 3) && S2 && !prevS2){
      flag = 1;
      fsm[0].state_new = 1;
    }
    else if((fsm[0].state == 3) && fsm[0].tis > 1000){
      fsm[0].state_new = 2;
    }
    else if((fsm[0].state == 3) && S1 && !prevS1){
      fsm[0].state_new = 6;
    }
    else if((fsm[0].state == 4) && S1 && !prevS1){
      fsm[0].state_new = 6;
    }
    else if((fsm[0].state == 6) && !S1 && prevS1 && fsm[0].tis < 3000){
      fsm[0].state_new = 0;
    }
    else if((fsm[0].state == 6) && !S1 && prevS1 && fsm[0].tis >= 3000){
      fsm[0].state_new = last_state;
    }
    /*WARNING*/
    else if((fsm[0].state == 5) && (fsm[1].state == 0)){ 
      fsm[0].state_new = last_state;
    }
    else if(fsm[1].state == 2){
      fsm[0].state_new = 5;
    }

    if((fsm[0].state != 5) && (fsm[0].state != 6)){
      last_state = fsm[0].state; // last_state -> guarda o estado em que ficou :)
    }

    // State Machine 5 Process (DoubleClick)
    if((fsm[5].state == 0) && S2 && !prevS2){
      fsm[5].state_new = 1;
    }
    else if((fsm[5].state == 1) && fsm[5].tis > 500){
      fsm[5].state_new = 0;
    }
    else if((fsm[5].state == 1) && S2 && !prevS2){
      fsm[5].state_new = 2;
    }
    else if(fsm[5].state == 2){
      fsm[5].state_new = 0;
    }

    // State Machine 1 Process (Long Click, Configuration Mode)
    if((fsm[1].state == 0) && S1 && !prevS1){
      fsm[1].state_new = 1;
    }
    else if((fsm[1].state == 1) && !S1 && prevS1 && fsm[1].tis < 3000){
      fsm[1].state_new = 0;
    }
    else if((fsm[1].state == 1) && !S1 && prevS1 && fsm[1].tis >= 3000){
      fsm[1].state_new = 2;
    }
    else if((fsm[1].state == 2) && S1 && !prevS1){
      fsm[1].state_new = 3;
    }
    else if((fsm[1].state == 3) && !S1 && prevS1 && fsm[1].tis < 3000){
      fsm[1].state_new = 2;
    }
    else if((fsm[1].state == 3) && !S1 && prevS1 && fsm[1].tis >= 3000){
      fsm[1].state_new = 0;
    }

    // State Machine 2 Process (Mode Select)
    if((fsm[2].state == 0) && fsm[1].state == 2){
      fsm[2].state_new = 1;
    }
    else if((fsm[2].state == 1) && S1 && !prevS1){
      fsm[2].state_new = 2;
    }
    else if((fsm[2].state == 1) && S2 && !prevS2){
      //
      if(interval_LED < 8000){
        interval_LED *= 2;
      }
      else{
        interval_LED = 1000;
      }
      fsm[2].state_new = 1;
    }
    else if((fsm[2].state == 1) && (fsm[1].state != 2) && (fsm[1].state != 3)){
      fsm[2].state_new = 0;
    }
    else if((fsm[2].state == 2) && S1 && !prevS1){
      fsm[2].state_new = 3;
    }
    else if((fsm[2].state == 2) && S2 && !prevS2){
      //
      if(mod2 < 3){
        mod2++;
      }
      else{
        mod2 = 1;
      }
      fsm[2].state_new = 2;
    }
    else if((fsm[2].state == 2) && (fsm[1].state != 2) && (fsm[1].state != 3)){
      fsm[2].state_new = 0;
    }
    else if((fsm[2].state == 3) && S1 && !prevS1){
      fsm[2].state_new = 1;
    }
    else if((fsm[2].state == 3) && S2 && !prevS2){
      mod3 = !mod3;
      fsm[2].state_new = 3;
    }
    else if((fsm[2].state == 3) && (fsm[1].state != 2) && (fsm[1].state != 3)){
      fsm[2].state_new = 0;
    }

    // State Machine 3,4 AND 6 Process (LED BLINKING /CONFIGURATION)
    /*3*/
    if((fsm[3].state == 0) && fsm[2].state == 1){
      fsm[3].state_new = 1;
    }
    else if((fsm[3].state == 1) && fsm[3].tis > 500){
      fsm[3].state_new = 2;
    }
    else if((fsm[3].state == 1) && fsm[2].state != 1){
      fsm[3].state_new = 0;
    }
    else if((fsm[3].state == 2) && fsm[3].tis > 500){
      fsm[3].state_new = 1;
    }
    else if((fsm[3].state == 2) && fsm[2].state != 1){
      fsm[3].state_new = 0;
    }
    /*4*/
    if((fsm[4].state == 0) && fsm[2].state == 2){
      fsm[4].state_new = 1;
    }
    else if((fsm[4].state == 1) && fsm[4].tis > 500){
      fsm[4].state_new = 2;
    }
    else if((fsm[4].state == 1) && fsm[2].state != 2){
      fsm[4].state_new = 0;
    }
    else if((fsm[4].state == 2) && fsm[4].tis > 500){
      fsm[4].state_new = 1;
    }
    else if((fsm[4].state == 2) && fsm[2].state != 2){
      fsm[4].state_new = 0;
    }
    /*6*/
    if((fsm[6].state == 0) && fsm[2].state == 3){
      fsm[6].state_new = 1;
    }
    else if((fsm[6].state == 1) && fsm[6].tis > 500){
      fsm[6].state_new = 2;
    }
    else if((fsm[6].state == 1) && fsm[2].state != 3){
      fsm[6].state_new = 0;
    }
    else if((fsm[6].state == 2) && fsm[6].tis > 500){
      fsm[6].state_new = 1;
    }
    else if((fsm[6].state == 2) && fsm[2].state != 3){
      fsm[6].state_new = 0;
    }

    // State Machine 7 Process (Present the current interval on LED7 aka LEDp)
    if((fsm[7].state == 0) && fsm[2].state == 1){
      fsm[7].state_new = 1;
    }
    else if((fsm[7].state == 1) && fsm[7].tis >= interval_LED){
      fsm[7].state_new = 2;
    }
    else if((fsm[7].state == 1) && fsm[2].state != 1){
      fsm[7].state_new = 0;
    }
    else if((fsm[7].state == 2) && fsm[7].tis >= interval_LED){
      fsm[7].state_new = 1;
    }
    else if((fsm[7].state == 2) && fsm[2].state != 1){
      fsm[7].state_new = 0;
    }

    // State Machine 8 Process (mod3 controller. The end of the time....)
    if((fsm[8].state == 0) && fsm[0].state == 4){
      fsm[8].state_new = 1;
    }
    else if((fsm[8].state == 1) && mod3 == 0){
      fsm[8].state_new = 2;
    }
    else if((fsm[8].state == 1) && mod3 == 1){
      fsm[8].state_new = 3;
    }
    else if((fsm[8].state == 2) && fsm[0].state != 4){
      fsm[8].state_new = 0;
    }
    else if((fsm[8].state == 3) && fsm[0].state != 4){
      fsm[8].state_new = 0;
    }
    else if((fsm[8].state == 3) && fsm[8].tis >= 250){
      fsm[8].state_new = 4;
    }
    else if((fsm[8].state == 4) && fsm[0].state != 4){
      fsm[8].state_new = 0;
    }
    else if((fsm[8].state == 4) && fsm[8].tis >= 250){
      fsm[8].state_new = 3;
    }

    // State Machine 9 Process (mod2 controller. The led that will be switch off will....)
    if((fsm[9].state == 0) && fsm[0].state == 1 && mod2 == 2){
      fsm[9].state_new = 1;
    }
    else if((fsm[9].state == 1) && fsm[0].state == 1 && mod2 == 3){
      fsm[9].state_new = 4;
    }
    else if((fsm[9].state == 1) && fsm[0].state != 1){
      fsm[9].state_new = 0;
    }
    else if((fsm[9].state == 1) && fsm[0].tis >= interval_LED/2){
      fsm[9].state_new = 2;
    }
    else if((fsm[9].state == 2) && fsm[0].state != 1){
      fsm[9].state_new = 0;
    }
    else if((fsm[9].state == 2) && fsm[9].tis >= 500){
      fsm[9].state_new = 3;
    }
    else if((fsm[9].state == 2) && flag_K == 1){
      flag_K = 0;
      fsm[9].state_new = 1;
    }
    else if((fsm[9].state == 3) && fsm[0].state != 1){
      fsm[9].state_new = 0;
    }
    else if((fsm[9].state == 3) && fsm[9].tis >= 500){
      fsm[9].state_new = 2;
    }
    else if((fsm[9].state == 3) && flag_K){
      flag_K = 0;
      fsm[9].state_new = 1;
    }
    else if((fsm[9].state == 4) && fsm[0].state != 1){
      fsm[9].state_new = 0;
    }


    // Update the States
    set_state(fsm[0], fsm[0].state_new);
    set_state(fsm[5], fsm[5].state_new);
    set_state(fsm[1], fsm[1].state_new);
    set_state(fsm[2], fsm[2].state_new);
    set_state(fsm[3], fsm[3].state_new);
    set_state(fsm[4], fsm[4].state_new);
    set_state(fsm[6], fsm[6].state_new);
    set_state(fsm[7], fsm[7].state_new);
    set_state(fsm[8], fsm[8].state_new);
    set_state(fsm[9], fsm[9].state_new);

    // Actions that FSM0 states trigger
    if(fsm[0].state == 0){
      K = 6;
      for(int i = 0; i < K; i++) LED[i] = 0;
      LEDp = 0;
      save_tis = 0;
    }
    else if(fsm[0].state == 1){
      for(int i = 0; i < K; i++) LED[i] = 1;
      for(int i = K; i < 6; i++) LED[i] = 0;
      LEDp = 0;
    }
    else if(fsm[0].state == 2){
      for(int i = 0; i < K; i++) LED[i] = 0;
      LEDp = 0;
    }
    else if(fsm[0].state == 3){
      for(int i = 0; i < K; i++) LED[i] = 1;
      for(int i = K; i < 6; i++) LED[i] = 0;
      LEDp = 0;
    }
    else if(fsm[0].state == 4){
      //LEDp = 1; 
    }
    else if(fsm[0].state == 5){
      for(int i = 0; i < 6; i++) LED[i] = 0;
    }

    // Actions that FSM5 states trigger
    if(fsm[5].state == 0){}
    else if(fsm[5].state == 1){}
    else if(fsm[5].state == 2){
      if(K<6 && ((fsm[1].state == 0) || (fsm[1].state == 1))) K++;
    }

    // Actions that FSM1 states trigger
    // Actions that FSM2 states trigger
    // Actions that FSM3 states trigger
    if(fsm[3].state == 0){}
    else if(fsm[3].state == 1){
      LED[5] = 1;
    }
    else if(fsm[3].state == 2){
      LED[5] = 0;
    }
    // Actions that FSM4 states trigger
    if(fsm[4].state == 0){}
    else if(fsm[4].state == 1){
      LED[4] = 1;
    }
    else if(fsm[4].state == 2){
      LED[4] = 0;
    }
    // Actions that FSM6 states trigger
    if(fsm[6].state == 0){}
    else if(fsm[6].state == 1){
      LED[3] = 1;
    }
    else if(fsm[6].state == 2){
      LED[3] = 0;
    }
    // Actions that FSM7 states trigger
    if(fsm[7].state == 0){}
    else if(fsm[7].state == 1){
      LEDp = 1;
    }
    else if(fsm[7].state == 2){
      LEDp = 0;
    }
    // Actions that FSM8 states trigger
    if(fsm[8].state == 0){}
    else if(fsm[8].state == 1){}
    else if(fsm[8].state == 2){
      LEDp = 1;
    }
    else if(fsm[8].state == 3){
      for(int i = 0; i < 6; i++) LED[i] = 1;
    }
    else if(fsm[8].state == 4){
      for(int i = 0; i < 6; i++) LED[i] = 0;
    }
    // Actions that FSM9 states trigger
    if(fsm[9].state == 0){}
    else if(fsm[9].state == 1){}
    else if(fsm[9].state == 2){
      LED[K-1] = 1;
    }
    else if(fsm[9].state == 3){
      LED[K-1] = 0;
    }
    else if(fsm[9].state == 4){}

    // Set the outputs
    digitalWrite(LED1, LED[5]);
    digitalWrite(LED2, LED[4]);
    digitalWrite(LED3, LED[3]);
    digitalWrite(LED4, LED[2]);
    digitalWrite(LED5, LED[1]);
    digitalWrite(LED6, LED[0]);
    digitalWrite(LED7, LEDp);

    /*
    Serial.println(fsm[1].state); 
    Serial.println(fsm[2].state); 
    Serial.print(fsm[3].state);Serial.print("  "); Serial.println(fsm[3].tis);
    Serial.print(fsm[4].state);Serial.print("  "); Serial.println(fsm[4].tis);
    Serial.print(fsm[6].state);Serial.print("  "); Serial.println(fsm[6].tis);
    Serial.println(interval_LED);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(fsm[7].state);
    Serial.println(fsm[8].state);
    Serial.println(fsm[9].state);
    */
    
    Serial.print("S1: "); Serial.println(S1);
    Serial.print("S2: "); Serial.println(S2);
    //Serial.println("LED1-LED2-LED3-LED4-LED5-LED6-LEDp "); Serial.print(LED[5]);Serial.print(LED[4]);Serial.print(LED[3]);Serial.print(LED[2]);Serial.print(LED[1]);Serial.println(LED[0]);
    Serial.print("Machine 0 state: "); Serial.println(fsm[0].state);
    Serial.print("Machine 5 state: "); Serial.println(fsm[5].state);
    Serial.print("Machine 1 state: "); Serial.println(fsm[1].state);
    Serial.print("Machine 2 state: "); Serial.println(fsm[2].state);
    Serial.print("Machine 3 state: "); Serial.println(fsm[3].state);
    Serial.print("Machine 4 state: "); Serial.println(fsm[4].state);
    Serial.print("Machine 6 state: "); Serial.println(fsm[6].state);
    Serial.print("Machine 7 state: "); Serial.println(fsm[7].state);
    Serial.print("Machine 8 state: "); Serial.println(fsm[8].state);
    Serial.print("Machine 9 state: "); Serial.println(fsm[9].state);

    Serial.println();
    //Serial.println(fsm[1].tis);
    //Serial.print(K);
  }
}