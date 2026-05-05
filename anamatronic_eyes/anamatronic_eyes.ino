#include <Servo.h>
#define SENSORPIN 3 

Servo eye1_X; //left vertical
Servo eye1_Y; //left horizontal
Servo eye2_X; //right vertical
Servo eye2_Y; //right horizontal

const int centerPos = 90;
const int radius = 40; //50 absouloute HARD max

bool pirEnabled = true;

void setup() {
  eye1_X.attach(6); //white wire
  eye1_Y.attach(4); //blue wire
  eye2_X.attach(7); //green wire
  eye2_Y.attach(5); //orange wire

  pinMode(SENSORPIN, INPUT);

  resetToCenter();
}

void resetToCenter() {
  eye1_X.write(centerPos);
  eye1_Y.write(centerPos);
  eye2_X.write(centerPos);
  eye2_Y.write(centerPos);
}

void dartTo(int xOffset, int yOffset) {
  eye1_X.write(centerPos + xOffset);
  eye1_Y.write(centerPos + yOffset);
  eye2_X.write(centerPos + xOffset);
  eye2_Y.write(centerPos - yOffset);
}

void jitterMove(){
  for (int x = -radius; x <= radius; x+= 2){
    int jitterY = random(-5,6); //random but small jitter
    eye1_X.write(centerPos + x);
    eye2_X.write(centerPos + x);

    eye1_Y.write(centerPos + jitterY);
    eye2_Y.write(centerPos + jitterY);

    delay(random(20, 60)); //uneven timing 
  }
}

void Circles() {
  float angle = 0;

  while (angle <= 2 * PI) {
    int x1 = centerPos + radius * cos(angle);
    int y1 = centerPos + radius * sin(angle);

    int x2 = centerPos + radius * cos(-angle);
    int y2 = centerPos + radius * sin(-angle);

    eye1_X.write(x1);
    eye1_Y.write(y1);

    eye2_X.write(x2);
    eye2_Y.write(y2);

    angle += 0.18;  // increase for more speed
    delay(20);
  }
}

void performEyeRoutine() {

  resetToCenter();
  delay(200);

  //dart left
  dartTo(-radius, 0);
  delay(150);

  //slow move 
  jitterMove();

  //dart back to center
  resetToCenter();
  delay(500);

  //dart around like
  dartTo(-radius, 0);
  delay(500);
  dartTo(radius, 0);
  delay(500);
  resetToCenter();
  delay(500);
  dartTo(0, -radius);
  delay(500);
  dartTo(0, radius);
  delay(500);
  
  delay(500);
  resetToCenter();


  //pause
  delay(1000);

  //circles
  Circles();

  //reset
  resetToCenter();
}

void loop() {
  int pirSensor = digitalRead(SENSORPIN);

  if (pirSensor == HIGH && pirEnabled) {
    pirEnabled = false;

    performEyeRoutine();

    delay(1500);
    pirEnabled = true;

  delay(50);
  }
}
