//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico, not the arduino one
#include <RPi_Pico_TimerInterrupt.h>
#include <RPi_Pico_ISR_Timer.h>
#include <RPi_Pico_ISR_Timer.hpp>

#include <vector>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//=========================================================PINS===============================================
//setting up screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_MOSI   5 //"D1" pin 7
#define OLED_CLK   2 //"D0" pin 4
#define OLED_DC    4 //"DC" pin 1
#define OLED_CS    3 //"CS" pin 5
#define OLED_RESET 1 //"RES" pin 7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

unsigned const char screenWidth = 128;
unsigned const char screenHeight = 64;

const char x_Pin = 26;
const char y_Pin = 27;
const char joy_press_Pin = 16;
const char new_Pin = 11;
const char select_Pin = 0;//CHANGE
const char shift_Pin = 13;
const char delete_Pin = 14;
const char play_Pin = 15;
const char track_clk_Pin = 6;
const char note_clk_Pin = 20;
const char track_data_Pin =7;
const char note_data_Pin = 19;
const char note_press_Pin = 18;
const char track_press_Pin = 8;
const char UART0_RX_Pin = 17;
const char OUT_Pin = 16;
const char loop_Pin = 12;

// 'eyes', 128x64px
const unsigned char eyes_bmp [] PROGMEM = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 
  0x17, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x23, 0xfb, 0xff, 0xff, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x78, 0x07, 0xbf, 0xef, 0xf7, 
  0x3f, 0xd9, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xe8, 0x00, 0x0c, 0xff, 0xef, 0xff, 
  0x1c, 0x01, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x4e, 0x00, 0x00, 0x00, 0x9f, 0xff, 0xff, 
  0x24, 0x00, 0x47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfb, 0xff, 
  0x00, 0x00, 0x01, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0x00, 0x00, 0x00, 0x07, 0x77, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x1f, 0xff, 
  0x80, 0x00, 0x00, 0x00, 0x04, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x1f, 0xd6, 0xc0, 0x0f, 0xff, 
  0x80, 0x0a, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x8c, 0x02, 0x08, 0xff, 0xef, 0xf0, 0x0f, 0xfe, 
  0x80, 0x3f, 0xc0, 0x00, 0x00, 0x3f, 0xff, 0xfe, 0x42, 0x0b, 0x6f, 0xff, 0xff, 0xff, 0x8f, 0xff, 
  0x80, 0xff, 0xfe, 0x82, 0x00, 0xbf, 0xff, 0xfa, 0x00, 0xfb, 0xff, 0xff, 0xff, 0xfe, 0xf6, 0xff, 
  0x85, 0xff, 0xff, 0xf6, 0x60, 0x17, 0xff, 0xfd, 0xcc, 0xf7, 0xef, 0xfe, 0xff, 0xff, 0xdf, 0xfb, 
  0x4f, 0xff, 0xff, 0xfd, 0xee, 0xaf, 0xff, 0xff, 0xec, 0x3f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xef, 
  0x8b, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xfd, 0xf7, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xdf, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0x73, 0xff, 0xff, 0xff, 0xe7, 0xf7, 0xff, 0xff, 
  0x7b, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0x7f, 0xdf, 0xff, 0x7f, 0xfd, 0x7f, 0xff, 0xff, 
  0xfd, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xfd, 0xbf, 0x3b, 0x7f, 0x7f, 0xf7, 0xff, 0xff, 0xff, 
  0xfb, 0xe7, 0xff, 0xff, 0xf4, 0xe3, 0xff, 0xff, 0xff, 0xf7, 0xf9, 0xff, 0x81, 0x7f, 0xff, 0xff, 
  0x7f, 0xe5, 0xff, 0xff, 0xff, 0xb1, 0xff, 0xff, 0xff, 0xc5, 0x71, 0xff, 0xef, 0x3f, 0xff, 0xff, 
  0xff, 0xf0, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0x48, 0x7f, 0xfd, 0xf2, 0xbf, 0xff, 0xff, 
  0xff, 0xf8, 0xff, 0xfd, 0xdc, 0x79, 0xff, 0xff, 0xff, 0x80, 0x0f, 0xfe, 0xe0, 0xff, 0xff, 0xff, 
  0xff, 0xf8, 0x1f, 0xd8, 0x10, 0x33, 0xff, 0xff, 0xff, 0x80, 0x00, 0x50, 0x01, 0xff, 0xff, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x09, 0xef, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 
  0xff, 0xf8, 0x00, 0x00, 0x09, 0xf3, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x0f, 0xcb, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 
  0x7f, 0xf0, 0x00, 0x00, 0x1f, 0xcf, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 
  0xff, 0xe0, 0x00, 0x00, 0x5f, 0xf7, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 
  0xff, 0xe0, 0x00, 0x06, 0xff, 0xa1, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x00, 0x03, 0xff, 0xff, 0xff, 
  0x7f, 0xf4, 0x00, 0x09, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xcc, 0x00, 0x0f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xc1, 0x9f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x7f, 0xff, 0xff, 0xff, 0xfb, 0xf5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 1040)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
  eyes_bmp
};


//USB MIDI object
Adafruit_USBD_MIDI usb_midi;
//
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
//Serial1.setRX(pin)
//Serial1.setTX(pin)
//MIDI_CREATE_INSTANCE(MIDI_OUT, Serial1, MIDI);

using namespace std;

bool trackSolo;//if a track is being solo'd
float bpm = 120;

bool playing = false;
bool listening = false;
bool recording = false;
bool gotClock = false;
bool startRec = false;
bool helloChild = false;
bool overwrite = true;
bool notLooping = false;
bool isArping = false;
bool menuIsActive = false;
volatile bool rotaryActionA;//set to true when rotary encoders are moved
volatile bool rotaryActionB;

short int cursorPos; //cursor position (before playing)
unsigned short int trackNumber; //number of tracks
unsigned short int activeLoop;//loop that's currently active
unsigned short int activeTrack; //sets which track you're editing, you'll only be able to edit one at a time
unsigned char subDivInt;//sets where the divider bars are in the console output
//cursor jump is locked to this division

unsigned short int loopStart;//where the loop starts, for play mode
unsigned short int loopEnd; //where the loop ends, for play mode
unsigned short int loopIterations;//number of times the loop repeats
unsigned short int loopCount;//controls how many times sequence has looped
bool isLooping = true;//controls whether or not the sequence loops at all
bool isInfinite;//true if loop should repeat infinitely
vector<unsigned short int> loopSeq;//controls the order of loops

unsigned short int viewStart;//where the view ends, usually moves by measures but shift lets it move one at a time
unsigned short int viewEnd;//where the view ends
unsigned short int viewLength;//how big the view is
unsigned short int seqStart;
unsigned short int seqEnd;

unsigned short int defaultLength;
unsigned short int recentID;

unsigned short int playheadPos;
unsigned short int recheadPos;

unsigned const char debugHeight = 16;
unsigned char trackDisplay;
// unsigned short int stepLength;
float stepLength;
unsigned char trackHeight;
unsigned char maxTracksShown = 5;
unsigned char startTrack = 0;
unsigned char endTrack;

unsigned long lastTime;

unsigned char const blank_ID = 0;//just a 0, as an unsigned char
unsigned int const tail_ID = 65535;//last address, for note tail_IDs
unsigned char defaultPitch;
unsigned char defaultChannel;
unsigned char defaultVel;//default velocity;

int timestep;

//Function prototypes ----------------------------->
void keyListen();
void debugNoteCount();
void setActiveTrack(int, bool);
void setBpm();
void increaseBpm();
void clearSelection();
void clearSelection(int, int);
void selectNote(bool, int, int);
void selectAll(bool, bool, bool);
void debugPrint();
unsigned short int getRecentNoteID();
unsigned short int getRecentNoteID(int, int);
void displaySeq();
void displaySeqSerial();
void initSeq();
void addTrack(unsigned char);
void addTrack(unsigned char, bool);
void eraseTrack();
void deleteTrack();
void eraseSeq();
void setTrackPitch(unsigned char);
void setTrackPitch(int, unsigned char);
void deleteSelectedNotes();
void makeNote(int, bool);
void makeNote(int, int, int, bool);
void moveToClosestNote(unsigned char, bool);
bool isInView(int);
void drawNote(unsigned short int, unsigned short int, unsigned short int, unsigned short int, unsigned short int, bool);
void deleteTracks();
int getTrackWithPitch(int);
unsigned short int addTrack_return(unsigned short int,bool);
void echoPreview(short unsigned int, short unsigned int, short unsigned int);
void curvePreview();

//Classes-----------------------------------------------------------
//storage efficient Note class:
class Note {
  private:
  
  public:
    Note();
    Note(unsigned short int, unsigned short int, unsigned char, unsigned char, unsigned char);
    Note(unsigned short int, unsigned char, unsigned char, unsigned char); //for no endPos
    Note(unsigned short int, unsigned short int, unsigned short int);
    bool isSelected;
    unsigned char velocity;
    unsigned char trackID;
    unsigned short int startPos;
    unsigned short int endPos;
    unsigned char chance;
    char qOffSet;//for storing how far offset the note is, when playing/displaying it
};

//default constructor
Note::Note() {
  startPos = cursorPos;
  endPos = cursorPos + subDivInt;
  velocity = defaultVel;
  trackID = activeTrack;
  isSelected = false;
  qOffSet = 0;
  chance = 100;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel, unsigned char track, unsigned char odds) {
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  trackID = track;
  isSelected = false;
  qOffSet = 0;
  chance = odds;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned short int vel){
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  isSelected = false;
  qOffSet = 0;
  chance = 100;
}
Note::Note(unsigned short int xPos, unsigned char vel, unsigned char track, unsigned char odds){
    startPos = xPos;
    endPos = xPos + defaultLength;
    velocity = vel;
    trackID = track;
    isSelected = false;
    qOffSet = 0;
    chance = odds;
}

//track class
class Track {
  public:
    Track();
    Track(unsigned char, unsigned char); //constructs with a midi channel input
    unsigned char pitch; //for storing which midi note the track sends and receives
    unsigned char channel;//for storing midi channels
    bool mute;//if a track is muted, I think shift/mute should solo
    bool isSending;//if a note is currently being sent
    bool isSelected;
};

//constructor for each track
Track::Track() {
  pitch = defaultPitch;//c1 to start
  channel = defaultChannel;
  mute = false;
  isSelected = false;
  isSending = false;
}

Track::Track(unsigned char p, unsigned char c){
  pitch = p;
  channel = c;
  mute = false;
  isSending = false;
  isSelected = false;
}

//arpeggiator objects! these store which notes they play (scale), in what order (order), how fast (subDiv)
//what does an arpeggiator need? which notes to play, how fast to play them, and in what order
//also, randomness
//arps don't create notes in data, unless you commit. They play 'virtual notes', so no data is actually created unless you want it to be
class Arp{
  public:
    Arp();
    vector<unsigned short int> scale;
    vector<unsigned short int> order;
    // unsigned short int *randNotes[];//which notes to randomize
    unsigned short int startPitch;
    unsigned short int randomness;
    unsigned short int subDiv;
    unsigned short int range;
    void commit();
    void setScaleLive();//sets the scale to whatever notes are being played atm
    void setOrder(unsigned short int []);
    void setSubDiv(unsigned short int);
    void setRandom(unsigned short int, unsigned short int);
    void up();
    void down();
    void downUp();
    void upDown();
    void debugPrintArp();
    void constructArp();
};

Arp::Arp(){
  startPitch = 36;//start on c3
  subDiv = subDivInt;//default is whatever current subdiv is
  randomness = 0;
  range = 0;
  scale = {0,2,4,5,7,9,11}; //default is major scale
  order = {0,1,2,3,4,5,6,7};//default is 'up'
}
//range

//this destroys the current seq and replaces it with the arppegiation
void Arp::commit(){
  Serial.println("deleting tracks");
  deleteTracks();
  for(int i = 0; i<order.size(); i++){//setting notes in scale[] as referenced by order[], moving through order (sizeof thing gets the length of order arr)
    int note = order[i];
    int octave = 0;
    while(note>scale.size()-1){//if it's a note outside the scale, go up according to the range
      note = note-scale.size();
      octave++;
    }
    unsigned short int pitch = startPitch+scale[note]+12*octave;
    Serial.println("regenerating tracks");
    Serial.println(order[i]);
    Serial.println(scale[note]);
    int track = getTrackWithPitch(pitch);
    if(track == -1){
      Serial.println("adding tracks (where necessary)");
      track = addTrack_return(pitch,false);//adds a new track with the correct pitch
    }
    Serial.println("making note");
    makeNote(track, seqStart+i*subDiv, subDiv, false);//makes a note
  }
}

// void Arp::up(){
//   vector<unsigned short int> newOrder[scale.size()];//here, order needs to be the length of scale so each note is played
//   for(int i = 0; i<sizeof(scale)/sizeof(scale[0]), i++){//loop runs once for each val in scale
//     order[i] = i;
//   }
// }

// void Arp::down(){
// unsigned short int newOrder[scale.size()];//here, order needs to be the length of scale so each note is played
//   for(int i = 0; i<sizeof(scale)/sizeof(scale[0]), i++){//loop runs once for each val in scale
//     order[i] = sizeof(scale)/sizeof(scale[0])-i;
//   }
// }

void Arp::debugPrintArp(){
  Serial.println("scale:");
  Serial.print("{");
  for(int i = 0; i<scale.size(); i++){
    Serial.print(scale[i]);
    Serial.print(",");
  }
  Serial.println("}");
  Serial.print("order:");
  for(int i = 0; i<order.size(); i++){
    Serial.print(order[i]);
    Serial.print(",");
  }
  Serial.println("}");
}

class Curve{
  public:
  Curve();
  vector<short int> points;//stores the y-vals
  unsigned short int period;
  unsigned char function;//stores lookup to which function to generate it with
  unsigned char phase;
  unsigned char amplitude;
  unsigned short int length;//stores over what range the curve is applied to
  short int xAxis;//vertical translation
  bool absolute;//whether or not the curve adds/substracts to, or replaces the values
};

Curve::Curve(){
  function = 1;//default is a sine wave
  phase = 0;//default is 0º
  amplitude = 127;
  absolute = false;
  length = 128;
  xAxis = screenHeight/2;
  period = screenWidth/10;
}

Curve debugCurve;

void genCurve(Curve curve){//this creates the points, then draws them
  debugCurve.points.erase(debugCurve.points.begin(), debugCurve.points.end());
  for(float point = 0; point<=curve.length; point++){
    if(curve.function == 1){
      curve.points.push_back(curve.amplitude*sin(PI*point/curve.period));
      Serial.println(curve.amplitude*sin(PI*point/curve.length));
    }
  }
}

void genCurveDebug(){//this creates the points, then draws them
  debugCurve.points.erase(debugCurve.points.begin(), debugCurve.points.end());
  for(float point = 0; point<=debugCurve.length; point++){
    float p = debugCurve.amplitude*sin(PI*point/debugCurve.period);
    debugCurve.points.push_back(p);
    Serial.println(p);
  }
}

void drawCurve(Curve curve){
  display.clearDisplay();
  unsigned short int stepSize = screenWidth/curve.length;
  for(int point = 0; point<curve.points.size(); point++){
    int x1 = point*stepSize;
    int x2 = x1+stepSize;
    int y1 = (curve.points[point])/16+curve.xAxis;//so it starts in da middle
    int y2 = (curve.points[point+1])/16+curve.xAxis;
    display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
    // display.drawPixel(x1,y1,SSD1306_WHITE);
  }
  display.display();
}

void serialPrintCurve(Curve curve){

  genCurveDebug();
  Serial.println("points:");
  for(int i = 0; i<curve.points.size(); i++){
    Serial.print(curve.points[i]);
    Serial.print(',');
  }
  drawCurve(debugCurve);
}
vector<String> debugOptions = {"debug","idk","i love you","jk"};
//Menu object
class Menu{
  public:
  Menu();
  Menu(unsigned short int, unsigned short int, unsigned short int, unsigned short int, String, vector<String>,unsigned char);
  void displayMenu();
  void moveMenuCursor(bool);
  vector<String> options;
  unsigned short int highlight;
  unsigned short int topL[2];
  unsigned short int bottomR[2];
  bool isActive;
  String menuTitle;
  unsigned char style;//vertical and horizontal styles
};

Menu::Menu(){
  options = {"test","test","test"};
  isActive = false;
  highlight = 0;
  topL [0] = 0;
  topL [1] = 0;
  bottomR [0] = screenWidth;
  bottomR [1] = screenHeight;
  menuTitle = "test";
}

Menu::Menu(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2, String title, vector<String> ops,unsigned char s){
  topL [0] = x1;
  topL [1] = y1;
  bottomR [0] = x2;
  bottomR [1] = y2;
  options = ops;
  isActive = false;
  highlight = 0;
  menuTitle = title;
  style = s;
}

void Menu::moveMenuCursor(bool forward){
  if(forward && highlight<options.size()){
    highlight++;
  }
  else if(!forward && highlight>0){
    highlight--;
  }
}

Menu activeMenu;
Menu debugMenu(5,5,70,50,"debug",debugOptions,0);
Menu curveMenu(10,10,118,54,"Curve",debugOptions,1);

vector<String> arpOptions = {"arp","subDiv","type","Construct","Commit"};
Menu arpMenu(0,0,128,64,"arp",debugOptions,1);

vector<String> echoOptions = {"target","time","decay","repeats","Commit"};
Menu echoMenu(10,5,118,59,"Echo",echoOptions,1);
vector<short unsigned int>echoData = {24,50,3};

void Menu::displayMenu(){
  //drawing menu box
  display.fillRect(topL[0],topL[1], bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
  display.drawRect(topL[0],topL[1], bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_WHITE);
  display.setCursor(topL[0]+1,topL[1]+1);
  display.print(menuTitle);
  //drawing menu options, and the highlight
  unsigned short int textWidth = 20;
  unsigned short int textHeight = 10;

  if(menuTitle == "Echo"){
    echoPreview(echoData[0],echoData[1],echoData[2]);
  }
  if(menuTitle == "Curve"){
    curvePreview();
  }
  if(style == 0){//vertical, used for data
    for(int i = 0; i<options.size(); i++){
      display.setCursor(topL[0],(i+1)*textHeight+topL[1]);
      if(highlight != i)
        display.print(options[i]);
      else if(highlight == i){
        // shadeArea(topL[0],(i+1)*textHeight+topL[1],bottomR[0]-topL[0],textHeight,6);
        display.fillRect(topL[0],(i+1)*textHeight+topL[1],bottomR[0]-topL[0],textHeight,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
        display.print(options[i]);
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
      }
    }
  }
  if(style == 1){//horizontal, used for fx
    for(int i = 0; i<options.size(); i++){
      display.setCursor(i*textWidth+topL[0],topL[1]+40+(i/3)*textHeight);
      if(highlight != i)
        display.print(options[i]);
      else if(highlight == i){
        display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
        display.print(options[i]);
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
      }
    }
  }
}

//Data variables -------------------------------------------
Note offNote; //default note, always goes in element 0 of seqData for each track
Arp activeArp;

vector<Note> defaultVec;//default vector for a track, holds offNote at 0
vector<vector<unsigned short int> > lookupData; //char map of notes; 0 = no note, 1-665,535 = noteID
vector<vector<Note> > seqData(trackNumber, defaultVec);//making a 2D vec, number of rows = tracks, number of columns = usable notes, and stores Note objects
vector<Track> trackData;//holds tracks
vector<Track> soloTrack;//vector that holds the solo'd track
vector<unsigned short int> noteCount;//vector that holds the number of notes on each track
vector<unsigned short int> selectionBuffer;//for storing the ID's of selected notes, order is: {track, note, track, note...}
vector<Note> copyBuffer;//stores copied notes
vector<vector<unsigned short int>> loopData;

//FX
void commitEcho(){
}
void echoPreview(short unsigned int time, short unsigned int decay, short unsigned int repeats){
  unsigned short int previewLength = 24;
  drawNote(15,15,24,10,1,false);//pilot note
  float vel = 127;
  for(int rep = 1; rep<repeats+1; rep++){
    vel = vel*decay/100;
    short unsigned int x1 = 15+time*rep;
    short unsigned int y1 = 15;
    if(vel>125)
      drawNote(x1,y1,time,trackHeight,1,false);
    else if(vel>100)
      drawNote(x1,y1,time,trackHeight,2,false);
    else if(vel>80)
      drawNote(x1,y1,time,trackHeight,3,false);
    else if(vel>60)
      drawNote(x1,y1,time,trackHeight,4,false);
    else if(vel>40)
      drawNote(x1,y1,time,trackHeight,5,false);
    else if(vel>20)
      drawNote(x1,y1,time,trackHeight,6,false);
    else if(vel>0)
      drawNote(x1,y1,time,trackHeight,7,false);
    else if(vel == 0)
      drawNote(x1,y1,time,trackHeight,10,false);
  }
}
void createArp(){
  Arp newArp;
  activeArp = newArp;
}

void printArp(Arp arp){
  arp.debugPrintArp();
}

void commitArp(Arp arp){
  arp.commit();
}

// void arpFromSeq(){
//   vector<unsigned short int> scale;
//   vector<unsigned short int> order;
//   Arp newArp;
//   newArp.startPitch = getLowestPitch();
//   int count = 0;
//   for(int track = 0; track<trackNumber; track++){
//     for(int note = loopStart; note<loopEnd+1; note++){
//       if(lookupData[track][note] != 0){
//         if()
//         scale.push_back(trackData[track].pitch - newArp.startPitch);
//         order.push_back
//       }

//     }
//   }
//   for(int j = loopStart; j<loopEnd-1; j++){
//     for(int i = 0; i<trackNumber; i++){
//       if(seqData[i][lookupData[i][j]].startPos == j){//if there's a note here, add val to scale and reference it

//       }
//     }
//   }
// }

void curvePreview(){

}
//Notes ------------------------------------------------------------------------
unsigned short int getRecentNoteID(int track, int time) {
  unsigned short int recentID;
  for (int i = time; i>-1; i--) { //step backwards through the lookupdata until you find the start of the note
    recentID = lookupData[track][i];
    if (recentID != 0)
      break;
  }
  return recentID;
}

unsigned short int getRecentNoteID() {
  unsigned short int recentID;
  int time = cursorPos;
  int track = activeTrack;
  for (int i = time; i>-1; i--) { //step backwards through the lookupdata until you find the start of the note
    recentID = lookupData[track][i];
    if (recentID != 0)
      break;
  }
  return recentID;
}

//truncates any note in this position
void truncateNote(int track, int time) {
  int id = lookupData[track][time];
  for(int i = seqData[track][id].endPos; i>time-1; i--){//check this [CHECK]
    lookupData[track][i] = 0;
  }
  seqData[track][id].endPos = time-1;
}

//deletes a note that the cursor is on
void deleteNote() {
  //if there's a note/something here
  if (lookupData[activeTrack][cursorPos] != blank_ID) {
    noteCount[activeTrack] -= 1;//decrease the noteCount for the track
    unsigned short int targetNoteID = lookupData[activeTrack][cursorPos];//store the NoteID
    //erasing note from selection buffer
    if (seqData[activeTrack][targetNoteID].isSelected) {
      clearSelection(activeTrack, cursorPos);
    }
    //clearing tail
    for (int i = seqData[activeTrack][targetNoteID].startPos; i < seqData[activeTrack][targetNoteID].endPos + 1; i++) {
      if (lookupData[activeTrack][i] != 0) {
        lookupData[activeTrack][i] = 0;
      }
    }
    //erasing note from seqData
    seqData[activeTrack].erase(seqData[activeTrack].begin() + targetNoteID);
    //since we deleted it from seqData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the deleted note's ID should be decremented.
    for (int notes = 0; notes < seqEnd; notes++) {
      int changedNotes = 0;
      if ((lookupData[activeTrack][notes] > (targetNoteID)) && (lookupData[activeTrack][notes] != 0) && (changedNotes < noteCount[activeTrack])) { //if there's a higher note and if there are still notes to be changed
        lookupData[activeTrack][notes] -= 1;
        changedNotes++;
      }
    }
    //we also need to do this for the selectionBuffer
    for (int notes = 0; notes < selectionBuffer.size(); notes += 2) {
      if ((selectionBuffer[notes] == activeTrack) && (selectionBuffer[notes + 1] > targetNoteID)) { //only updating notes in the active track
        selectionBuffer[notes + 1] -= 1; //decrement the ID values by 1;
      }
    }
    //setting the note's lookup value to 0
    lookupData[activeTrack][cursorPos] = 0;
    }
}

void deleteNote(int track, int targetNoteID, bool deleteAllSelected) {
  if (targetNoteID <= seqData[track].size()) { //if the target note exists in seqData
    noteCount[track] -= 1;
    unsigned short int notePos = seqData[track][targetNoteID].startPos;//getting the position

    //erasing note from selection buffer
    if (seqData[track][targetNoteID].isSelected) {
      clearSelection(track, notePos);
    }
    //clearing tail
    for (int i = notePos; i < seqData[track][targetNoteID].endPos + 1; i++) {
      if (lookupData[track][i] == targetNoteID) {
        lookupData[track][i] = 0;
      }
    }
    seqData[track].erase(seqData[track].begin() + targetNoteID);
    //updating lookupData IDs
    //move through all notes, and if they have an ID>targetID, decrease it by 1
    //this way the values in lookupData still refer to the correct indices in seqData
    //also need to update selectionBuffer
    int changedNotes = 0;
    for (int notes = 0; notes < seqEnd; notes++) {
      if ((lookupData[track][notes] > (targetNoteID)) && (lookupData[track][notes] != 0) && (changedNotes < noteCount[track])) { //if there's a higher note, and if there are still notes to be changed
        lookupData[track][notes] -= 1;
        changedNotes++;
      }
    }//updating selectionbuffer
    for (int notes = 0; notes < selectionBuffer.size(); notes += 2) {
      if ((selectionBuffer[notes] == track) && (selectionBuffer[notes + 1] > targetNoteID)) { //only updating notes in the active track
        selectionBuffer[notes + 1] -= 1; //decrement the ID values by 1;
      }
    }
    lookupData[track][notePos] = 0;
  }
}
//this one is special since you'll know it's a real note ID, not a tail, and it needs to be ASAP
void deleteNoteWhileRecording(int track, int targetNoteID) {
    noteCount[track] -= 1;
    unsigned short int notePos = seqData[track][targetNoteID].startPos;//getting the position
    //erasing note from selection buffer
    if (seqData[track][targetNoteID].isSelected) {
      clearSelection(track, notePos);
    }
    //clearing tail
    for (int i = notePos; i < seqData[track][targetNoteID].endPos + 1; i++) {
      if (lookupData[track][i] == targetNoteID) {
        lookupData[track][i] = 0;
      }
    }
    seqData[track].erase(seqData[track].begin() + targetNoteID);
    //updating lookupData IDs
    //move through all notes, and if they have an ID>targetID, decrease it by 1
    //this way the values in lookupData still refer to the correct indices in seqData
    int changedNotes = 0;
    for (int notes = 0; notes < seqEnd; notes++) {
      if ((lookupData[track][notes] > (targetNoteID)) && (lookupData[track][notes] != 0) && (changedNotes < noteCount[track])) { //if there's a higher note, and if there are still notes to be changed
        lookupData[track][notes] -= 1;
        changedNotes++;
      }
    }//updating selectionbuffer
    for (int notes = 0; notes < selectionBuffer.size(); notes += 2) {
      if ((selectionBuffer[notes] == track) && (selectionBuffer[notes + 1] > targetNoteID)) { //only updating notes in the active track
        selectionBuffer[notes + 1] -= 1; //decrement the ID values by 1;
      }
    }
    lookupData[track][notePos] = 0;
}

//deletes selected notes, stored in selection buffer
void deleteSelectedNotes() {
  while (selectionBuffer.size() > 1) { //while there are selected notes
    deleteNote(selectionBuffer[selectionBuffer.size() - 2], selectionBuffer[selectionBuffer.size() - 1], true); //target the last note in the selection buffer. Delete all tag tells the deleteNote function to erase the last item in the buffer too, so it should just continuously work it's way down
  }
}


//this one is for quickly placing a ntoe at the cursor, on the active track
void makeNote(int length, bool loudly) {
  Note newNoteOn(cursorPos, (cursorPos + length-1), defaultVel, activeTrack, 100);
  if (lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos) { //if there's no note there
    if (lookupData[activeTrack][cursorPos] != 0) {
      truncateNote(activeTrack, cursorPos); //if there's a note tail_ID, truncate it
    }
    noteCount[activeTrack] += 1;//increasing note count
    unsigned short int id = noteCount[activeTrack];
    lookupData[activeTrack][cursorPos] = id;//set noteID in lookupData to the index of the new note
    for (int i = cursorPos; i < length-1; i++) {
      if (lookupData[activeTrack][i + 1] == 0) { //continues note, if there're no notes after it
        lookupData[activeTrack][i + 1] = id;
      }
      else { //if there's something there, then set the end to the step before it
        newNoteOn.endPos = i;
        break;
      }
    }
    if (loudly) {
      MIDI.sendNoteOn(trackData[activeTrack].pitch, newNoteOn.velocity, trackData[activeTrack].channel);
      MIDI.sendNoteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
    }
  }
  seqData[activeTrack].push_back(newNoteOn);//add note at next index (number of indices == number of notes)
}

void makeNote(int track, int time, int length, bool loudly) {
  Note newNoteOn(time, (time + length-1), defaultVel, track, 100);
  if (lookupData[track][time] == blank_ID || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos) { //if there's no note there
    if (lookupData[track][time] != 0) {
      truncateNote(track, time);
    }
    noteCount[track] += 1;//increasing note count
    unsigned short int id = noteCount[track];
    lookupData[track][time] = noteCount[track];//set noteID in lookupData to the index of the new note
    for (int i = time; i < length + time-1; i++) { //sets id
      if (lookupData[track][i + 1] == 0)
        lookupData[track][i + 1] = id;
      else { //if there's something there, then set the end to the step before it
        lookupData[track][i] = id;
        newNoteOn.endPos = i;
        break;
      }
    }
  }
  if (loudly) {
    MIDI.sendNoteOn(trackData[track].pitch, newNoteOn.velocity, trackData[track].channel);
    MIDI.sendNoteOff(trackData[track].pitch, 0, trackData[track].channel);
  }
  seqData[track].push_back(newNoteOn);//add note at next index (number of indices == number of notes)
}
//gets a note from data, returns a note object
Note getNote(int track, int timestep) {
  unsigned short int noteID;
  if (lookupData[track][timestep] != 0) {
    noteID = lookupData[track][timestep];
    Note targetNote = seqData[track][noteID];
    return targetNote;
  }
  else {
    Serial.print("No note, your majesty\n");
    return offNote;
  }
}


void changeVelSelected(int amount) {
  for (int i = 0; i < selectionBuffer.size(); i += 2) {
    seqData[selectionBuffer[i]][selectionBuffer[i + 1]].velocity += amount;
    int vel = seqData[selectionBuffer[i]][selectionBuffer[i + 1]].velocity;
    if(vel>127){
      seqData[selectionBuffer[i]][selectionBuffer[i + 1]].velocity = 127;
    }
    if(vel<0){
      seqData[selectionBuffer[i]][selectionBuffer[i + 1]].velocity = 0;
    }
  }
}

void changeVel(int amount) {
  if (selectionBuffer.size() > 1) {
    changeVelSelected(amount);
  }
  else {
    unsigned short int noteID = lookupData[activeTrack][cursorPos];
    int vel = seqData[activeTrack][noteID].velocity;
    vel += amount;
    seqData[activeTrack][noteID].velocity = vel;
    if(vel>=127){
      seqData[activeTrack][noteID].velocity = 127;
    }
    if(vel<0){
      seqData[activeTrack][noteID].velocity = 0;
    }
  }
}

//Tracks ------------------------------------------------------------------------

//returns id of track with a specific pitch, returns -1 if track doesn't exist
int getTrackWithPitch(int pitch){
  for(int i=0;i<trackNumber;i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}
//this function should add a new row to the seqData, and lookupData initialized with 64 zeroes
void addTrack(unsigned char pitch) {
  Track newTrack;
  newTrack.pitch = pitch;
  trackData.push_back(newTrack);//adding track to vector data
  seqData.resize(trackNumber + 1); //adding row to seqData
  noteCount.resize(trackNumber + 1); //adding a row to the noteCount vector, to count notes for this track
  lookupData.resize(trackNumber + 1); //adding a row to the lookupData vector, to store lookupID's for notes in this track
  lookupData[trackNumber].resize(seqEnd, blank_ID);//adding all the columns the track holds to the lookupData
  seqData[trackNumber] = defaultVec;//setting the note data for the new track to the default blank data
  activeTrack = trackNumber;
  trackNumber += 1;
  MIDI.sendNoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
  MIDI.sendNoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
}
void addTrack(unsigned char pitch, bool loudly) {
  Track newTrack;
  newTrack.pitch = pitch;
  trackData.push_back(newTrack);
  seqData.resize(trackNumber + 1);
  noteCount.resize(trackNumber + 1);
  lookupData.resize(trackNumber + 1);
  lookupData[trackNumber].resize(seqEnd, blank_ID);
  seqData[trackNumber] = defaultVec;
  activeTrack = trackNumber;
  trackNumber += 1;
  if(loudly){
    MIDI.sendNoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    MIDI.sendNoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
  }
}
unsigned short int addTrack_return(unsigned short int pitch, bool loudly) {
  Track newTrack;
  newTrack.pitch = pitch;
  trackData.push_back(newTrack);
  seqData.resize(trackNumber + 1);
  noteCount.resize(trackNumber + 1);
  lookupData.resize(trackNumber + 1);
  lookupData[trackNumber].resize(seqEnd, blank_ID);
  seqData[trackNumber] = defaultVec;
  activeTrack = trackNumber;
  trackNumber += 1;
  if(loudly){
    MIDI.sendNoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    MIDI.sendNoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
  }
  return (trackNumber-1);
}

void duplicateTrack(unsigned short int track){
  unsigned short int newTrackID = addTrack_return(trackData[track].pitch, false);
  for(unsigned short int note = 1; note < noteCount[track]; note++){
    Note newNote = seqData[track][note];
    seqData[newTrackID].push_back(newNote);
    noteCount[newTrackID]++;
  }
  updateLookupData_track(newTrackID);
}
//erases notes, but doesn't delete track
void eraseTrack() {
  seqData[activeTrack].resize(1);//truncating note data to just the first, blank note
  noteCount[activeTrack] = 0;//setting the note count to zero

  for (int i = 0; i < seqEnd; i++) { //erasing notes that were in the selectionBuffer, and setting all the lookupData values to 0
    clearSelection(activeTrack, i);
    lookupData[activeTrack][i] = blank_ID;
  }
}
void eraseTrack(int track) {
  seqData[track].resize(1);
  noteCount[track] = blank_ID;
  for (int i = 0; i < seqEnd; i++) {
    clearSelection(track, i);
    lookupData[track][i] = blank_ID;
  }
}
//erases every track
void eraseSeq() {
  Serial.print("erasing seq\n");
  for (int i = 0; i < trackNumber; i++) {
    eraseTrack(i);
  }
  clearSelection();
}

//CHECK do you need to update trackID's??? idk if this will cause problems
//deletes the track AND notes stored within it (from seqData and lookupData)
void deleteTrack() {
  if (trackNumber == 1) { //if there's only one track left, erase it but don't delete it
    eraseTrack();
  }
  else if (trackNumber > 1) { //if there's more than one track, delete the active track
    eraseTrack();
    trackData.erase(trackData.begin() + activeTrack);
    seqData.erase(seqData.begin() + activeTrack);
    lookupData.erase(lookupData.begin() + activeTrack);
    noteCount.erase(noteCount.begin() + activeTrack);
    if (endTrack = activeTrack) { //if the active track isn't the last track, then active the next track down
      activeTrack--;
      endTrack--;
      if(startTrack>0){
        startTrack--;
      }
    }
    trackNumber--;
  }
}

//as long as you delete tracks from the back, i think this is okay
void deleteTrack(unsigned short int track){
  eraseTrack(track);
  trackData.erase(trackData.begin() + track);
  seqData.erase(seqData.begin() + track);
  lookupData.erase(lookupData.begin() + track);
  noteCount.erase(noteCount.begin() + track);
  trackNumber--;
}

//deletes all tracks
void deleteTracks(){
  while(trackData.size()>0){
    deleteTrack(0);
  }
}

//sets the active track to a pitch
void setTrackPitch(unsigned short int note) {
  trackData[activeTrack].pitch = note;
}

void setTrackPitch(int track, unsigned short int note) {
  trackData[track].pitch = note;
}

void selectTrack(){
  trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
}

void selectTrack(unsigned short int id){
  trackData[id].isSelected = !trackData[id].isSelected;
}


//Debug -------------------------------------------------------------------------
//counts notes
void debugNoteCount() {
  int totalNotes = 0;
  for (int i = 0; i < noteCount.size(); i++) {
    totalNotes += noteCount[i];
  }
  Serial.print(totalNotes + " notes stored in memory\n");
}

//prints the selection buffer, doe
void debugPrint() {
  Serial.println("Printing out seqData...");
  for(int i = 0; i<trackNumber; i++){
    for(int j = 1; j<noteCount[i]+1; j++){
      Serial.println("+------------+");
      Serial.print("id: ");
      Serial.println(j);
      Serial.print("start: ");
      Serial.println(seqData[i][j].startPos);
      Serial.print("end: ");
      Serial.println(seqData[i][j].endPos);
    }
  }
  Serial.print("total notes on this track: ");
  Serial.print(int(noteCount[activeTrack]));
  Serial.print("total notes in the sequence: ");
  int totalNotes = 0;
  for (int i = 0; i < trackNumber; i++) {
    totalNotes = totalNotes + noteCount[i];
  }
  Serial.print(totalNotes);
  Serial.print("\nNotes in data: ");
  totalNotes = 0;
  for (int i = 0; i < trackNumber; i++) {
    totalNotes = seqData[i].size() - 1 + totalNotes;
  }
  Serial.print(totalNotes);
  Serial.print("Notes in the copy buffer: ");
  Serial.print(copyBuffer.size());
  Serial.print("selected notes:\n");
  Serial.print("+---------------+\n");

  if (selectionBuffer.size() > 0) {
    for (int i = 0; i < (selectionBuffer.size() - 1); i += 2) {
      Serial.print("| track:  ");
      Serial.print(int(selectionBuffer[i]));
      Serial.print("| noteID: ");
      Serial.print(int(selectionBuffer[i + 1]));
      Serial.print("\n");
    }
  }
  else {
    Serial.print("| no notes baby!\n");
  }
  Serial.print("+---------------+\n");
}

//fills sequence with notes
void debugFillSeq() {
  Serial.print("filling with notes\n");
  for (int i = 0; i < trackNumber; i++) {
    for (int j = 0; j < 254; j++) {
      makeNote(i, j, defaultLength, 0);
    }
  }
  debugNoteCount();
}

//fills sequence with notes
void debugFillTrack(int division) {
  Serial.print("placing note every ");
  Serial.print(division);
  Serial.print(" slices...\n");
  for (int j = 0; j < 254; j += division) {
    makeNote(activeTrack, j, defaultLength, 0);
  }
}
//Sequence parameters -----------------------------------------------------------

void initSeq(int tracks) {

  offNote.startPos = 0;
  offNote.endPos = 0;
  offNote.velocity = 0;

  defaultChannel = 1;
  defaultVec.push_back(offNote);//setting up the default vector with a 0 in it
  trackNumber = tracks; //this way you can intialize sequences with different start tracks
  defaultPitch = 39;

  bpm = 120;
  seqStart = 0;
  seqEnd = 1536;

  loopStart = 0;
  loopEnd = 96;
  isLooping = true;
  loopIterations = 1;
  activeLoop = 0;

  viewStart = 0;
  viewEnd = 96;
  viewLength = 96;
  trackDisplay = screenWidth-viewLength;
  defaultLength = subDivInt; //default note length

  rotaryActionA = false;//set to true when rotary encoders are moved
  rotaryActionB = false;


  cursorPos = viewStart; //cursor position (before playing)
  trackNumber = 4; //number of tracks, starts with four
  activeTrack = 0; //sets which track you're editing, you'll only be able to edit one at a time
  subDivInt = 24;//sets where the divider bars are in the console output
  //cursor jump is locked to this division
  defaultVel = 127;

  vector<unsigned short int> loop1{loopStart,loopEnd};
  loopData.push_back(loop1);
  seqData.resize(trackNumber);
  lookupData.resize(trackNumber);
  noteCount.resize(trackNumber);
  for(int i = trackNumber-1; i > -1; i--){
    lookupData[i].resize(seqEnd+1, blank_ID);
    seqData[i] = defaultVec; //each new track is defaultVec, with 0 notes
    noteCount[i] = blank_ID;
    Track newTrack;
    trackData.push_back(newTrack);
    defaultPitch--;
  }
}

void changeLoopIterations(bool increase){
  if(increase)
    loopIterations++;
  else if(!increase && loopIterations>0)
    loopIterations--;
}

void setLoop(unsigned int id){
  vector<unsigned short int> oldLoop{loopStart,loopEnd,loopIterations};//store current loop
  loopData[activeLoop] = oldLoop;//store current loop
  activeLoop = id;
  loopStart = loopData[activeLoop][0];
  loopEnd = loopData[activeLoop][1];
  loopIterations = loopData[activeLoop][2];
  loopCount = 0;
}
void serialDispLoopData(){
  Serial.print("activeLoop: ");
  Serial.println(activeLoop);
  Serial.print("loopStart:");
  Serial.println(loopStart);
  Serial.print("loopEnd:");
  Serial.println(loopEnd);
  Serial.print("iterations:");
  Serial.println(loopIterations);
  Serial.print("count:");
  Serial.println(loopCount);
}
void addLoop(){
  vector<unsigned short int> newLoop{loopStart,loopEnd,loopIterations};//makes a copy of the current loop
  loopData.push_back(newLoop);
  setLoop(loopData.size()-1);
}
void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter){
  vector<unsigned short int> newLoop{start,end,iter};
  loopData.push_back(newLoop);
}

void deleteLoop(){
  if(loopData.size() > 1 && loopData.size()>activeLoop){//if there's more than one loop, and id is in loopData
    loopData.erase(loopData.begin()+activeLoop);
  }
  if(activeLoop != 0){  
    activeLoop = activeLoop-1;
  }
  setLoop(activeLoop);
}

void toggleLoop(){
  isLooping = !isLooping;
}

void nextLoop(){//moves to the next loop in loopSeq
  if(loopData.size()>1){
    vector<unsigned short int> oldLoop{loopStart,loopEnd,loopIterations};//store current loop
    loopData[activeLoop] = oldLoop;//store current loop
    if(activeLoop>=loopData.size()-1)
      activeLoop = 0;
    else
      activeLoop++;
    loopStart = loopData[activeLoop][0];
    loopEnd = loopData[activeLoop][1];
    loopIterations = loopData[activeLoop][2];
  }
  loopCount = 0;
}

void setBpm(int newBpm) {
  bpm = newBpm;
}

//increases/decreases bpm by 20
void increaseBpm(bool increase) {
  if (increase) {
    bpm += 10;
  }
  else {
    bpm -= 10;
  }
  if (bpm < 1) {
    bpm = 1;
  }
  if (bpm > 1000){
    bpm = 999;
  }
}

//changes which track is active, changing only to valid tracks
void setActiveTrack(int newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < trackNumber) {
    activeTrack = newActiveTrack;
    if (loudly) {
      MIDI.sendNoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      MIDI.sendNoteOff(trackData[activeTrack].pitch, 0, trackData[activeTrack].channel);
    }
  }
}

//changes the resolution/subDivInt
void resChange(bool direction) {
  if(direction)//increase
    subDivInt += subDivInt;
  if(!direction)
    subDivInt /= 2;
}

//checks if the cursor is out of bounds
bool isOutOfBounds(){
  if(cursorPos>seqEnd || cursorPos < seqStart)
    return true;
  else
    return false;
}

//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void moveToNextNote(bool forward){
  if(noteCount[activeTrack]>0){
    unsigned short int id = lookupData[activeTrack][cursorPos];
    if(forward){
      for(int i = cursorPos; i<seqEnd; i++){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          cursorMove(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          break;
        }
      }
    }
    else{
      for(int i = cursorPos; i>seqStart; i--){
        if(lookupData[activeTrack][i] !=id && lookupData[activeTrack][i] != 0){
          cursorMove(seqData[activeTrack][lookupData[activeTrack][i]].startPos-cursorPos);
          break;
        }
      }
    }
  }
}

void stop(){
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].isSending == true){
      MIDI.sendNoteOff(trackData[i].pitch,127,trackData[i].channel);
      trackData[i].isSending = false;
    }
    trackData[i].isSelected = false;
  }
}
//View ------------------------------------------------------------------
//view start is inclusive, starts at 0
//view end is inclusive, 127
//handles making sure the view is correct
void moveView(int val) {
  viewStart += val;
  viewEnd += val;
}

//moving the cursor around
void cursorMove(int moveAmount){
  cursorPos += moveAmount;
  if (cursorPos < seqStart) {
    cursorPos = seqStart;
  }
  if (cursorPos > seqEnd-1) {
    cursorPos = seqEnd-1;
  }
  if (cursorPos < viewStart) {
    moveView(-viewLength);
  }
  if (cursorPos >= viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(viewLength);
  }
}

void changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    seqData[track][id].endPos += val;
    if(seqData[track][id].endPos<seqData[track][id].startPos)
      seqData[track][id].endPos = seqData[track][id].startPos+1;
    if(seqData[track][id].endPos>=seqEnd)
      seqData[track][id].endPos = seqEnd-1;
    for(int i = seqData[track][id].startPos; i<seqData[track][id].endPos+1; i++){
      lookupData[track][i] = id;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------
String getTrackPitch(int id){
  String pitch;
  int midiNote;
  midiNote = trackData[id].pitch;
  int octave = midiNote/12;
  midiNote = midiNote%12;
  switch(midiNote){
    case 0:
      pitch = "C";
      break;
    case 1:
      pitch = "Db";
      break;
    case 2:
      pitch = "D";
      break;
    case 3:
      pitch = "Eb";
      break;
    case 4:
      pitch = "E";
      break;
    case 5:
      pitch = "F";
      break;
    case 6:
      pitch = "Gb";
      break;
    case 7:
      pitch = "G";
      break;
    case 8:
      pitch = "Ab";
      break;
    case 9:
      pitch = "A";
      break;
    case 10:
      pitch = "Bb";
      break;
    case 11:
      pitch = "B";
      break;
  }
  pitch = pitch+octave;
  return pitch;
}
//draws dashed note
void drawNote(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned short int height, unsigned short int shade, bool isSelected){
//every third pixel, draw a pixel
  for(int j = 0; j<height; j++){
    for(int i = x1+1;i+j%shade<x1+len-1; i+=shade){
      display.drawPixel(i+j%shade,y1+j,SSD1306_WHITE);
    }
  }
  display.drawRect(x1+2, y1+1, len-3, height-2, SSD1306_WHITE);
  display.drawRect(x1+1,y1,2*len,height, SSD1306_BLACK);
}

void drawMutedNote(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned char height){
  display.drawLine(x1,y1,x1+len,y1+height,SSD1306_WHITE);
  display.drawLine(x1+len,y1,x1,y1+height,SSD1306_WHITE);
}

bool isInView(int target){
  if(target>=viewStart && target<=viewEnd)
    return true;
  else
    return false;
}

void shadeArea(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned char height, unsigned char shade){
  for(int j = 0; j<height; j++){
    for(int i = x1;i+j%shade<x1+len; i+=shade){
      display.drawPixel(i+j%shade,y1+j,SSD1306_WHITE);
    }
  }
}

//sends data to screen
//move through rows/columns, printing out data
void displaySeq() {
  viewEnd = viewStart + viewLength;
  if(viewEnd>seqEnd){
    viewEnd = seqEnd;
  }
  endTrack = startTrack + trackNumber;
  trackHeight = (screenHeight - debugHeight) / trackNumber;
  stepLength = (screenWidth-trackDisplay) / viewLength;
  if(trackNumber>maxTracksShown){
    endTrack = startTrack + maxTracksShown;
    trackHeight = (screenHeight-debugHeight)/maxTracksShown;
  }
  while(activeTrack>endTrack-1 && trackNumber>maxTracksShown){
    startTrack++;
    endTrack++;
  }
  while(activeTrack<startTrack && trackNumber>maxTracksShown){
    startTrack--;
    endTrack--;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  for (int track = startTrack; track < endTrack; track++) {
    unsigned short int y1 = (track-startTrack) * trackHeight + debugHeight;
    unsigned short int y2 = y1 + trackHeight;
    if(trackData[track].isSelected){
      shadeArea(trackDisplay,y1,screenWidth-trackDisplay, trackHeight,4+3*track%2);
    }
    display.setCursor(2, y1+2);
    display.print(getTrackPitch(track));
    if(trackData[track].isSending){
      display.drawRect(0,y1,trackDisplay,trackHeight,SSD1306_WHITE);
    }
    for (int step = viewStart; step < viewEnd; step++) {
      int id = lookupData[track][step];
      unsigned short int x1 = trackDisplay+int(step * stepLength) % (screenWidth-trackDisplay);
      unsigned short int x2 = x1 + stepLength;
      //measure bars
      if (!(step % subDivInt)) {
        display.drawFastVLine(x1, y1, trackHeight, SSD1306_WHITE);
      }
      //drawing note
      if (id != blank_ID && step == seqData[track][id].startPos){
        Note targetNote = seqData[track][id];
        unsigned short int length = targetNote.endPos - targetNote.startPos+1;
        if(targetNote.velocity>125){
          drawNote(x1,y1,stepLength*length,trackHeight,1,targetNote.isSelected);
        }
        else if(targetNote.velocity>100){
          drawNote(x1,y1,stepLength*length,trackHeight,2,targetNote.isSelected);
        }
        else if(targetNote.velocity>80){
          drawNote(x1,y1,stepLength*length,trackHeight,3,targetNote.isSelected);
        }
        else if(targetNote.velocity>60){
          drawNote(x1,y1,stepLength*length,trackHeight,4,targetNote.isSelected);
        }
        else if(targetNote.velocity>40){
          drawNote(x1,y1,stepLength*length,trackHeight,5,targetNote.isSelected);
        }
        else if(targetNote.velocity>20){
          drawNote(x1,y1,stepLength*length,trackHeight,6,targetNote.isSelected);
        }
        else if(targetNote.velocity>0){
          drawNote(x1,y1,stepLength*length,trackHeight,7,targetNote.isSelected);
        }
        else if(targetNote.velocity == 0){
          drawNote(x1,y1,stepLength*length,trackHeight,10,targetNote.isSelected);
        }
        if(targetNote.isSelected){
          display.drawRoundRect(trackDisplay+x1+1,y1+1,stepLength*length-1,trackHeight-2,4,SSD1306_BLACK);
        }
      }
    }
  }
 //loop points
  if(isInView(loopStart)){
      display.fillTriangle(trackDisplay+stepLength*(loopStart-viewStart), debugHeight-1, trackDisplay+stepLength*(loopStart-viewStart), debugHeight-4, trackDisplay+stepLength*(loopStart-viewStart)+3, debugHeight-4,SSD1306_WHITE);
  }
  if(isInView(loopEnd-1)){
      display.drawTriangle(trackDisplay+stepLength*(loopEnd-viewStart)-1, debugHeight-1, trackDisplay+stepLength*(loopEnd-viewStart)-4, debugHeight-4, trackDisplay+stepLength*(loopEnd-viewStart)-1, debugHeight-4,SSD1306_WHITE);
  }
  //playhead/rechead
  if(playing && isInView(playheadPos))
    display.drawRoundRect(trackDisplay+stepLength*(playheadPos-viewStart),debugHeight,subDivInt*stepLength/2, screenHeight-debugHeight, 3, SSD1306_WHITE);
  if(recording && isInView(recheadPos))
    display.drawRoundRect(trackDisplay+stepLength*(recheadPos-viewStart),debugHeight,subDivInt*stepLength/2, screenHeight-debugHeight, 3, SSD1306_WHITE);
  //velocity info
  display.setCursor(screenWidth-48,0);
  display.print("vel:");
  if(lookupData[activeTrack][cursorPos] == 0)
    display.print(defaultVel);
  else{
    display.print(seqData[activeTrack][getRecentNoteID(activeTrack,cursorPos)].velocity);
  }
  
  //drawing cursor
  display.drawRect(trackDisplay+int(cursorPos * stepLength) % (screenWidth-trackDisplay), debugHeight, stepLength+1, screenHeight-debugHeight, SSD1306_WHITE);
  display.setCursor(0,8);
  display.print(activeLoop);
  display.print("id:");
  display.print(lookupData[activeTrack][cursorPos]);
  display.print(" ");
  display.print(seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos);
  display.print(",");
  display.print(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos);
  //drawing active track highlight
  unsigned short int x1 = trackDisplay;
  unsigned short int y1 = (activeTrack-startTrack) * trackHeight + debugHeight;
  unsigned short int x2 = x1+screenWidth-trackDisplay;
  unsigned short int y2 = y1 + trackHeight;
  display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);

  //drawing debug
  display.setCursor(2, 0);
  display.print("BPM:");
  display.print(bpm);
  if(playing)
    display.print(" P");
  if(listening && !recording)
    display.print(" L");
  if(recording)
    display.print(" R");
//  display.drawRect(0,0,128,16,SSD1306_WHITE);
//  display.drawRect(0,16,128,48,SSD1306_WHITE);
  if(menuIsActive){
    activeMenu.displayMenu();
  }
  display.display();
}

//displays notes in all tracks from viewStart -> viewEnd
void displaySeqSerial() {
  unsigned short int id = lookupData[activeTrack][cursorPos];
  Serial.print("\n");
  for (int track = trackNumber - 1; track > -1; track--) {
    Serial.print(trackData[track].pitch);//prints the pitch before each track
    for (int note = viewStart; note < viewEnd; note++) {
      if (!(note % subDivInt)) { //if note is a multiple of subDivInt, print a divider
        Serial.print("|");
      }
      //if no note
      if (lookupData[track][note] == blank_ID) {
        if (note == cursorPos) { //if the cursor is there
          Serial.print("#");
        }
        else if (track == activeTrack) { //if the track is active
          Serial.print("=");
        }
        else {
          Serial.print(".");//default track icon
        }
      }//if there's a tail_ID
      //if there is a note
      if (lookupData[track][note] != blank_ID) {
        if (note == cursorPos && track == activeTrack) { //if the cursor is on it and the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("{S}");
          }
          else {
            Serial.print("[N]");
          }
        }
        else if (track == activeTrack) { //if the track is active
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("s");
          }
          else {
            Serial.print("n");
          }
        }
        else { //default display of a note
          if (seqData[track][lookupData[track][note]].isSelected) {
            Serial.print("s");
          }
          else {
            Serial.print("n");
          }
        }
      }
    }
    Serial.print("|\n");
  }
}

//Selection------------------------------------------------------------------------------------------
//this just clears the buffer, and resets any notes in it to being unselected
void clearSelection() {
  Serial.print("clearing...\n");
  Serial.print(selectionBuffer.size());
  if (selectionBuffer.size() > 0) {
    for (int i = 0; i < selectionBuffer.size() - 1; i += 2) {
      unsigned char trackID = selectionBuffer[i];
      unsigned short int noteID = selectionBuffer[i + 1];
      seqData[trackID][noteID].isSelected = false;
    }
  }
  selectionBuffer.clear();
}
//this searches thru the buffer and deletes a target note, resets its tag
void clearSelection(int track, int time) {
  if (selectionBuffer.size() > 0) {
    int targetNoteID = lookupData[track][time];
    Note targetNote = seqData[track][targetNoteID];
    for (int i = 0; i < (selectionBuffer.size() - 1); i += 2) { //sifting thru buffer
      if (((int)selectionBuffer[i] == track) && ((int)selectionBuffer[i + 1] == targetNoteID)) { //if the target note is in the buffer
        selectionBuffer.erase((selectionBuffer.begin() + i), selectionBuffer.begin() + 2 + i); //delete from buffer
        targetNote.isSelected = false;
        break;
      }
    }
    seqData[track][targetNoteID] = targetNote;
  }
}

//selecting a note. Additive tells the function if it should add to the current selection, or reset all selected notes.
//selects notes by ID and location
void selectNote(int track, int step, bool additive) {
  if (lookupData[track][step] != blank_ID) {
    unsigned short int targetNoteID = lookupData[track][step];
    bool isSelected = seqData[track][targetNoteID].isSelected;
    //if it's not additive, clear the buffer
    if (!additive && !isSelected) {
      clearSelection();
      selectionBuffer.push_back(track);
      selectionBuffer.push_back(targetNoteID);
      seqData[track][targetNoteID].isSelected = true;
    }
    else{
      //if the note was already selected, and the buffer is larger than 0,clear note from selection buffer
      //idk if you need the larger than zero??
      if (isSelected && selectionBuffer.size() > 0) {
        clearSelection(track, step);
      }
      //if it wasn't already selected
      else if (!isSelected) {
        selectionBuffer.push_back(track);//adding to the selection list
        selectionBuffer.push_back(targetNoteID);
        seqData[track][targetNoteID].isSelected = true;
      }
    }
  }
  Serial.print("selection buffer holds ");
  Serial.print(selectionBuffer.size() / 2);
  Serial.print(" notes.\n");
}

void selectNotesInTrack(){
  for(int i = 0; i<noteCount[activeTrack];i++){
    selectNote(activeTrack, seqData[activeTrack][i + 1].startPos, true);
  }
}

//selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
void selectAll(bool additive, bool inTrack, bool inTime) {
  if (inTrack) {
    for (int i = 0; i < noteCount[activeTrack]; i++) { //starting from the second note, select all notes in the track
      selectNote(activeTrack, seqData[activeTrack][i + 1].startPos, additive);
    }
  }
  else if (inTime) { //this'll be the thing that breaks
    for (int i = 0; i < trackNumber; i++) {
      selectNote(i, seqData[activeTrack][cursorPos].startPos + 1, additive);
    }
  }
  else { //rolling thru all notes in each track
    for (int i = 0; i < trackNumber; i++) {
      for (int j = 0; j < noteCount[i]; j++) {
        selectNote(i, seqData[i][j + 1].startPos + 1, additive);
      }
    }
  }
}

//moving a note at the cursor
//moving a note moves its location in lookupData, AND it's startPos data

//moving a specific note
// void moveNote(unsigned short int noteID, unsigned char trackID, char amount) {
//   int oldLoc = seqData[trackID][noteID].startPos;
//   seqData[trackID][noteID].startPos = oldLoc + amount;//moving it in seqData
//   lookupData[trackID][oldLoc] = blank_ID;//erasing old note
//   lookupData[trackID][oldLoc + amount] = noteID; //putting it in the new place
// }

//moving all the selected notes
// void moveSelected(char amount) {
//   for (int i = 0; i < selectionBuffer.size() - 1; i += 2) {
//     moveNote(lookupData[selectionBuffer[i]][selectionBuffer[i + 1]], selectionBuffer[i], amount); //apply moveNote to each note in the selection buffer
//   }
// }

//adds a little random velocity offset to selected notes
//this is for APPLYING AN OFFSET to the current velocity, which is different than assigning new random velocities
void mildlyRandomizeVel(int maxAmount) {
  Note targetNote;
  for (int i = 0; i < selectionBuffer.size() - 1; i += 2) {
    unsigned char track = selectionBuffer[i];
    unsigned short int ID = selectionBuffer[i + 1];
    targetNote = seqData[track][ID];
    int randomOffset = rand() % maxAmount; //cut the range in half, since half will be for negatives
    if (randomOffset % 2 == 0) { //if it's even (half the time) then make it a negative offset
      randomOffset = -randomOffset;
    }
    if ((int(targetNote.velocity) + randomOffset) < 0) { //catching too much of a negative velocity
      targetNote.velocity = 1;//won't ever silence notes, just sets v to 1
    }
    else {
      targetNote.velocity += randomOffset;
      if (targetNote.velocity > 127) {
        targetNote.velocity = 127;
      }
    }
    seqData[track][ID] = targetNote;
  }
}
//assigns new random velocities, centered on a value
void randomizeVel(int min, int max, int mid) {
  Note targetNote;
  for (int i = 0; i < selectionBuffer.size() - 1; i += 2) {
    unsigned char track = selectionBuffer[i];
    unsigned short int ID = selectionBuffer[i + 1];
    targetNote = seqData[track][ID];
    int newVel = rand() % max + min;

    if (newVel < mid) {
      newVel = -newVel;
    }
    targetNote.velocity = newVel + mid;//sets the velocity to the mid value, with the newVel random offset

    if (targetNote.velocity > 127) {
      targetNote.velocity = 127;
    }
    if (targetNote.velocity <= 0) {
      targetNote.velocity = 0;
    }
    seqData[track][ID] = targetNote;
  }
}

void playStep(int timestep) {
  for (int track = 0; track < trackNumber; track++) {
    if (lookupData[track][timestep] == 0)
      continue;
    else{ //if there's a note there
      unsigned int id = lookupData[track][timestep];
      if (timestep == seqData[track][id].endPos) { //if it's a tail_ID, check and see if it's the last part. If it is, send off, if not, break
        trackData[track].isSending = false;
        MIDI.sendNoteOff(trackData[track].pitch, 127, trackData[track].channel);
        continue;
      }
      else if(timestep == seqData[track][id].startPos){ //if it isn't a tail_ID, or off
        if(seqData[track][id].chance == 100){
          trackData[track].isSending = true;
          MIDI.sendNoteOn(trackData[track].pitch, seqData[track][id].velocity, trackData[track].channel);
          continue;
        }
        else{
          unsigned char odds = random(100);
          if(seqData[track][id].chance > odds){
            trackData[track].isSending = true;
            MIDI.sendNoteOn(trackData[track].pitch, seqData[track][id].velocity, trackData[track].channel);
          }
        }
      }
    }
  }
}

void nudgeNote(unsigned short int amount, bool forward){
  unsigned short int id = lookupData[activeTrack][cursorPos];
  unsigned short int start = seqData[activeTrack][id].startPos;
  unsigned short int end = seqData[activeTrack][id].endPos;
  if(forward){
    if(end+amount<seqEnd){
      moveNote(id, activeTrack, start+amount);
      cursorMove(amount);
    }
  }
  else{
    if(start-amount>=seqStart){
      moveNote(id, activeTrack, start-amount);
      cursorMove(-amount);
    }
  }
}

void moveNote(unsigned short int id, unsigned short int track, unsigned short int newStart){
  unsigned short int length = seqData[track][id].endPos-seqData[track][id].startPos;
  for(int i = seqData[track][id].startPos; i<seqData[track][id].endPos+1; i++){//clearing out old pos from lookupData
    if(lookupData[track][i] == id){
      lookupData[track][i] = 0;
    }
  }
  seqData[track][id].startPos = newStart;
  seqData[track][id].endPos = newStart+length;
  for(int i = seqData[track][id].startPos; i<seqData[track][id].endPos+1; i++){//writing in new lookupData
    lookupData[track].clear();
    if(lookupData[track][i] != 0){
      if(lookupData[track][i] = seqData[track][lookupData[track][i]].startPos)
        deleteNote();
      else
        truncateNote(track,i);
    }
    else
      lookupData[track][i] = id;
  }
}
void updateLookupData_track(unsigned short int track){
    lookupData[track].assign(seqEnd,0);//blank the track
    for(int id = 1; id<noteCount[track]+1; id++){//for each note in data
      unsigned short int start = seqData[track][id].startPos;
      unsigned short int end = seqData[track][id].endPos;
      for(int i = start; i<end+1; i++){
        lookupData[track][i] = id;
      }
    }
}

//idrk why this is broken, but this is super broken (but maybe not???)
void updateLookupData(){
  for(int track = 0; track<trackNumber; track++){//for each track
    updateLookupData_track(track);
  }
  Serial.println("updating...");
}

//quantizes note to closest subdiv
void quantizeNote(unsigned short int track, unsigned short int id){
  unsigned short int oldStart = seqData[track][id].startPos;
  unsigned short int offset = oldStart%subDivInt;

  Serial.print("id:");
  Serial.print(id);
  Serial.print(",");
  Serial.println(offset);

  if(offset > subDivInt/2){//if note is closer to the next subdiv, offset will be greater than half of subdiv
    Serial.print("moving to:");
    Serial.println(oldStart+abs(subDivInt-offset));
    if(lookupData[track][oldStart+subDivInt-offset] == 0 || lookupData[track][oldStart+subDivInt-offset] == id)
      moveNote(id, track, oldStart+abs(subDivInt-offset));
  }
  // && lookupData[track][oldStart-offset] == 0
  else if(offset < subDivInt/2){
    Serial.print("moving to:");
    Serial.println(oldStart-offset);
    if(lookupData[track][oldStart-offset] == 0 || lookupData[track][oldStart-offset] == id)
      moveNote(id, track, oldStart-offset);
  }
  else{//if there's no open spot, delete 'em
    Serial.print("deleting:");
    Serial.print(id);
    Serial.print(",");
    Serial.println(track);
    deleteNote(track, id, false);
  }
}

void quantizeTrack(unsigned short int track){
  for(unsigned short int id = 1; id<noteCount[track]+1; id++){
    quantizeNote(track,id);
  }
}

void quantizeSelection(){
  for(int i = 0; i<selectionBuffer.size(); i+=2){
    quantizeNote(i,i+1);
  }
}

void quantizeSeq(){
  int a = 0;
  for(unsigned short int track = 0; track<trackNumber; track++){
    for(unsigned short int id = 1; id<noteCount[track]+1; id++){
      quantizeNote(track,id);
      a++;
    }
  }
  Serial.println(a);
}

//editing the pitch of a track
void movePitch(unsigned char track, bool up, bool loudly) {
  if (up) {
    trackData[track].pitch += 1;
  }
  else {
    trackData[track].pitch -= 1;
  }
  if(loudly){
    MIDI.sendNoteOn(trackData[track].pitch, defaultVel, trackData[track].channel);
    MIDI.sendNoteOff(trackData[track].pitch, defaultVel, trackData[track].channel);
  }
}

//moves the pitch of all notes up or down by 1
void moveAllPitches(bool up, bool loudly) {
  if (up) {
    for (int i = 0; i < trackNumber; i++) {
      if ((trackData[i].pitch + 1) <= 127) { //if the pitch won't be more than 127
        trackData[i].pitch += 1; //increase it by 12
      }
    }
    if(loudly){
      MIDI.sendNoteOn(trackData[0].pitch, defaultVel, trackData[0].channel);
      MIDI.sendNoteOff(trackData[0].pitch, defaultVel, trackData[0].channel);
    }
  }
  if (!up) {
    for (int i = 0; i < trackNumber; i++) {
      if ((trackData[i].pitch - 1) >= 0) { //if the pitch won't be less than 0
        trackData[i].pitch -= 1; //decrease it by 12
      }
    }
   MIDI.sendNoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
   MIDI.sendNoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
  }
}

void writeNoteOn(unsigned short int pitch, unsigned short int vel){
    Note newNote(recheadPos, recheadPos, vel);//this constuctor sets the endPos of the note at the same position
    unsigned short int track = getTrackWithPitch(pitch);
    noteCount[track]++;
    lookupData[track][recheadPos] = noteCount[track];
    seqData[track].push_back(newNote);
    trackData[track].isSending = true;
}
void writeNoteOff(unsigned short int pitch){
    unsigned short int track = getTrackWithPitch(pitch);
    unsigned short int note = getRecentNoteID(track, recheadPos);
    seqData[track][note].endPos = recheadPos;
    trackData[track].isSending = false;
    for(int i = seqData[track][note].startPos; i<=seqData[track][note].endPos; i++){
      lookupData[track][i] = lookupData[track][seqData[track][note].startPos];
    }
}

volatile bool noteOnReceived = false;
volatile bool noteOffReceived = false;
volatile uint8_t lastNoteOn [3];
volatile uint8_t lastNoteOff [2];
uint8_t lastPitch,lastChannel,lastVel;

void handleClock_recording(){
    gotClock = true;
    recheadPos++;
}
void handleStart_recording(){
  startRec = true;
}
void handleStop_recording(){
  startRec = false;
}

void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOn(note, velocity);
}
void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity){
  writeNoteOff(note);
}
void handleNoteOn_Recording_LMAOFUUUCK(uint8_t channel, uint8_t note, uint8_t velocity){
    noteOnReceived = true;
    lastNoteOn[0] = channel;
    lastNoteOn[1] = note;
    lastNoteOn[2] = velocity;
    // lookupData[getTrackWithPitch(note)][recheadPos] = note;
}
void handleNoteOff_Recording_fuck(uint8_t channel, uint8_t note, uint8_t velocity){
    noteOffReceived = true;
    lastNoteOff[0] = channel;
    lastNoteOff[1] = note;
    // lookupData[getTrackWithPitch(note)][recheadPos] = 255;
}

void handleNoteOn_Listening(uint8_t channel, uint8_t note, uint8_t velocity){
    noteOnReceived = true;
    lastPitch = note;
    lastChannel = channel;
    lastVel = velocity;
}
void handleNoteOff_Listening(uint8_t channel, uint8_t note, uint8_t velocity){
    trackData[getTrackWithPitch(note)].isSending = false;
}

void readNotes_Listening(uint8_t channel, uint8_t note, uint8_t velocity){
    int track = getTrackWithPitch(note);
    if(track !=-1 ){
        trackData[track].isSending = true;
    }
    else{
        addTrack(note, false);
        trackData[getTrackWithPitch(note)].isSending = true;
        activeTrack = getTrackWithPitch(note);
    }
    noteOnReceived = false;
}

void cleanupRecording(int stopTime){
  for(int i = 0; i<trackNumber; i++){
    if(trackData[i].isSending){
      seqData[i][getRecentNoteID(i,stopTime)].endPos = stopTime;
      trackData[i].isSending = false;
    }
  }
}

void debugPrintRec(){
  for(int i = 0; i<trackNumber;i++){
    Serial.print('|');
    for(int j = loopStart; j<= loopEnd; j++){
      if(!j%subDivInt)
        Serial.print('|');
      if(lookupData[i][j]==0)
        Serial.print('-');
      else
        Serial.print(lookupData[i][j]);
    }
    Serial.println('|');
  }
}
void toggleListeningMode(){
  listening = !listening;
  if(listening){
      stop();
      MIDI.disconnectCallbackFromType(midi::NoteOn);
      MIDI.disconnectCallbackFromType(midi::NoteOff);
      MIDI.setHandleNoteOn(handleNoteOn_Listening);
      MIDI.setHandleNoteOff(handleNoteOff_Listening);
  }
  if(!listening){
      stop();
      MIDI.disconnectCallbackFromType(midi::NoteOn);
      MIDI.disconnectCallbackFromType(midi::NoteOff);
  }
}

void toggleRecordingMode(){
  recording = !recording;
  if(recording){
      stop();
      MIDI.disconnectCallbackFromType(midi::NoteOn);
      MIDI.disconnectCallbackFromType(midi::NoteOff);
      MIDI.setHandleNoteOn(handleNoteOn_Recording);
      MIDI.setHandleNoteOff(handleNoteOff_Recording);
      MIDI.setHandleClock(handleClock_recording);
      MIDI.setHandleStart(handleStart_recording);
      MIDI.setHandleStop(handleStop_recording);
  }
  if(!recording){//go back to normal listening mode
      MIDI.disconnectCallbackFromType(midi::NoteOn);
      MIDI.disconnectCallbackFromType(midi::NoteOff);
      MIDI.disconnectCallbackFromType(midi::Start);
      MIDI.disconnectCallbackFromType(midi::Stop);
      MIDI.setHandleNoteOn(handleNoteOn_Listening);
      MIDI.setHandleNoteOff(handleNoteOff_Listening);
      cleanupRecording(recheadPos);
  }
}

unsigned short int getLowestPitch(){
  unsigned short int pitch = trackData[activeTrack].pitch;
  for(int i = 0; i<trackNumber; i++){
    if(trackData[i].pitch<pitch)
      pitch = trackData[i].pitch;
  }
  return pitch;
}


//this is the big switch statement that listens for key inputs and runs the according functions
void keyListen() {
  unsigned char key = 0;
  String pitch;
  int track;
  key = Serial.read();
  switch (key) {
    case 'c':
      serialPrintCurve(debugCurve);
      break;
    case 'q':
      quantizeNote(activeTrack, lookupData[activeTrack][cursorPos]);
      displaySeqSerial();
      break;
    case 'Q':
      quantizeSeq();
      displaySeqSerial();
      break;
    case '`':
      updateLookupData();
      displaySeqSerial();
      break;
    case 'u':
      track = getTrackWithPitch(24);
      Serial.println(track);
      break;
    case '.'://>
      cursorMove(subDivInt);
      displaySeqSerial();
      break;
    case 'f':
      debugPrintRec();
      break;
    case ','://<
      cursorMove(-subDivInt);
      displaySeqSerial();
      break;
    case '>'://precise movement
      cursorMove(1);
       displaySeqSerial();
      break;
    case '<':
      cursorMove(-1);
        displaySeqSerial();
      break;
    case 'n':
      Serial.print("writing a note...\n");
      makeNote(activeTrack, cursorPos, defaultLength, 1); //default makes an 8th note
        displaySeqSerial();
      break;
    case ']':
      setActiveTrack(activeTrack - 1, 1);
        displaySeqSerial();
      break;
    case '[':
      setActiveTrack(activeTrack + 1, 1);
        displaySeqSerial();
      break;
    case 's'://select a note
      selectNote(activeTrack, cursorPos, true);
        displaySeqSerial();
      break;
    case 'S'://select, non-additive
      selectNote(activeTrack, cursorPos, false);
        displaySeqSerial();
      break;
    case 'p'://debug print selected notes
      debugPrint();
        displaySeqSerial();
      break;
    case 't': //add track (tricky! big moment if this works first time) LMAO
      addTrack(defaultPitch);
      defaultPitch++;
        displaySeqSerial();
      break;
    case 'D'://deleting the selected track
      deleteTrack();
        displaySeqSerial();
      break;
    case 'd'://deleting the selected note(s)
      Serial.print("attempting to delete ");
      if (selectionBuffer.size() > 0) {
        Serial.print("all selected notes...\n");
        deleteSelectedNotes();
      }
      else {
        Serial.print("1 note...\n");
        deleteNote();
      }
      displaySeqSerial();
      break;
    case 'e'://erase a track
      eraseTrack();
        displaySeqSerial();
      break;
    case 'E'://erases the sequence
      eraseSeq();
        displaySeqSerial();
      break;
    case 'm':
      menuIsActive = !menuIsActive;
      break;
    case '1':
      activeMenu = debugMenu;
      break;
    case '2'://counts notes
      activeMenu = arpMenu;
      break;
    case '3'://counts notes
      activeMenu = echoMenu;
      break;
    case 'F'://fills sequence
      debugFillSeq();
        displaySeqSerial();
      break;
    case 'a'://selects all in track
      printArp(activeArp);
      break;
    case 'A'://selects all
      commitArp(activeArp);
      displaySeqSerial();
      break;
    case 'w'://selects by timeslice
      selectAll(1, 0, 1);
        displaySeqSerial();
      break;
    case ';':
      loopEnd -= 16;
        displaySeqSerial();
      break;
    case '\'':
      loopEnd += 16;
    case 'r':
      toggleRecordingMode();
      break;
    case 'R':
      randomizeVel(0, 127, 63);
        displaySeqSerial();
      break;
    case 'P':
      playing = !playing;
      break;
    case '_':
        displaySeqSerial();
      break;
    case '+':
        displaySeqSerial();
      break;
    case '=':
        displaySeqSerial();
      break;
    case '-':
        displaySeqSerial();
      break;
    case 'B':
      increaseBpm(1);
      break;
    case 'l':
      notLooping = !notLooping;
      break;
    case 'L':
      toggleListeningMode();
      Serial.println("Listening!");
      break;
  }
}

short int x,y;
bool joy_Press,n,sel=false,shift,del,play,track_Press,note_Press, track_clk, note_clk,loop_Press;

int encoderA = 0,encoderB = 0;
void rotaryActionA_Handler(){
  rotaryActionA = true;
}
void rotaryActionB_Handler(){
  rotaryActionB = true;
}
int readEncoder(bool encoder){
    //Reset the flag that brought us here (from ISR)
    
    int8_t r;
    int8_t l;
    
    static uint8_t lrmem = 3;
    static int lrsum = 0;
    static int8_t TRANS[] = {0, -1, 1, 14, 1, 0, 14, -1, -1, 14, 0, 1, 14, 1, -1, 0};

    int clk_Pin;
    int data_Pin;

    // Read BOTH pin states to deterimine validity of rotation (ie not just switch bounce)
    if(!encoder){
      clk_Pin = track_clk_Pin;
      data_Pin = track_data_Pin;
    }
    else{
      clk_Pin = note_clk_Pin;
      data_Pin = note_data_Pin;
    }
    
    l = digitalRead(clk_Pin);
    r = digitalRead(data_Pin);
  
    // Move previous value 2 bits to the left and add in our new values
    lrmem = ((lrmem & 0x03) << 2) + 2 * l + r;

    // Convert the bit pattern to a movement indicator (14 = impossible, ie switch bounce)
    lrsum += TRANS[lrmem];

    /* encoder not in the neutral (detent) state */
    if (lrsum % 4 != 0)
    {
        return 0;
    }

    /* encoder in the neutral state - clockwise rotation*/
    if (lrsum == 4)
    {
        lrsum = 0;
        return -1;
    }

    /* encoder in the neutral state - anti-clockwise rotation*/
    if (lrsum == -4)
    {
        lrsum = 0;
        return 1;
    }

    // An impossible rotation has been detected - ignore the movement
    lrsum = 0;
    return 0;
}
void joyCommands(){
  if(!menuIsActive){
    if ((millis() - lastTime) > 100) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          cursorMove(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          cursorMove(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          cursorMove(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          cursorMove(subDivInt);
          lastTime = millis();
        }
      }
      if (y == 1 && !shift) {
        if(listening||recording||playing)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, true);
        lastTime = millis();
      }
      if (y == -1 && !shift) {
        if(listening||recording||playing)//if you're not in normal mode, you don't want it to be loud
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
        lastTime = millis();
      }
    }
    if ((millis() - lastTime) > 50) {
      if (x == 1 && shift) {
        cursorMove(-1);
        lastTime = millis();
      }
      if (x == -1 && shift) {
        cursorMove(1);
        lastTime = millis();
      }
      if (y == 1 && shift) {
        changeVel(-10);
        lastTime = millis();
      }
      if (y == -1 && shift) {
        changeVel(10);
        lastTime = millis();
      }
      if(lookupData[activeTrack][cursorPos]==0){
        if(y == 1 && shift){
          defaultVel-=10;
          if(defaultVel<1)
            defaultVel = 1;
          lastTime = millis();
        }
        if(y == -1 && shift){
          defaultVel+=10;
          if(defaultVel>127)
            defaultVel = 127;
          lastTime = millis();
        }
      }
    }
    if ((millis() - lastTime) > 50 && loop_Press) {
      if (y == 1 && shift) {
        nextLoop();
        lastTime = millis();
        serialDispLoopData();
      }
      if (y == -1 && shift) {
        nextLoop();
        lastTime = millis();
        serialDispLoopData();
      }
      if(y == 1 && shift){
        changeLoopIterations(true);
        serialDispLoopData();
      }
      if(y == -1 && shift){
        changeLoopIterations(false);
        serialDispLoopData();
      }
    }
  }
  if(menuIsActive && (millis() - lastTime)>50){
    if(activeMenu.style == 1){//horizontal menus
        if(x == 1){
          activeMenu.moveMenuCursor(true);
          lastTime = millis();
        }
        if(x == -1){
          activeMenu.moveMenuCursor(false);
          lastTime = millis();
        }
    }
    if(activeMenu.style == 0){//horizontal menus
        if(y == 1){
          activeMenu.moveMenuCursor(true);
          lastTime = millis();
        }
        if(y == -1){
          activeMenu.moveMenuCursor(false);
          lastTime = millis();
        }
    }
  }
}
void joyRead(){
  int xVal = analogRead(x_Pin);
  int yVal = analogRead(y_Pin);
   x = 0;
   y = 0;
  if (xVal > 800) {
    x = 1;
  }
  else if(xVal < 200) {
    x = -1;
  }
  if (yVal > 800) {
    y = 1;
  }
  else if(yVal < 200) {
    y = -1;
  }
}
void buttons(){
  if(millis()-lastTime > 200){
    //new
    if(n){
      if(lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos){
        makeNote(activeTrack,cursorPos,subDivInt,true);
        lastTime = millis();
      }
    }
    //select
    if(sel && lookupData[activeTrack][cursorPos] != 0){
      unsigned short int id;
      id = lookupData[activeTrack][cursorPos];
      if(shift){
        clearSelection();
        selectNote(activeTrack, seqData[activeTrack][id].startPos, false);
      }
      else{
        selectNote(activeTrack, seqData[activeTrack][id].startPos, true);          
      }
      lastTime = millis();
    }
    if(sel && lookupData[activeTrack][cursorPos] == 0){//might need to change this to do selection box
      clearSelection();
    }
    //delete
    if(del && !shift){
      if (selectionBuffer.size() > 0) {
        deleteSelectedNotes();
        lastTime = millis();
      }
      else {
        deleteNote();
        lastTime = millis();
      }
    }
    if(del && shift){
      eraseTrack(activeTrack);
      lastTime = millis();
    }
    if(play && !shift && !listening && !recording){//play mode
      playing = !playing;
      if(!playing){
        stop();
      }
      lastTime = millis();
    }
    //play
    if(play && shift){//listening mode
      playing = false;
      toggleListeningMode();
      if(recording){
        toggleRecordingMode();
      }
      lastTime = millis();
    }
    if(play && listening && !shift){
      toggleRecordingMode();
      lastTime = millis();
    }
    if(play && n){//save midi file to flash
    }
    if(play && del){
      eraseSeq();
      lastTime=millis();
    }
    //loop
    if(loop_Press && !shift){
      if(cursorPos>loopEnd){
        loopEnd = cursorPos - 1;
        lastTime = millis();
      }
      else if(cursorPos<loopStart){
        loopStart = cursorPos;
        lastTime = millis();
      }
      else if(shift){
        loopEnd = cursorPos - 1;
        lastTime = millis();
      }
      else{
        loopStart = cursorPos;
        lastTime = millis();
      }
    }
    if(loop_Press && n){
      addLoop();
      serialDispLoopData();
    }
    if(loop_Press && del){
      deleteLoop();
      serialDispLoopData();
    }
  }
  //track press
  if(millis()-lastTime > 200){
    if(track_Press && sel){  
        selectNotesInTrack();
        selectTrack();
        lastTime = millis();
      }
    if(track_Press && n){
      addTrack(trackData[trackNumber-1].pitch-1);
      lastTime = millis();
    }
    if(track_Press && del){
      deleteTrack();
      lastTime = millis();
    }
  }
  if(track_Press && encoderA == 0 && millis()-lastTime>200 && !sel){
    if(cursorPos>loopEnd){
      loopEnd = cursorPos - 1;
      lastTime = millis();
    }
    else if(cursorPos<loopStart){
      loopStart = cursorPos;
      lastTime = millis();
    }
    else if(shift){
      loopEnd = cursorPos - 1;
      lastTime = millis();
    }
    else{
      loopStart = cursorPos;
      lastTime = millis();
    }
  }
}
void readButtons(){
  joy_Press = !digitalRead(joy_press_Pin);
  loop_Press = !digitalRead(loop_Pin);//CHANGE THIS WHEN U GET MORE BUTTONS
  n = !digitalRead(new_Pin);
  // sel = !digitalRead(select_Pin);
  shift = !digitalRead(shift_Pin);
  del = !digitalRead(delete_Pin);
  play = !digitalRead(play_Pin);
  track_Press = !digitalRead(track_press_Pin);
  note_Press = !digitalRead(note_press_Pin);
}
void encoders(){
  if(encoderA != 0){
     if(encoderA == 1 && !shift && !track_Press){
        if(listening||recording)
         movePitch(activeTrack,1,false);
        else
          movePitch(activeTrack,1,true);
     }
     if(encoderA == -1 && !shift && !track_Press){
        if(listening||recording)
         movePitch(activeTrack,0,false);
        else
          movePitch(activeTrack,0,true);
     }
     if(encoderA == 1 && shift && !track_Press){
        if(listening||recording)
         moveAllPitches(1,false);
        else
          moveAllPitches(1,true);
      }
     if(encoderA == -1 && shift && !track_Press){
        if(listening||recording)
         moveAllPitches(0,false);
        else
          moveAllPitches(0,true);
     }
     if(encoderA == 1 && track_Press){
        increaseBpm(1);
     }
     if(encoderA == -1 && track_Press){
        increaseBpm(0);
     }
     encoderA = 0;
  }
  if(encoderB != 0){
    if(encoderB == 1 && !shift){
      moveToNextNote(1);
    }
    if(encoderB == -1 && !shift){
      moveToNextNote(0);
    }
    if(encoderB == 1 && note_Press){
      subDivInt /= 2;
      if(subDivInt < 3){
        subDivInt = 3;
      }
    }
    if(encoderB == -1 && note_Press){
      subDivInt *= 2;
      if(subDivInt>192){
        subDivInt = 192;
      }
    }
    if(encoderB == 1 && shift){
      changeNoteLength(1,activeTrack, lookupData[activeTrack][cursorPos]);
    }
    if(encoderB == -1 && shift){
      changeNoteLength(-1,activeTrack, lookupData[activeTrack][cursorPos]);
    }
    encoderB = 0;
  }
}
void encoders_arp(){
}
void inputRead() {
  joyRead();
  readButtons();
    if(encoderA != 0 || encoderB != 0)
      encoders();
    if(x != 0 || y != 0)
      joyCommands();
    if(n||joy_Press||sel||shift||del||play||track_Press||note_Press||loop_Press)
      buttons();
}

bool core0ready = false;

void setup() {
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  TinyUSB_Device_Init(0);
#endif

  MIDI.begin(MIDI_CHANNEL_OMNI);

  //might not need both of these
  while (!TinyUSBDevice.mounted()) {
    delay(1);
  }

  //starting serial monitor output
  Serial.begin(9600);

  //needed to turn on display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  if(helloChild){
    display.display();
    display.clearDisplay();
    display.setCursor(32, 0);
    display.setTextSize(3.5);
    display.setTextColor(SSD1306_WHITE);
    display.print("hello");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setCursor(10, 32);
    display.print("child.");
    display.invertDisplay(true);
    display.display();
    delay(2000);
    display.invertDisplay(false);
  }
//  display.clearDisplay();
//  display.drawBitmap(0,16,eyes_bmp,128,48,SSD1306_WHITE);
//  display.display();
//  delay(5000);

  pinMode(x_Pin, INPUT);
  pinMode(y_Pin, INPUT);
  pinMode(joy_press_Pin, INPUT_PULLUP);
  pinMode(loop_Pin, INPUT_PULLUP);
  pinMode(new_Pin, INPUT_PULLUP);
  pinMode(select_Pin, INPUT_PULLUP);
  pinMode(shift_Pin, INPUT_PULLUP);
  pinMode(play_Pin, INPUT_PULLUP);
  pinMode(delete_Pin, INPUT_PULLUP);
  pinMode(track_press_Pin, INPUT_PULLUP);
  pinMode(note_press_Pin, INPUT_PULLUP);
  pinMode(track_clk_Pin, INPUT_PULLUP);
  pinMode(note_clk_Pin, INPUT_PULLUP);
  pinMode(track_data_Pin, INPUT_PULLUP);
  pinMode(note_data_Pin, INPUT_PULLUP);

  attachInterrupt(track_clk_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(track_data_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(note_clk_Pin,rotaryActionB_Handler, CHANGE);
  attachInterrupt(note_data_Pin,rotaryActionB_Handler, CHANGE);

  srand(1);

  createArp();
  initSeq(trackNumber);
  lastTime = millis();
  core0ready = true;

  activeMenu = debugMenu;
}

void loop() {
  //set the active timestep to the beginning of the loop
  inputRead();
  keyListen();
  displaySeq();
}

float delayVal = 0;
float MSperTimeStep;
float t1;
float t2;
float t3;
float stepTimer;

void setup1() {
  while(!core0ready){
    Serial.println("waiting for core 0...");
  }
}

void writeStep(unsigned short int timestep){
  if(noteOnReceived){
    noteOnReceived = false;
    Note newNote(timestep, timestep, lastNoteOn[2]);//this constuctor sets the endPos of the note at the same position
    unsigned short int track = getTrackWithPitch(lastNoteOn[1]);
    noteCount[track]++;
    lookupData[track][timestep] = noteCount[track];
    seqData[track].push_back(newNote);
    trackData[track].isSending = true;
  }
  if(noteOffReceived){
    noteOffReceived = false;
    unsigned short int track = getTrackWithPitch(lastNoteOff[1]);
    unsigned short int note = getRecentNoteID(track, timestep);
    Serial.println(note);
    seqData[track][note].endPos = timestep;
    trackData[track].isSending = false;
  }
  for(int i = 0; i<trackNumber; i++){
    if(trackData[i].isSending){//if track is sending/receiving, write ID
      lookupData[i][timestep] = noteCount[i];
    }
  }
}

//cuts notes off when loop repeats
void cutLoop(unsigned short int timestep){
  for(int i = 0; i<trackNumber; i++){
    if(trackData[i].isSending){
      trackData[i].isSending = false;
      seqData[i][noteCount[i]].endPos = timestep;
    }
  }
}

//this cpu handles time-sensitive things
void loop1(){
  t1 = micros();
  MSperTimeStep = 60000 / (bpm * 24);
  if(!playing && !recording){
      playheadPos = loopStart;
      recheadPos = loopStart;
  }
  //play mode
  if(!recording && playing && (micros()-stepTimer+delayVal >= 1000*MSperTimeStep)) {
      if (playheadPos >= loopEnd && isLooping) { //if the timestep is past the end of the loop, loop it to the start
        loopCount++;
        if(loopIterations == loopCount){
          nextLoop();
        }
        playheadPos = loopStart;
      }
      stepTimer = micros();
      playStep(playheadPos);
      playheadPos += 1;
  }
  //listening mode
  if(listening && !recording){
    MIDI.read();
    if(noteOnReceived)
        readNotes_Listening(lastChannel, lastPitch, lastVel);
  }

  //record mode
  if(recording){
    if(recheadPos>=loopEnd){
      if(notLooping){
        toggleRecordingMode();
      }
      else{
        recheadPos = loopStart;
        cutLoop(recheadPos);
      }
    }
    MIDI.read();
  }
  //arp
  // if(isArping){
  //   for(int i = 0; i<trackNumber; i++){
  //     if(trackData[i].isSending)
  //       if(find(activeArp.scale.begin(), activeArp.scale.end(), trackData[i].pitch)==activeArp.scale.end()){
  //         activeArp.scale.push_back(trackData[i].pitch);
  //       }
  //   }
  // }
  //encoders
  if(rotaryActionA){
      encoderA = readEncoder(0);
      rotaryActionA = false;
  }
  if(rotaryActionB){
      encoderB = readEncoder(1);
      rotaryActionB = false;
  }
  t3 = micros();
  delayVal = (t3-t1);
}


/*step seq. should have 64 steps per measure, and 4 measures available.
  //If you need more than 64 steps, you can just increase bpm and decrease how many measures you have
  //triplets:
  triplets are tricky, I think the best way is just to approximate it with 1/4 notes being spaced ~20 16th notes apart
  ideally, notes would be 20.33 notes apart, but for this it should be 20, 21, 20
  //live edit
  i need to be able to edit it while playing, and hear the additions/changes AS THEY'RE TWEAKED

  hardware ideas:
  raspberry pi 0 would be perfect
  raspberry pi pico also looks very perfect:
  - 246KB of SRAM, 2MB of flash
  -26 GPIO pins
  Thoughts on mem:
  ---------------------------------
  if you're using 'duino mega, you have 8kB of SRAM to store notes in
  8KB = 8,000 bytes
  int = 4Bytes
  Note object = 5ints = 20B, but could be a lot less
  256 Note Array <= 256 x 20B = 4096 Bytes
  minimization:
    velocity can be a char, signed or unsiged
    startPos and
  248kB for program, 4KB of EEPROM

  bytes available rn (2.17.22):
  250864
  system space taken up by notes is always (noteData)notes+(trackData)tracks
  (7B)notes+(3B+255B)tracks = 250864
  7x+258y = 250864
  if y = notes/254 (since you need a track every 254 notes)


  new efficient notes:
  4 chars + 2 bools = 6B
  4KB/6B = 666 notes, hard max
  256 x 5 =  1280B
  if you saved another byte by combining bools (would make code a lil sloppier):
  4KB/5B = 800 notes
  lookup:
  256 * 1byte * # of rows
  10 rows (maybe a max) = 2.56kB

  1 row full of notes: 256 * 20B + 256 = 4,352kB

  to buffer a 128*64 screen, you'll need ~ 1KB of ram

  efficient/dynamic seqData:
    seqData should just be a list of notes for each track, but not for each timeslice. NoteID = index in dataSeq of a note
    lookupData is a matrix, where each non-zero value is the index of a note in seqData
    so seqData[track][lookupData[track][targetNote]] == the data for targetNote
    SO lookupData needs to update its note IDs when a note is deleted, since the indices for ALL NOTES in seqData change
    would be super slick if it could just be a char, since it wouldn't add storage
    lookupData needs to store 'OFF' notes too, but doesn't need to have them displayed (they can be checked individually)
    lookupData should be a vector of arrays[256] of tuples
    OR
    it could just be a vector of arrays of 256 unsigned chars. 0 would be for no note, the downside being you couldn't theoretically have a note on every timestep
    0 refers to the OFF note
    with ID's, you can have multiple lookupData notes that reference one note

  thoughts on tempo/time
  ------------------------------------------------------------------------------
  256notes => 4 measures @ 64slices/measure => 4 measures @ 16 slices/beat (smallest note = 64th)
         => 8 measures @ 32slices/measure => 8 measures @ 8 slices/beat (smallest note = 32nd) <----- this is probably the most useful
         => 16 measures @ 16slices/measure => 16 measures @ 4 slices/beat (smallest note = 16th)
  midi timing resolution is 24ticks/4th note
  this is 16 beats/4th, right now (but can be higher, with fewer measures)
  ratio: 24/16 = 3/2. So for every 3 midi clock events, sequencer moves forward 2 slices.
    - test and see if this sounds wack, it's a 128th note delay
    - it doesn't sound great, you can kind of tell
  An idea: depending on how excessive/not enough the resolution feels when playing/recording/editing
         notes could be by 48 instead of 64. This would sync better with midi, and would save space


  midi:
  21 = A0, goes to 127 = G9
  OFF thots:
  implement off notes:
  this is actually a tough problem. When making a note, offNote should be placed in the lookupData matrix
  and in seqData, BUT it shouldn't overwrite or be placed past the next onNote
  if a note is made 'inside' another on-Off pair? It should truncate the first note
  how do you tell if you're inside a pair?
  we need a lookupData val to indicate that a note is 'there', maybe just 255?
  when note is placed, and the tick contains 255, then find the next note off and place it at the new notes position-1
  offNotes need to get deleted with on notes, so delete note should get the next off note
  you should just have a getNextOff() function

  TO DO:
  small:
  edit note vel, note length (tricky)
  view window should be able to be less than 64
  soloing (should be a separate list of solo'd tracks, like a note being selected)
  addRndm/humanize function to note locations (nudges them around)
  addRndm/humanize function to velocity (adds some variation) //should this be an offset, or set a midpoint that then it gets offset from?
  switch the cursor from snapping to a new windo when it reaches the end or DRAGGING the window with it
  add measure/remove measure
  move note(doesn't work right now)
  loopA/B/C capability
  latency when playing


  BUGS:
  moveNote is very weird
  deleteAll crashes
  non-additive selection (shift+enter) is bugged


  BAD CRASHES:
  quantize



  big:
  upgrade lookupData and noteStart/end to use short ints, so you can store more than 255 notes
  - OOOhhh could use negative values to store the offnotes of notes, and zero would be no note
  - playTimestep would just send an on or off depending on the sign of the value. To draw tail_ID values,
  though, you still need something in lookUpData, so maybe just like 32,000 or something
  - only do this if it'll be more efficient, the way you do it now isn't bad

  transformation curves --> set a map of 8 points that can act on a group of notes
                          points could be linearly connected, and curve objects could be big piecewise functions
  record mode --> think about system priority, listening for new notes should be highest. Does
                record mode need to like contain all the other functions, so the FIRST thing
                the program does is check if a note is being sent

  following an external midi clock
  chord mode
  - idk like 8 tracks should be created, so you can have 8 mode polyphony
  tracks should be initialized to the last note that was externally sent into the sequencer
  drumsynth


  switching to base 24:

  view length needs to be 96, not 64
  seqLength should be 4 measures of 96 steps, 384
*/
