#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

volatile int ammo = 300;  // Initial ammo count
volatile unsigned long firstBulletTime = 0;
volatile unsigned long secondBulletTime = 0;
const float distanceBetweenPhototransistors = 0.08;  // Distance between phototransistors in meters (10 cm)

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT);  // Connect the first phototransistor to digital pin 2
  pinMode(3, INPUT);  // Connect the second phototransistor to digital pin 3
  attachInterrupt(digitalPinToInterrupt(2), firstBulletDetected, FALLING); // Set up interrupt for the first phototransistor
  attachInterrupt(digitalPinToInterrupt(3), secondBulletDetected, FALLING); // Set up interrupt for the second phototransistor

  delay(250);
  display.begin(i2c_Address, true);
  display.display();
  delay(2000);
}

void loop() {
  // Calculate speed of the last bullet
  float bulletSpeed = calculateBulletSpeed();

  // Display ammo count and bullet speed
  display.clearDisplay();

   // Draw the cross and circle (same as before)
  int crossSize = 10;
  int crossX = (SCREEN_WIDTH - crossSize) / 2;
  int crossY = (SCREEN_HEIGHT - crossSize) / 2;
 //display.drawLine(crossX, crossY + crossSize / 2, crossX + crossSize, crossY + crossSize / 2, SH110X_WHITE);
  //display.drawLine(crossX + crossSize / 2, crossY, crossX + crossSize / 2, crossY + crossSize, SH110X_WHITE);
  display.drawLine(0,32,60,32, SH110X_WHITE);
  display.drawLine(68,32,128,32, SH110X_WHITE);

  display.drawLine(60,40,68,40, SH110X_WHITE);
  display.drawLine(62,44,66,44, SH110X_WHITE);

  display.drawLine(64,34,64,64, SH110X_WHITE);


  int circleRadius = 5;
  int circleCenterX = SCREEN_WIDTH / 2;
  int circleCenterY = SCREEN_HEIGHT / 2;
  display.drawCircle(circleCenterX, circleCenterY, circleRadius, SH110X_WHITE);

  // Display ammo count
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, SCREEN_HEIGHT - 8); // Position for ammo count
  display.print(ammo);

  // Display bullet speed
  display.setCursor(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 8);
  display.print(bulletSpeed);
  display.print(" m/s");

  // Draw ammo animation
  drawAmmoAnimation();

  display.display();
  delay(100); // Small delay for display update
}

void firstBulletDetected() {
  firstBulletTime = micros(); // Record the time when the first phototransistor detected the bullet
}

void secondBulletDetected() {
  secondBulletTime = micros();
  if (ammo > 0) {
    ammo--; // Decrement ammo count by 1
    delay(15); // Small delay for display update
  }
}

float calculateBulletSpeed() {
  if (firstBulletTime == 0 || secondBulletTime == 0) {
    return 0.0;  // No complete bullet detection yet
  }

  float timeDifference = (secondBulletTime - firstBulletTime) / 1000000.0; // Convert to seconds
  return distanceBetweenPhototransistors / timeDifference;
  //return 262467 / timeDifference;
}

void drawAmmoAnimation() {
  int maxSquares = 30; // Maximum number of squares to represent full ammo
  int squaresToShow = map(ammo, 0, 300, 0, maxSquares); // Calculate number of squares to show
  int squareSize = 4; // Size of each square
  int startX = (SCREEN_WIDTH - (maxSquares * squareSize)) / 2;
  int startY = 0;
  
  for (int i = 0; i < squaresToShow; i++) {
    display.fillRect(startX + i * squareSize, startY, squareSize, squareSize, SH110X_WHITE);
  }
}
