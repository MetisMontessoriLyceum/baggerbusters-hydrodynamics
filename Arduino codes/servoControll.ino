bool onNow = false;

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(6, OUTPUT);
}

void loop()
{
  if (digitalRead(2)==HIGH)
  {
    if (onNow){
      digitalWrite(6, HIGH);
    }else{
      digitalWrite(6, LOW);
    }
    onNow = !onNow;
    Serial.print(onNow);
    delay(100);
  }
}

