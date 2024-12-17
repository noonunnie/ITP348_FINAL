/* kaelyn cho
 * itp 348 final project - postal puppy
 * Date: 12/17/2024
 */
// see end of code for "MAIN STATE MACHINE PSUEDOCODE"

#include "Particle.h"
#include "SparkFunMicroOLED.h"  // Include MicroOLED library

// RFID
SYSTEM_MODE(AUTOMATIC);
SerialLogHandler logHandler(LOG_LEVEL_WARN);
#include <MFRC522.h>
#include <SPI.h>
constexpr uint8_t RST_PIN = S4;  // Reset pin
constexpr uint8_t SS_PIN = A5;   // Slave select pin
String card_K = "AE B2 83 BD";  // Kaelyn card
String card_J = "7E 84 8B BD";  // Jason card
String card_active = "00 00 00 00";  // Initialize card to use
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


// Buttons
const int BUTT_J = D9;  // Jason 
const int BUTT_K = D8;  // Kaelyn
const int BUTT_V = S3;  // variable recipient
// LEDs
const int LED_GREEN = D3; // green
const int LED_RED = D2;   // red

const int SWITCH = D10; // Magnetic switch

// Servo
const int SERVO = A2;
Servo LOCK;

// OLED
MicroOLED oled(MODE_I2C, 9, 1);    // Example I2C declaration RST=D7, DC=LOW (0)
// messages
String message_ready[3] = {"Packing a","lot of luv","<3"};
String message_close[3] = {"plz CLOSE","box before","sending..."};
String message_J[2] = {"A gift","for Jason"};
String message_K[2] = {"A gift","for Kaelyn"};
String message_V[3] = {"A gift","for you", "!!!"};
String message_scent[3] = {"Give me","the scent", "=o3"};
String message_denied[4] = {"Hey...","who","are", "YOU?"};
String message_open[4] = {"...","OpEn","SeSaMeEeE", "..."};


// timers
unsigned long currMillis = 0;
// blinking LED
const int BLINK_PERIOD = 300;
int blink = LOW;
unsigned long prevMillisBlink = 0;
// inital state
const int PUBLISH_PERIOD = 5000;
unsigned long prevMillisPublish = 0;
String print_state = "IDLE";
String print_recipient = "nobody";
int print_locked = 0;

// enum State for main state machine
enum State {IDLE, PROGRAM, LADEN};
State currentState = IDLE;

// set variables
void setup() {
  Serial.begin(9600);
  // inputs
  pinMode(BUTT_J, INPUT);
  pinMode(BUTT_K, INPUT);
  pinMode(BUTT_V, INPUT);
  pinMode(SWITCH, INPUT);

  // outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  LOCK.attach(SERVO);
  LOCK.write(60);

  // // motor
  // argon_ble_setup();
  // pinMode(AIN1, OUTPUT);
  // pinMode(AIN2, OUTPUT);
  // pinMode(BIN1, OUTPUT);
  // pinMode(BIN2, OUTPUT);
  // pinMode(PWMA, OUTPUT);
  // pinMode(PWMB, OUTPUT);

  // OLED
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(300);     // Delay 1000 ms
  oled.clear(PAGE); // erase flame logooled.clear(PAGE);            // Clear the display
  oled.setFontType(0);         // Smallest font

  // RFID
  while (!Serial);  // Wait until serial is ready (optional for Particle boards)
  SPI.begin();                        // Initialize SPI bus
  mfrc522.PCD_Init();                 // Initialize MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Output card reader version
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

/* FUNCTION FOR PRINTING ON OLED */
void printer(String message[], int size) {
  oled.clear(PAGE);  // Clear the display

  int middleX = oled.getLCDWidth() / 2 - 3;
  int middleY = oled.getLCDHeight() / (size + 2);
      
  int len = 0;
  for(int i = 0; i < size; i++){
      len = message[i].length();
      // Try to set the cursor in the middle of the screen
      oled.setCursor(middleX - (oled.getFontWidth() * len) / 2,
                    (i+1)*middleY);
      oled.print(message[i]);
      Serial.printf(message[i], " ");
  }
  oled.display();
}
// /* ASYNCHRONOUS MODULE */
// void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
//     //bluetoothSerialDebug(data, len); // uncomment for serial monitor debug
//     if (data[0] == '!' && data[1] == 'B' && data[2] == '5' && data[3] == '1') {
//         Serial.println("Button Up pressed");
//         forward();
//     } else if (data[0] == '!' && data[1] == 'B' && data[2] == '6' && data[3] == '1') {
//         Serial.println("Button Down pressed");
//         backward();
//     } else if (data[0] == '!' && data[1] == 'B' && data[2] == '7' && data[3] == '1') {
//         Serial.println("Button Left pressed");
//         left();
//     } else if (data[0] == '!' && data[1] == 'B' && data[2] == '8' && data[3] == '1') {
//         Serial.println("Button Right pressed");
//         right();
//     } else if (data[0] == '!' && data[1] == 'B' && data[2] == '1' && data[3] == '1') {
//         Serial.println("Circle Clockwise");
//         counterwise();
//     } else if (data[0] == '!' && data[1] == 'B' && data[2] == '2' && data[3] == '1') {
//         Serial.println("Circle Counterclockwise");
//         clockwise();
//     } else if (data[3] == '0'){
//         stop();
//     }
// }

void loop() {
  // get current values for time, buttons, and magnetic switch
  currMillis = millis();
  int OPEN = digitalRead(SWITCH);
  int J = !digitalRead(BUTT_J);
  int K = !digitalRead(BUTT_K);
  int V = !digitalRead(BUTT_V);

  // publish to initial state
  if (currMillis - prevMillisPublish > PUBLISH_PERIOD){
    prevMillisPublish = currMillis;
    Particle.publish("machine_state", print_state, PRIVATE);
    if (OPEN){
      Particle.publish("box_status", "OPEN", PRIVATE);
    }
    else{
      Particle.publish("box_status", "CLOSED", PRIVATE);
    }
    Particle.publish("box_locked", String(print_locked), PRIVATE);
    Particle.publish("recipient", print_recipient, PRIVATE);
  }
    
  // inputs: time, magnetic switch, buttons
  switch (currentState) {
    // waiting to be assigned a recipient (note: box must be closed to assign recipient)
    case IDLE:
      print_state = "IDLE";
      print_recipient = "nobody";
      // output
      digitalWrite(LED_GREEN, HIGH);
      // only assign recipient if box closed
      if(OPEN == HIGH)
      {
        // blink red if box open
        if (currMillis - prevMillisBlink > BLINK_PERIOD){
          prevMillisBlink = currMillis;
          blink = !blink;
          digitalWrite(LED_RED, blink);
        }
        if (J || K || V){
          // OLED message "please close box" // Serial.println("please close box"); // debug
          printer(message_close, 3);
        } else {
          printer(message_ready, 3);
        }
      }
      else {
        digitalWrite(LED_RED, LOW);  // red light off if closed
        // changes states based off button press
        if (J) {
          // assign recipient as Jason
          card_active = card_J;
          // lock box (servo)
          LOCK.write(15);
          print_locked = 1;
          // OLED message "A gift for Jason" // Serial.println("A gift for Jason"); // debug
          printer(message_J, 2);
          print_recipient = "Jason";
          currentState = LADEN;
        } else if (K){
          // assign recipient as Kaelyn
          card_active = card_K;
          // lock box (servo)
          LOCK.write(15);
          print_locked = 1;
          // OLED message "A gift for Kaelyn" // Serial.println("A gift for Kaelyn"); // debug
          printer(message_K, 2);
          print_recipient = "Kaelyn";
          currentState = LADEN;
        } else if (V){
          // lock box (servo)
          LOCK.write(15);
          print_locked = 1;
          // OLED message "Give me the scent :>3" // Serial.println("Give me the scent :>3"); // debug
          printer(message_scent, 3);
          currentState = PROGRAM;
        }
      }
    break;
    case PROGRAM:
      print_state = "PROGRAMMING ID";
      // blink green in this state
      if (currMillis - prevMillisBlink > BLINK_PERIOD) {
        prevMillisBlink = currMillis;
        blink = !blink;
        digitalWrite(LED_GREEN, blink);
      }
      // RFID
      if (mfrc522.PICC_IsNewCardPresent()) // Check if a new card is present
      {// Read the card's UID
        card_active = "";
        if (mfrc522.PICC_ReadCardSerial()) 
        {// Construct the UID as a string
            for (byte i = 0; i < mfrc522.uid.size; i++) 
            {
                if (mfrc522.uid.uidByte[i] < 0x10){
                    card_active += " 0";
                }
                else{
                    card_active += " ";
                }
                card_active += String(mfrc522.uid.uidByte[i], HEX);
            }
            card_active.toUpperCase();
            card_active.trim();  // Remove any leading/trailing spaces
            mfrc522.PICC_HaltA();// Stop card communication
            // OLED message "A gift for you!" // Serial.println("A gift for you!"); // debug
            printer(message_V, 3);
            print_recipient = "somebody";
            currentState = LADEN;
        }
      }

    break;
    case LADEN:
      print_state = "LADEN WITH GIFTS";
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
        // RFID
      String scannedCard = "";  // To store the scanned card UID
      // Check if a new card is present
      if (mfrc522.PICC_IsNewCardPresent()) 
      {
        // Read the card's UID
        if (mfrc522.PICC_ReadCardSerial()) {
            // Construct the UID as a string
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                if (mfrc522.uid.uidByte[i] < 0x10)
                  {scannedCard += " 0";}
                else
                  {scannedCard += " ";}
                scannedCard += String(mfrc522.uid.uidByte[i], HEX);
            }
            scannedCard.toUpperCase();
            scannedCard.trim();  // Remove any leading/trailing spaces
            Serial.println(String(scannedCard)); // debug
            Serial.println(String(card_active)); // debug
            // Match against stored card IDs
            if (scannedCard == card_active) {
                Serial.println("Access granted");
                // unlock box (servo)
                LOCK.write(60);
                print_locked = 0;
                // TODO: OLED message "Open sesame!" // Serial.println("Open sesame!"); // debug
                printer(message_open, 4);
                currentState = IDLE;
            } else {
                Serial.println("Access denied");
                printer(message_denied, 4);
            }
            // Stop card communication
            mfrc522.PICC_HaltA();
        }
      }
    break;
    }
}

/*    "MAIN STATE MACHINE PSUEDOCODE"
IDLE: waiting for button press
  GREEN LED: ON
  if box open
    RED LED: BLINK
    if B1 or B2 or B3 pressed
      error message: "close the box please!" (OLED)
  else
    RED LED: OFF
    if B1 pressed
      ID = Jason
      lock box (servo)
      message: "A gift for Jason"
      STATE: LADEN
    else if B2 pressed
      ID = Kaelyn
      lock box (servo)
      message: "A gift for Kaelyn"
      STATE: LADEN
      lock box (servo)
    else if B3 pressed
      STATE = PROGRAM_ID
PROGRAM_ID: waiting to program recipient
  GREEN LED: BLINK
  if card_scanned
    ID = card_scanned
    message: "A gift for you"
    STATE: LADEN
  else if B1
    message: "No gift for you"
    STATE: IDLE
  else if B3
    message: "A gift for you... again"
LADEN: waiting for recipient authorization
  GREEN LED: OFF
  RED LED: ON
  if card_scanned == ID
    message: "open sesame!"
    unlock
    STATE: IDLE
  if secret_code (nested state machine for a 4 digit pin)
    message: "I've been hacked"
    unlock
*/ 