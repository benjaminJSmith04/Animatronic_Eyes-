#include <Arduino_FreeRTOS.h>
#include <Servo.h>

#define SENSORPIN 3

Servo eye1_X; //left vertical
Servo eye1_Y; //left horizontal
Servo eye2_X; //right vertical
Servo eye2_Y; //right horizontal

const int centerPos = 90;
const int radius = 40; //50 absouloute HARD max

TaskHandle_t eyeTaskHandle = NULL;

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

void jitterMove() {
  for (int x = -radius; x <= radius; x += 2) {
    int jitterY = random(-5, 6); //random but small jitter

    eye1_X.write(centerPos + x);
    eye2_X.write(centerPos + x);

    eye1_Y.write(centerPos + jitterY);
    eye2_Y.write(centerPos + jitterY);

    vTaskDelay(pdMS_TO_TICKS(random(20, 60))); //uneven timing
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
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void performEyeRoutine() {

  resetToCenter();
  vTaskDelay(pdMS_TO_TICKS(200));

  //dart left
  dartTo(-radius, 0);
  vTaskDelay(pdMS_TO_TICKS(150));

  //slow move
  jitterMove();

  //dart back to center
  resetToCenter();
  vTaskDelay(pdMS_TO_TICKS(500));

  //dart around like
  dartTo(-radius, 0);
  vTaskDelay(pdMS_TO_TICKS(500));

  dartTo(radius, 0);
  vTaskDelay(pdMS_TO_TICKS(500));

  resetToCenter();
  vTaskDelay(pdMS_TO_TICKS(500));

  dartTo(0, -radius);
  vTaskDelay(pdMS_TO_TICKS(500));

  dartTo(0, radius);
  vTaskDelay(pdMS_TO_TICKS(500));

  vTaskDelay(pdMS_TO_TICKS(500));

  resetToCenter();

  //pause
  vTaskDelay(pdMS_TO_TICKS(1000));

  //circles
  Circles();

  //reset
  resetToCenter();
}

void PIRTask(void *pvParameters) {
  bool triggered = false;

  while (1) {
    int pirSensor = digitalRead(SENSORPIN);

    if (pirSensor == HIGH) {
      if (!triggered) {
        triggered = true;
        xTaskNotifyGive(eyeTaskHandle);
      }
    } else {
      triggered = false;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void EyeTask(void *pvParameters) {
  while (1) {

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    performEyeRoutine();

    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void setup() {
  eye1_X.attach(6); //white wire
  eye1_Y.attach(4); //blue wire
  eye2_X.attach(7); //green wire
  eye2_Y.attach(5); //orange wire

  pinMode(SENSORPIN, INPUT);

  resetToCenter();

  xTaskCreate(
    PIRTask,
    "PIR",
    128,
    NULL,
    2,
    NULL);

  xTaskCreate(
    EyeTask,
    "Eyes",
    256,
    NULL,
    1,
    &eyeTaskHandle);
}

void loop() {
}
