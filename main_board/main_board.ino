#define PUNCH_SIGNAL 12
#define SEND_TO_LCD 11

void setup() {
  // put your setup code here, to run once:
  pinMode(PUNCH_SIGNAL, INPUT);
  pinMode(SEND_TO_LCD, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  static int status = 0;
  int signal = digitalRead(PUNCH_SIGNAL);
  if (signal && status == 0) {
    digitalWrite(SEND_TO_LCD, HIGH);
    status = 1;
  }
  if (!signal && status == 1) {
    digitalWrite(SEND_TO_LCD, LOW);
    status = 0;
  }
}

