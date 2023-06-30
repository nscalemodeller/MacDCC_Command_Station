
// Just a very simple test to see if the LMD18200 is working.
// Basically the blink sketch with extra ports being switched.


#define DCC_PIN    4    // Arduino pin for DCC out. This pin is connected to "DIRECTION" of LMD18200
#define DCC_PWM    5    // must be HIGH for signal out. Connected to "PWM in" of LMD18200
int led = 13;

void setup() {
  // put your setup code here, to run once:
  
    //Set the pins for DCC to "output".
    pinMode(DCC_PIN,OUTPUT);        // this is for the DCC Signal
    
    pinMode(DCC_PWM,OUTPUT);        // will be kept high, PWM pin
    digitalWrite(DCC_PWM,1);
    
    pinMode(led, OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(DCC_PIN, HIGH);
  delay(4000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(DCC_PIN, LOW);
  delay(4000); 

}
