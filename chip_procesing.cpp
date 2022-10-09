//++++++++++++LIBRARY++++++++++++
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <TimerOne.h>
int hourCurrent = 19;
int minuteCurrent = 30;
int secondCurrent = 0;
int hourSetting = 0;
int minuteSetting = 0;
String arrayOfStatus[3] = {"N/A", "ON", "OFF"};
bool isNumber = false;
bool isPreNumber = false;
String lastCommand = "";
#define RELAY1 8
#define RELAY2 9
String statusRELAY1 = "Off";
String statusRELAY2 = "Off";

/*
    Class Box : có nhiệm vụ chứa thông tin của từng trạng thái được cài đặt của relay, load thông tin từ EEPROM
    , nhận thông tin và khởi tạo thông tin vào EEPROM
*/
class Box
{
public:
    int hourAddress;
    int minuteAddress;
    int statusAddress;
    int hourSetting;
    int minuteSetting;
    int statusAlarmIndex;
    String arrayOfStatus[3] = {"N/A", "ON", "OFF"};
    Box(int hourAddress, int minuteAddress, int statusAddress)
    {
        this->hourAddress = hourAddress;
        this->minuteAddress = minuteAddress;
        this->statusAddress = statusAddress;
    }
    // init infor from EEPROM
    void initInforFromEEPROM()
    {
        this->hourSetting = EEPROM.read(hourAddress);
        this->minuteSetting = EEPROM.read(minuteAddress);
        this->statusAlarmIndex = EEPROM.read(statusAddress);
    }

    void updateInforToEEPROM()
    {
        EEPROM.update(hourAddress, hourSetting);
        EEPROM.update(minuteAddress, minuteSetting);
        EEPROM.update(statusAddress, statusAlarmIndex);
    }

    void updateInfor(int statusAlarmIndex, int hourSettingL, int minuteSettingL)
    {
        this->hourSetting = hourSettingL;
        this->minuteSetting = minuteSettingL;
        this->statusAlarmIndex = statusAlarmIndex;
        updateInforToEEPROM();
    }
};
// Mảng khởi tạo mặc định các giá trị trước khi chọn relay
Box arrayOfBoxes[5] = {Box(100, 100, 100), Box(100, 100, 100), Box(100, 100, 100), Box(100, 100, 100), Box(100, 100, 100)};
/*
Class button: Thực hiện đọc các thông tin nút bấm từ mạch input , xử lý thông tin input
*/
class Button
{
public:
    int setHourSetting(int hourSetting, String buttonWhichPressed)
    {
        if (buttonWhichPressed == "Up")
        {
            hourSetting++;
            if (hourSetting > 23)
            {
                hourSetting = 0;
            }
        }
        else if (buttonWhichPressed == "Down")
        {
            hourSetting--;
            if (hourSetting < 0)
            {
                hourSetting = 23;
            }
        }
        if (buttonWhichPressed != "" && buttonWhichPressed != "Run" && buttonWhichPressed != "Mode")
        {
            if (isPreNumber == false && isNumber == true)
            {
                hourSetting = int(buttonWhichPressed[0] - '0');
            }
            else if (isPreNumber == true && isNumber == true)
            {
                hourSetting = hourSetting * 10 + int(buttonWhichPressed[0] - '0');
                if (hourSetting > 23)
                    hourSetting %= 10;
            }
        }
        return hourSetting;
    }

    int setMinuteSetting(int minuteSetting, String buttonWhichPressed)
    {
        if (buttonWhichPressed == "Up")
        {
            minuteSetting++;
            if (minuteSetting > 59)
            {
                minuteSetting = 0;
            }
        }
        else if (buttonWhichPressed == "Down")
        {
            minuteSetting--;
            if (minuteSetting < 0)
            {
                minuteSetting = 59;
            }
        }
        if (buttonWhichPressed != "" && buttonWhichPressed != "Run" && buttonWhichPressed != "Mode")
        {
            if (isPreNumber == false && isNumber == true)
            {
                minuteSetting = int(buttonWhichPressed[0] - '0');
            }
            else if (isPreNumber == true && isNumber == true)
            {
                minuteSetting = minuteSetting * 10 + int(buttonWhichPressed[0] - '0');
                if (minuteSetting > 59)
                    minuteSetting %= 10;
            }
        }
        return minuteSetting;
    }
    String pressWhichButton()
    {
        String lastButton = Serial.readStringUntil('/');

        if (lastButton[0] >= '0' && lastButton[0] <= '9')
        {
            isPreNumber = isNumber;
            isNumber = true;
        }
        else if (lastButton == "Up" || lastButton == "Down")
        {
            isPreNumber = isNumber;
            isNumber = false;
        }
        if (lastButton.length() > 1)
        {
            lastCommand = lastButton;
        }
        return lastButton;
    }
};
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/*
    Lấy thông tin từ cài đặt gần nhất của relay được chọn và hiển thị lên màn hình LCD
*/
int getFollwingBoxIndex(int hourCurrent, int minuteCurrent)
{
    int smallestTimeRemaining = 24 * 60;
    int smallestTimeRemainingIndex = -1;
    for (int i = 0; i < 5; i++)
    {
        if (arrayOfBoxes[i].statusAlarmIndex == 0)
        {
            continue;
        }
        int hourRemainingOfBox = arrayOfBoxes[i].hourSetting - hourCurrent;
        int minuteRemainingOfBox = arrayOfBoxes[i].minuteSetting - minuteCurrent;
        if (minuteRemainingOfBox < 0)
        {
            minuteRemainingOfBox += 60;
            hourRemainingOfBox--;
        }
        if (hourRemainingOfBox < 0)
        {
            hourRemainingOfBox += 24;
        }
        int timeRemainingOfBox = hourRemainingOfBox * 60 + minuteRemainingOfBox;
        if (timeRemainingOfBox < smallestTimeRemaining)
        {
            smallestTimeRemaining = timeRemainingOfBox;
            smallestTimeRemainingIndex = i;
        }
    }
    return smallestTimeRemainingIndex;
}

/*
    Class LedScreen: Hiện thị thông tin từng màn hình
*/
class LedScreen
{
public:
    void selectRelayScreen(int hourCurrent, int minuteCurrent, int secondCurrent)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        String currentTime = formatTime(hourCurrent, minuteCurrent, secondCurrent);
        lcd.print(currentTime);
        lcd.setCursor(0, 1);
        lcd.print("RELAY: 1UP 2DOWN");
        delay(200);
    }
    void setRelayScreen(int hourCurrent, int minuteCurrent, int secondCurrent)
    {
        int follwingBoxIndex = getFollwingBoxIndex(hourCurrent, minuteCurrent);
        String followingMessage = "";
        if (follwingBoxIndex == -1)
        {
            followingMessage = "No alarm yet";
        }
        else
        {
            followingMessage = "NextAlarm: " + formatTime(arrayOfBoxes[follwingBoxIndex].hourSetting, arrayOfBoxes[follwingBoxIndex].minuteSetting);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        String currentTime = formatTime(hourCurrent, minuteCurrent, secondCurrent);
        lcd.print(currentTime);
        lcd.setCursor(0, 1);
        lcd.print(followingMessage);
        delay(200);
    }
    String formatTime(int hourCurrent, int minuteCurrent, int secondCurrent)
    {
        String hour = String(hourCurrent);
        String minute = String(minuteCurrent);
        String second = String(secondCurrent);

        if (hourCurrent < 10)
        {
            hour = "0" + hour;
        }

        if (minuteCurrent < 10)
        {
            minute = "0" + minute;
        }
        if (secondCurrent < 10)
        {
            second = "0" + second;
        }
        return hour + ":" + minute + ":" + second;
    }
    String formatTime(int hourSetting, int minuteSetting)
    {
        String hour = String(hourSetting);
        String minute = String(minuteSetting);
        if (hourSetting < 10)
        {
            hour = "0" + hour;
        }
        if (minuteSetting < 10)
        {
            minute = "0" + minute;
        }

        return hour + ":" + minute;
    }
    //  screen 2
    void showBoxInforScreen(int indexBox, int hourSetting, int minuteSetting, int statusAlarmIndex)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        String settingTime = formatTime(hourSetting, minuteSetting);
        lcd.print("Box " + String(indexBox) + ": " + settingTime);
        lcd.setCursor(0, 1);
        lcd.print(arrayOfStatus[statusAlarmIndex]);
        delay(200);
    }

    //  screen 3
    void showStatusBoxScreen()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ON[UP] OFF[DOWN]");
        lcd.setCursor(0, 1);
        lcd.print("N/A[RUN]");
    }
    //  screen 4
    void setTimeBoxScreen(int hourSetting, int minuteSetting)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hour: " + String(hourSetting));
        lcd.setCursor(0, 1);
        lcd.print("Minute: " + String(minuteSetting));
    }
    void showSellectSwitch()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Switch 1: Up");
        lcd.setCursor(0, 1);
        lcd.print("Switch 2: Down");
    }
};

//++++++++++++GLOBAL VARIABLES++++++++++++
// keypad
// KeypadMatrix keypadMatrix = KeypadMatrix(Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM));
String inputStringFromKeypad;
long inputIntFromKeypad = 0;
int statusAlarmIndex = 0;

// time
int alarmingHour = -1;
int alarmingMinute = -1;
int indexBoxsRelay1[5] = {0, 3, 6, 9, 12};
int indexBoxsRelay2[5] = {15, 18, 21, 24, 27};

LedScreen ledScreen = LedScreen();
Button button;

/*
    Load dữ liệu đang chứa trong EEPROM
*/
void readAllFromEEPROM()
{
    Serial.println("readAllFromEEPROM");
    for (int i = 0; i < 5; i++)
    {
        // read from EEPROM
        arrayOfBoxes[i].initInforFromEEPROM();
    }
}

/*
    Lấy dữ liệu được nhập từ bàn phím và xử lý thông tin của
*/
void inputFromUART()
{
    // SettingBox:Relay:indexBox:status:hour:minute
    // SettingCurrentTime:hour:minute:second
    // SettingBox:01:02:00:12:23/
    // SettingCurrentTime:12:23:00/
    // Read command from serial
    // String command = Serial.readStringUntil('/');
    // Check command

    //
    if (lastCommand.substring(0, 10) == "SettingBox")
    {
        Serial.println(lastCommand);
        int relayMode = lastCommand.substring(11, 13).toInt();
        int indexBox = lastCommand.substring(14, 16).toInt();
        int status = lastCommand.substring(17, 19).toInt();
        int hour = lastCommand.substring(20, 22).toInt();
        int minute = lastCommand.substring(23, 25).toInt();
        // Check relay
        if (relayMode == 1)
        {

            Box updateBox(indexBoxsRelay1[indexBox], indexBoxsRelay1[indexBox] + 1, indexBoxsRelay1[indexBox] + 2);
            updateBox.updateInfor(status, hour, minute);
            readAllFromEEPROM();
        }
        if (relayMode == 2)
        {
            Serial.println(lastCommand);

            Box updateBox(indexBoxsRelay2[indexBox], indexBoxsRelay2[indexBox] + 1, indexBoxsRelay2[indexBox] + 2);
            updateBox.updateInfor(status, hour, minute);
            readAllFromEEPROM();
        }
        lastCommand = "";
    }
    else if (lastCommand.substring(0, 18) == "SettingCurrentTime")
    {
        hourCurrent = lastCommand.substring(19, 21).toInt();
        minuteCurrent = lastCommand.substring(22, 24).toInt();
        secondCurrent = lastCommand.substring(25, 27).toInt();
        Serial.println("Done set current time!" + String(hourCurrent) + ":" + String(minuteCurrent) + ":" + String(secondCurrent));
        lastCommand = "";
    }
    else if (lastCommand == "Status")
    {
        Serial.println("RELAY1: " + statusRELAY1);
        Serial.println("RELAY2: " + statusRELAY2);
        lastCommand = "";
    }
}

/*
    Kiểm tra xem có đang trong thời gian bật hay tắt relay hay không
*/
void writeRelay()
{

    for (int i = 0; i < 5; i++)
    {

        int hourBox1 = EEPROM.read(indexBoxsRelay1[i]);
        int minuteBox1 = EEPROM.read(indexBoxsRelay1[i] + 1);
        int statusBox1 = EEPROM.read(indexBoxsRelay1[i] + 2);

        // Check to blink led
        if (hourCurrent == hourBox1 && minuteCurrent == minuteBox1)
        {
            // Serial.println("current Hour: " + String(hourCurrent) + " current Minute: " + String(minuteCurrent));
            // Serial.println("Hour: " + String(hourBox1) + " Minute: " + String(minuteBox1));
            if (statusBox1 == 1)
            {
                digitalWrite(RELAY1, 1);
                statusRELAY1 = "On";
            }
            else if (statusBox1 == 2)
            {
                digitalWrite(RELAY1, 0);
                statusRELAY1 = "Off";
            }
        }
    }
    for (int i = 0; i < 5; i++)
    {
        int hourBox2 = EEPROM.read(indexBoxsRelay2[i]);
        int minuteBox2 = EEPROM.read(indexBoxsRelay2[i] + 1);
        int statusBox2 = EEPROM.read(indexBoxsRelay2[i] + 2);
        // Check to blink led
        if (hourCurrent == hourBox2 && minuteCurrent == minuteBox2)
        {
            if (statusBox2 == 1)
            {
                digitalWrite(RELAY2, 1);
                statusRELAY2 = "On";
            }
            else if (statusBox2 == 2)
            {
                digitalWrite(RELAY2, 0);
                statusRELAY2 = "Off";
            }
        }
    }
}

/*
    Xử lý thời gian thực
*/
void realTimeProcessing()
{
    secondCurrent++;
    // Check second out of range
    if (secondCurrent > 59)
    {
        secondCurrent = 0;
        minuteCurrent++;
    }
    // Check minute out of range
    if (minuteCurrent > 59)
    {
        minuteCurrent = 0;
        hourCurrent++;
    }
    // Check hour out of range
    if (hourCurrent > 23)
    {
        hourCurrent = 0;
    }
}

/*
    Bao gồm các chức năng được gọi sau mỗi giây bằng tính năng ngắt thời gian
*/
void callback()
{
    realTimeProcessing();
    writeRelay();
    inputFromUART();
}
//++++++++++++SETUP()++++++++++++
/*
    Setup
*/
void setup()
{
    // clear EEPROM memory
    // led screen
    lcd.begin(16, 2);
    // keypad
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    Serial.begin(9600);
    Timer1.initialize(1000000); // initialize timer1, and set a 1/2 second period
    /*
    Using call back to replace them
    // Timer1.attachInterrupt(realTimeProcessing); // attaches callback() as a timer
    // Timer1.attachInterrupt(writeRelay);
    // Timer1.attachInterrupt(inputFromUART);
    */
    Timer1.attachInterrupt(callback);
}
/*
    Main program
*/
void loop()
{
    // Select relay to setting
    while (true)
    {
        ledScreen.selectRelayScreen(hourCurrent, minuteCurrent, secondCurrent);
        // Serial.println("Pipeline: Loop to select [switch1] or [switch2]");
        // TODO: add led screen
        String buttonWhichPressed = button.pressWhichButton();

        if (buttonWhichPressed == "Up")
        {
            arrayOfBoxes[0] = Box(0, 1, 2);
            arrayOfBoxes[1] = Box(3, 4, 5);
            arrayOfBoxes[2] = Box(6, 7, 8);
            arrayOfBoxes[3] = Box(9, 10, 11);
            arrayOfBoxes[4] = Box(12, 13, 14);
            // Serial.println("Run [[switch 1]]");
            break;
        }
        else if (buttonWhichPressed == "Down")
        {
            arrayOfBoxes[0] = Box(15, 16, 17);
            arrayOfBoxes[1] = Box(18, 19, 20);
            arrayOfBoxes[2] = Box(21, 22, 23);
            arrayOfBoxes[3] = Box(24, 25, 26);
            arrayOfBoxes[4] = Box(27, 28, 29);
            // Serial.println("Run [[switch 2]]");
            break;
        }
    }
    // Loading information from EEPROM
    readAllFromEEPROM();

    while (true)
    {
        ledScreen.setRelayScreen(hourCurrent, minuteCurrent, secondCurrent);
        String buttonWhichPressed = button.pressWhichButton();
        // Screen show infor of each box
        if (buttonWhichPressed == "Run")
        {
            int boxLoopIndex = 0;
            while (true)
            {
                ledScreen.showBoxInforScreen(boxLoopIndex, arrayOfBoxes[boxLoopIndex].hourSetting, arrayOfBoxes[boxLoopIndex].minuteSetting, arrayOfBoxes[boxLoopIndex].statusAlarmIndex);
                String buttonWhichPressed = button.pressWhichButton();
                if (buttonWhichPressed == "Up")
                {
                    boxLoopIndex++;
                }
                else if (buttonWhichPressed == "Down")
                {
                    boxLoopIndex--;
                }

                boxLoopIndex = (boxLoopIndex + 10) % 5;
                if (buttonWhichPressed == "Run")
                {
                    break;
                }
            }
        }
        // Change screen set statusAlarmIndex when user pressed mode button

        if (buttonWhichPressed == "Mode")
        {
            // Serial.println("Pipeline: Change screen set statusAlarmIndex when user pressed mode button");
            // Loop to setting statusAlarmIndex
            ledScreen.showStatusBoxScreen();
            while (true)
            {
                // get status of alarm from buttons
                String buttonWhichPressed = button.pressWhichButton();
                if (buttonWhichPressed == "Up")
                {
                    statusAlarmIndex = 1;
                    break;
                }
                else if (buttonWhichPressed == "Down")
                {
                    statusAlarmIndex = 2;
                    break;
                }
                else if (buttonWhichPressed == "Run")
                {
                    statusAlarmIndex = 0;
                    break;
                }
            }

            // set alarming time if status is ON or OFF
            if (statusAlarmIndex != 0)
            {
                alarmingHour = 0;
                alarmingMinute = 0;
                // get hour from keypad
                while (true)
                {
                    ledScreen.setTimeBoxScreen(alarmingHour, alarmingMinute);
                    String buttonWhichPressed = button.pressWhichButton();
                    alarmingHour = button.setHourSetting(alarmingHour, buttonWhichPressed);

                    // Change to set alarmingMinute
                    if (buttonWhichPressed == "Run")
                    {
                        break;
                    }
                }

                // get alarming minute
                while (true)
                {
                    String buttonWhichPressed = button.pressWhichButton();
                    alarmingMinute = button.setMinuteSetting(alarmingMinute, buttonWhichPressed);

                    ledScreen.setTimeBoxScreen(alarmingHour, alarmingMinute);
                    // Change to set box
                    if (buttonWhichPressed == "Run")
                    {
                        break;
                    }
                }
            }
            // Check duplicated "alarmingMinute", "alarmingHour"
            int indexDuplicate = -1; // flag

            // Traversal each box to check duplicate with alarmingTime
            for (int i = 0; i < 5; i++)
            {
                Box box = arrayOfBoxes[i];

                // Compare to with currentTime
                if (box.hourSetting == alarmingHour && box.minuteSetting == alarmingMinute)
                {
                    indexDuplicate = i;
                    break;
                }
            }

            // If there is a duplicate, then ask user to change time
            if (indexDuplicate != -1)
            {
                arrayOfBoxes[indexDuplicate].updateInfor(statusAlarmIndex, alarmingHour, alarmingMinute);
                alarmingHour = -1;
                alarmingMinute = -1;
            }

            // Not duplicate
            else
            {
                int chosenBoxIndex = 0;
                // Loop to choose box to set time
                while (true)
                {
                    Box boxInfor = arrayOfBoxes[chosenBoxIndex];
                    ledScreen.showBoxInforScreen(chosenBoxIndex, boxInfor.hourSetting, boxInfor.minuteSetting, boxInfor.statusAlarmIndex);
                    String buttonWhichPressed = button.pressWhichButton();
                    if (buttonWhichPressed == "Up")
                    {
                        chosenBoxIndex += 1;
                        chosenBoxIndex = (chosenBoxIndex + 10) % 5;
                    }
                    else if (buttonWhichPressed == "Down")
                    {
                        chosenBoxIndex -= 1;
                        chosenBoxIndex = (chosenBoxIndex + 10) % 5;
                    }

                    if (buttonWhichPressed == "Run")
                    {
                        arrayOfBoxes[chosenBoxIndex].updateInfor(statusAlarmIndex, alarmingHour, alarmingMinute);
                        alarmingHour = -1;
                        alarmingMinute = -1;
                        break;
                    }
                }
            }
            // back to the 1st screen
            break;
        }
    }
}