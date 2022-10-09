#define addressRun 13
#define addressMode 12
#define addressUp 11
#define addressDown 10
#define R1 5
#define R2 4
#define R3 3
#define R4 2

#define C1 A5
#define C2 A4
#define C3 A3
#define C4 A2

String listOfButtons[4] = {"Mode/", "Run/", "Up/", "Down/"};
int addressOfButtons[4] = {addressMode, addressRun, addressUp, addressDown};
int listOfcheck[4] = {0, 0, 0, 0};
String keyPressed = "";

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 4; i++)
  {
    pinMode(addressOfButtons[i], INPUT_PULLUP);
  }
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);

  pinMode(C1, INPUT_PULLUP);
  pinMode(C2, INPUT_PULLUP);
  pinMode(C3, INPUT_PULLUP);
  pinMode(C4, INPUT_PULLUP);
}
void readAndSendAllButton()
{
  for (int i = 0; i < 4; i++)
  {
    if (digitalRead(addressOfButtons[i]) == 0)
    {
      listOfcheck[i] = 1;
    }

    if (digitalRead(addressOfButtons[i]) == 1 && listOfcheck[i] == 1)
    {
      Serial.print(listOfButtons[i]);
      listOfcheck[i] = 0;
    }
  }
}
void readAndSendAllKey()
{
  digitalWrite(R1, 0);
  digitalWrite(R2, 1);
  digitalWrite(R3, 1);
  digitalWrite(R4, 1);
  if (digitalRead(C1) == 0)
  {
    keyPressed = "1/";
  }
  if (digitalRead(C2) == 0)
  {
    keyPressed = "2/";
  }
  if (digitalRead(C3) == 0)
  {
    keyPressed = "3/";
  }
  if (digitalRead(C4) == 0)
  {
    keyPressed = "A/";
  }
  digitalWrite(R1, 1);
  digitalWrite(R2, 0);
  digitalWrite(R3, 1);
  digitalWrite(R4, 1);
  if (digitalRead(C1) == 0)
  {
    keyPressed = "4/";
  }
  if (digitalRead(C2) == 0)
  {
    keyPressed = "5/";
  }
  if (digitalRead(C3) == 0)
  {
    keyPressed = "6/";
  }
  if (digitalRead(C4) == 0)
  {
    keyPressed = "B/";
  }
  digitalWrite(R1, 1);
  digitalWrite(R2, 1);
  digitalWrite(R3, 0);
  digitalWrite(R4, 1);
  if (digitalRead(C1) == 0)
  {
    keyPressed = "7/";
  }
  if (digitalRead(C2) == 0)
  {
    keyPressed = "8/";
  }
  if (digitalRead(C3) == 0)
  {
    keyPressed = "9/";
  }
  if (digitalRead(C4) == 0)
  {
    keyPressed = "C/";
  }
  digitalWrite(R1, 1);
  digitalWrite(R2, 1);
  digitalWrite(R3, 1);
  digitalWrite(R4, 0);
  // if (digitalRead(C1) == 0) {keyPressed="*/";}
  if (digitalRead(C2) == 0)
  {
    keyPressed = "0/";
  }
  // if (digitalRead(C3) == 0) {keyPressed="#/";}
  if (digitalRead(C4) == 0)
  {
    keyPressed = "D/";
  }
  if (keyPressed != "")
  {
    digitalWrite(R1, 0);
    digitalWrite(R2, 1);
    digitalWrite(R3, 1);
    digitalWrite(R4, 1);
    if (digitalRead(C1) == 1 && keyPressed == "1/")
    {
      Serial.print("1/");
      keyPressed = "";
    }
    if (digitalRead(C2) == 1 && keyPressed == "2/")
    {
      Serial.print("2/");
      keyPressed = "";
    }
    if (digitalRead(C3) == 1 && keyPressed == "3/")
    {
      Serial.print("3/");
      keyPressed = "";
    }
    if (digitalRead(C4) == 1 && keyPressed == "A/")
    {
      Serial.print("Run/");
      keyPressed = "";
    }
    digitalWrite(R1, 1);
    digitalWrite(R2, 0);
    digitalWrite(R3, 1);
    digitalWrite(R4, 1);
    if (digitalRead(C1) == 1 && keyPressed == "4/")
    {
      Serial.print("4/");
      keyPressed = "";
    }
    if (digitalRead(C2) == 1 && keyPressed == "5/")
    {
      Serial.print("5/");
      keyPressed = "";
    }
    if (digitalRead(C3) == 1 && keyPressed == "6/")
    {
      Serial.print("6/");
      keyPressed = "";
    }
    if (digitalRead(C4) == 1 && keyPressed == "B/")
    {
      Serial.print("Mode/");
      keyPressed = "";
    }
    digitalWrite(R1, 1);
    digitalWrite(R2, 1);
    digitalWrite(R3, 0);
    digitalWrite(R4, 1);
    if (digitalRead(C1) == 1 && keyPressed == "7/")
    {
      Serial.print("7/");
      keyPressed = "";
    }
    if (digitalRead(C2) == 1 && keyPressed == "8/")
    {
      Serial.print("8/");
      keyPressed = "";
    }
    if (digitalRead(C3) == 1 && keyPressed == "9/")
    {
      Serial.print("9/");
      keyPressed = "";
    }
    if (digitalRead(C4) == 1 && keyPressed == "C/")
    {
      Serial.print("Up/");
      keyPressed = "";
    }
    digitalWrite(R1, 1);
    digitalWrite(R2, 1);
    digitalWrite(R3, 1);
    digitalWrite(R4, 0);
    // if (digitalRead(C1) == 1 && keyPressed=="*/") {Serial.print("*/");keyPressed="";}
    if (digitalRead(C2) == 1 && keyPressed == "0/")
    {
      Serial.print("0/");
      keyPressed = "";
    }
    // if (digitalRead(C3) == 1 && keyPressed=="#/") {Serial.print("#/");keyPressed="";}
    if (digitalRead(C4) == 1 && keyPressed == "D/")
    {
      Serial.print("Down/");
      keyPressed = "";
    }
  }
}
void sendCommand()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('/');
    command.trim();
    if (command != "")
    {
      Serial.print(command + '/');
    }
  }
}
void loop()
{
  readAndSendAllButton();
  readAndSendAllKey();
  sendCommand();
}