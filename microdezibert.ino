#include <Servo.h> 
#include <SPI.h>

// hardware
int mic = 3;
int servo_port = 5;

//setup
int treshold = 2; //5: ich und thommy allein im raum; laptopsound schlägt an, komplette stille senkt den finger. 8: lärmige Cs
int reduce_every_steps = 5;
int finger_angle = 90;

//internals
Servo myservo;
int noise_level = analogRead(mic);
int penalty_level = 0;
int analog_value = 0;
int silent_steps = 0;
int finger_pos = 0;
int finger_moving = 0;

//led

int red = 2; 
int green = 4; 
int blue = 3;


void setup() {
  Serial.begin(9600);
  myservo.attach(servo_port);
  myservo.write(0);

  //matrix
  for(int i = 22; i <= 49; i += 1){
    pinMode(i, OUTPUT);
  }

  finger_up();
  finger_down();

}


void loop() {

  analog_value = analogRead(mic);
  //l(analog_value);

  int diff = noise_level - analog_value;
  diff = abs(diff);
  //l(diff);

  //read taster
  int taster = digitalRead(7);
  if(taster){
    diff = 100;
  } 

  //decibel value on LED-matrix
  int matrix_scale = scale(penalty_level, 20, 8);
  led_level(matrix_scale);


  if(silent_steps == reduce_every_steps){
    penalty_decrease();
    silent_steps = 0;
  }

  if(diff > treshold){
    penalty_increase();
  }

  noise_level = analog_value;
  silent_steps++;
  delay(100);

}


//penalty management
void penalty_increase(){
  if(penalty_level < 23){
    penalty_level++;
    penalty_handler();
  }
}

void penalty_decrease(){
  if(penalty_level > 0){
    penalty_level--;
    penalty_handler();
  }
}



void penalty_handler(){
  l(penalty_level);
  if(penalty_level >= 0 && penalty_level < 9){
    finger_down();
  }
  if(penalty_level >= 21 && penalty_level < 23){
    finger_up();
  }
  if(penalty_level >= 23){
    //tweet();
  }
}

//finger control
void finger_up(){
  if(!finger_moving && finger_pos == 0){
    finger_moving = 1;
    //move finger up
    for(int pos = 0; pos <= finger_angle; pos += 1){
      myservo.write(pos);
      delay(10);
    }
    finger_moving = 0;
    finger_pos = 1;
  }
}

void finger_down(){
  if(!finger_moving && finger_pos == 1){
    finger_moving = 1;
    //move finger down
    for(int pos = finger_angle; pos >= 1; pos -= 1){
      myservo.write(pos);
      delay(20);
    }
    finger_moving = 0;
    finger_pos = 0;
  }
}

/* 
 
 Level 0 nothing, 1-3 green, 4-6 orange, 7-8 red 
 
 */


void led_level(int level){

  //set colors
  if(level == 0){
    color(0,255,0);
  }
  else
    if(level > 0 && level <= 3){
      color(128,255,0);
    }
    else
      if(level > 3 && level <= 6){
        color(255,128,0);
      }
      else
        if(level > 6 && level <= 8){
          color(255,0,0);
        }
}



int scale(float val, int in_max, int iwant_max){
  float res = (val / in_max * iwant_max);
  res = int(res);
  return res;
}



//log
void l(int i){
  Serial.println(i);


}

void color (int c1, int c2, int c3) {	 
  analogWrite(red, c1);	 
  analogWrite(blue,c2);
  analogWrite(green, c3);
}

