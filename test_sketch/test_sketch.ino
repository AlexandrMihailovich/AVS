int incomingByte[6] = {0, 0, 0, 0, 0, 0};

const int analogOutPin[6] = {11, 10, 9, 6, 5, 3};

int i = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available() > 0)
  {
    if (i > 5)
    {
      i = 0;
    }
    incomingByte[i] = Serial.read();
    analogWrite(analogOutPin[i], incomingByte[i]);
    i++;
  }
}
