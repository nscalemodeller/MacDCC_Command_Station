
#include "Arduino.h"
#include "DCCLibrary.h"


extern void (*GetNextMessage)(void);
bool ValidCheckSum(void);
void DisableErrorTimer(void);
void EnableErrorTimer(void);
void NextMessage(void);
void PrintArray();
void PrintIndexes();

#define DCC_PIN    4    // Arduino pin for DCC out. This pin is connected to "DIRECTION" of LMD18200
#define DCC_PWM    5    // must be HIGH for signal out. Connected to "PWM in" of LMD18200
#define DCC_THERM  7    // thermal warning PIN

#define MINCOMMANDLENGTH 3
#define MAXCOMMANDLENGTH 7
#define ERRORTIMEOUT 156   // ~10ms   (16,000,000 / 1024 / 100 )
enum{NOERROR,BADLENGTH,TOMANYCHARS,INVALIDCOMMAND,VALIDCOMMAND};


Message msg[3] = { 
    { { 0xFF, 0, 0xFF, 0, 0, 0, 0}, 3},
    { { 0xFF, 0, 0xFF, 0, 0, 0, 0}, 3},
    { { 0, 0, 0, 0, 0, 0, 0}, 0}
  };      

int currentIndex = 0;
int previousIndex = 1;
int lastIndex = 0;
int newIndex = 2;

int charsReceived = 0;
int result = NOERROR;
bool acknowledge = false;



void NextMessage(void){
    int temp;
    
    if(result == VALIDCOMMAND && lastIndex == currentIndex){
         lastIndex = newIndex;
         newIndex = previousIndex;
         previousIndex = lastIndex;
         acknowledge = true;
    }
    
    temp = currentIndex;
    currentIndex = previousIndex;
    previousIndex = temp;
}


void setup(void) {
       
    GetNextMessage = &NextMessage;  // assign a proper function to GetNextMessage that actually does something
    
    Serial.begin(38400);
    
    DisableErrorTimer();
    
    //Set the pins for DCC to "output".
    pinMode(DCC_PIN,OUTPUT);        // this is for the DCC Signal
    
    pinMode(DCC_PWM,OUTPUT);        // will be kept high, PWM pin
    digitalWrite(DCC_PWM,1);

    SetupDCC(DCC_PIN);   
 }


void loop(void) {
      
    if (acknowledge){
        charsReceived = 0;
        msg[newIndex].len = 0;
        acknowledge = false;
        
        Serial.println(result);
        result = NOERROR;    
    }
}


void serialEvent() {
    
    while(Serial.available()){
        byte inChar = Serial.read();
        
        if(result)
            continue;
        
        if(0 == msg[newIndex].len){
            // first byte of command contains number of bytes in command. Make sure it is within specifications
            if (inChar < MINCOMMANDLENGTH || inChar > MAXCOMMANDLENGTH) {
                result = BADLENGTH;
            }else
                msg[newIndex].len = inChar;
        }else if(charsReceived == msg[newIndex].len){
            result = TOMANYCHARS;
        }else
            msg[newIndex].data[charsReceived++] = inChar;
    }
    
    if(result == NOERROR && charsReceived == msg[newIndex].len){
        result = ValidCheckSum()? VALIDCOMMAND : INVALIDCOMMAND;
    }
    
    if(result > NOERROR && result < VALIDCOMMAND){
        EnableErrorTimer();
    }
}


bool ValidCheckSum(){
    int checksum = 0;
    int i;

    for(i = 0; i < msg[newIndex].len - 1; ++i)
        checksum = (checksum ^ msg[newIndex].data[i]);
    
    return (checksum == msg[newIndex].data[msg[newIndex].len - 1]);
}



#pragma mark Error Timeout

void EnableErrorTimer(){
    
    OCR1A = ERRORTIMEOUT;               // time before timer fires
    
    TCCR1B = 0;                         // stop timer
    TCCR1B = bit(WGM12);                // CTC on OCR1A,
    TCCR1B |= bit(CS12) | bit(CS10);    // prescaler = 1024
    
    TIMSK1 = bit (OCIE1A);              // interrupt on Compare A Match
}


void DisableErrorTimer(){
    
    TCCR1A = 0;     // normal mode
    TCCR1B = 0;     // stop timer
    TIMSK1 = 0;     // cancel timer interrupt
}


ISR(TIMER1_COMPA_vect){
    DisableErrorTimer();
    acknowledge = true;
}



#pragma mark For Testing

void PrintArray(){
    int i;
    
    Serial.println();
    Serial.print("Array: ");

    for(i = 0; i < msg[newIndex].len; ++i){
        Serial.print(msg[newIndex].data[i]);
        Serial.print(" : ");
    }
    Serial.println();
}

void PrintIndexes(){
    Serial.print("Curr :");
    Serial.println(currentIndex);
    Serial.print("Prev :");
    Serial.println(previousIndex);
  //  Serial.print("Last :");
  //  Serial.println(lastIndex);
  //  Serial.print("New :");
  //  Serial.println(newIndex);
}


