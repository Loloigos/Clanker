#include <Servo.h>

// User-defined variables (kept active ones used for configuration/pins)
constexpr int pinLB = 0;      // define pin6 as left back connect with IN1
constexpr int pinLF = 0;      // define pin9 as left forward connect with IN2
constexpr int pinRB = 8;      // define pin10 as right back connect with IN3
constexpr int pinRF = 9;      // define pin11 as right back connect with IN4
constexpr int pinMotor = 11;  // Pin that controls the motor: ENB - Green cord
constexpr int inputPin = A0;  // define ultrasonic receive pin (Echo)
constexpr int outputPin = A1; // define ultrasonic send pin(Trig)
constexpr int ldrPin = A2;

#define SERVOS 2 
Servo myservo[SERVOS]; 
int servo_pins[SERVOS] = {5, 10}; 

void setup() {
  Serial.begin(9600); 
  pinMode(pinLB, OUTPUT);
  pinMode(pinLF, OUTPUT);
  pinMode(pinRB, OUTPUT);
  pinMode(pinRF, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ldrPin, INPUT);

  for (int i = 0; i < SERVOS; i++) {
    myservo[i].attach(servo_pins[i]);
    delay(500);
  }

  // Forward motion directly from the start:
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, HIGH);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
  delay(150);
}

int seek(int angle, int wait = 0) {
    myservo[0].write(angle);

    if (wait > 0) {
        delay(wait);
    }

    digitalWrite(outputPin, LOW);
    delayMicroseconds(2);
    digitalWrite(outputPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(outputPin, LOW);

    unsigned long duration = pulseIn(inputPin, HIGH, 25000UL);

    if (duration == 0) {
        return 200;
    }

    int distance = duration / 58;

    return constrain(distance, 2, 400);
}

void back(int g) //back
{
    myservo[1].write(90);
    delay(250);       
    analogWrite(pinMotor,100);
    digitalWrite(pinRB,HIGH);
    digitalWrite(pinRF,LOW);
    digitalWrite(pinLB,HIGH);
    digitalWrite(pinLF,LOW);
    delay(g * 300  /2);
}

void drive(int direction, int turn, int speed) { // the drive function
  if (direction == 0) {
    digitalWrite(pinRB, LOW);
    digitalWrite(pinRF, LOW);
  }
  if (direction < 0) {
    digitalWrite(pinRB, HIGH);
    digitalWrite(pinRF, LOW);
  }
  if (direction > 0) {
    digitalWrite(pinRB, LOW);
    digitalWrite(pinRF, HIGH);
  }

  if (turn == 0) {
    myservo[1].write(90);
  }
  if (turn < 0) {
    myservo[1].write(40);
  }
  if (turn > 0) {
    myservo[1].write(150);
  }
  analogWrite(pinMotor, speed);
}

bool inGarage() { 

  if(analogRead(ldrPin) > 25) {
    return false;
  } else {
    return true;
  }
}

void park() { // stops the car
  while (1) {
    drive(0, 0, 0);
  }
}

void loop() {
  if (inGarage()) {
        park();
        return;
    }

    const int scanAngles[] = {30, 50, 70, 90, 110, 130, 150};
    const int scanCount = 7;
    int distance[scanCount];

    for (int i = 0; i < scanCount; i++) {
        distance[i] = seek(scanAngles[i], 50);
    }

    seek(90, 100);
    delay(100);

    int rightSpace = (distance[0] + distance[1] + distance[2]) / 3;
    int frontSpace = distance[3];
    int leftSpace = (distance[4] + distance[5] + distance[6]) / 3;

    static int lastTurn = -1;

    if (frontSpace < 18 && leftSpace < 22 && rightSpace < 22) {
        drive(-1, 0, 100);
        delay(200);
        return;
    }

    if (frontSpace > 45) {
        drive(1, 0, 100);
        return;
    }

    int turn;

    if (leftSpace > rightSpace + 8) {
        turn = -1; 
    }
    else if (rightSpace > leftSpace + 8) {
        turn = 1;   
    }
    else {
        turn = lastTurn;
    }

    lastTurn = turn;

    if (frontSpace < 25) {
        drive(1, turn, 65);
    }
    else {
        drive(1, turn, 80);
    }
    delay(100);
}