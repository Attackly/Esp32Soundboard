#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1
#define SDA_PIN       8
#define SCL_PIN       9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;


// Rotary encoder pins
const int trackA = 40;
const int trackB = 37;
const int volA = 4;
const int volB = 5;

int lastTrackState = LOW;
int lastVolState = LOW;

int currentTrack = 1;
int lastTrack = 1;

int volume = 15;
int lastVolume = 15;

const int buttonPin = 1;


void setup() {
pinMode(trackA, INPUT_PULLUP);
pinMode(trackB, INPUT_PULLUP);
pinMode(volA, INPUT_PULLUP);
pinMode(volB, INPUT_PULLUP);
pinMode(buttonPin, INPUT_PULLUP);


  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("Stating Init!");
  display.display();
  delay(500);

  mySerial.begin(9600, SERIAL_8N1, 16, 17);  // RX, TX pins for ESP32

  display.clearDisplay();
  int retrys = 0;
  if (!myDFPlayer.begin(mySerial)) {
    display.println("DFMini not responding!");
    display.display();
    display.clearDisplay();
    display.println("Try: ");
    display.println(retrys);
    display.display();
    delay(500);
    while (retrys <50);
  }
  display.clearDisplay();
  display.println("DF Mini Success");
  display.display();
  delay(500);

  display.clearDisplay();
  display.println("Configuring DFMINI");
  display.display();
  delay(500);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
  myDFPlayer.volume(volume);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

}
void loop() {
  // --- Track Encoder ---
  int trackAState = digitalRead(trackA);
  if (trackAState != lastTrackState) {
    if (digitalRead(trackB) != trackAState) {
      currentTrack++;
    } else {
      currentTrack--;
    }
    currentTrack = constrain(currentTrack, 1, 255);
    lastTrackState = trackAState;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Track: ");
    display.println(currentTrack);
    display.setCursor(0, 20);
    display.print("Volume: ");
    display.println(volume);
    display.display();
  }

  // --- Volume Encoder ---
  int volAState = digitalRead(volA);
  if (volAState != lastVolState) {
    if (digitalRead(volB) != volAState) {
      volume++;
    } else {
      volume--;
    }
    volume = constrain(volume, 0, 30);
    lastVolState = volAState;
    myDFPlayer.volume(volume);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Track: ");
    display.println(currentTrack);
    display.setCursor(0, 20);
    display.print("Volume: ");
    display.println(volume);
    display.display();
  }

  // --- Button to Play ---
  if (digitalRead(buttonPin) == LOW) {
    myDFPlayer.play(currentTrack);
    display.setCursor(0, 40);
    display.println("Playing...");
    display.display();
    delay(500); // debounce
  }

  delay(5); // Small delay for stability
}
