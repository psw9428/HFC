#define PUNCH_SIGNAL 12
#define SEND_TO_LCD 11

void setup() {
  // put your setup code here, to run once:
  pinMode(PUNCH_SIGNAL, INPUT);
  pinMode(SEND_TO_LCD, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(SEND_TO_LCD, LOW);
  Serial.println("...");
  while (!digitalRead(PUNCH_SIGNAL)) {
    delay(50);
  }
  Serial.println("!!!!");
  digitalWrite(SEND_TO_LCD, HIGH);
  delay(300);
}

