// This is Arduino based toiletflusher code.
////////////////////////////////////////////
const int RADAR_PIN = 2;
const int SERVO_PIN = 3;
const int GERCON_PIN = 4;
const int REDLED_PIN = 5;
const int GREENLED_PIN = 6;

const int STATE_INIT        = 0;
const int STATE_READY       = 1;
const int STATE_PREWAIT     = 2;
const int STATE_WAIT        = 3;
const int STATE_DETECT      = 4;
const int STATE_PREARMED    = 5;
const int STATE_ARMED       = 6;
const int STATE_FLUSH       = 7;
const int STATE_FLUSH_GERCON = 8;
const int STATE_AFTERFLUSH  = 9;
const int STATE_IDLE        = 10;
const int STATE_GERCON      = 11;

const int LEDOFF = 0;
const int LEDON = 1;
const int LEDBLINK = 2;

unsigned int state = 0;
unsigned long stateStart = 0;
unsigned long currentTime;

int AFTERFLUSH_TIME = 2;
int FLUSH_TIME  =  5;
int IDLE_TIME =   30;
int ARMED_TIME =  180;
int WAIT_TIME =   30;
int DETECT_TIME = 120;
int GERCON_TIME = 10;

void setup() {
  pinMode(RADAR_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(GERCON_PIN,INPUT_PULLUP);
  pinMode(REDLED_PIN, OUTPUT);
  pinMode(GREENLED_PIN, OUTPUT);

  digitalWrite(SERVO_PIN, 0);
  digitalWrite(REDLED_PIN, 0);
  digitalWrite(GREENLED_PIN, 0);

  stateStart = micros();
  state = STATE_INIT;
}


void loop() {
  currentTime = micros();

  if (currentTime < stateStart) {
    stateStart = currentTime;
  }

  int oldState = state;

  switch(state) {
    case STATE_INIT:
      setLEDs(LEDOFF, LEDON);    
      pulse(0);
      
      if (stateSecs() > AFTERFLUSH_TIME) {  
        state = STATE_READY;      
      }
      
      break;
      
    case STATE_READY:
      setLEDs(LEDOFF, LEDOFF);    
      
      if (isInfraSensorON())  {            
          state = STATE_PREWAIT;
      }

      if (isGerconON()) {
        state = STATE_GERCON;  
      }
      
      break;
      
    case STATE_PREWAIT:
      setLEDs(LEDON, LEDOFF);      
      
      if (!isInfraSensorON())  {   
          state = STATE_WAIT;
      }

      if (isGerconON()) {
        state = STATE_GERCON;  
      }
      
      break;
      
    case STATE_WAIT:
      setLEDs(LEDBLINK, LEDOFF);         
      
      if (stateSecs() > WAIT_TIME) {      
          state = STATE_DETECT;
      }

      if (isGerconON()) {
        state = STATE_GERCON;  
      }
            
      break;

     case STATE_DETECT:
      setLEDs(LEDON, LEDOFF);      
      
      if (stateSecs() > DETECT_TIME) {      
          state = STATE_READY;
      }
      
      if (isInfraSensorON())  {           
          state = STATE_PREARMED;
      }
      
      if (isGerconON()) {
        state = STATE_GERCON;  
      }
          
      break;

     case STATE_PREARMED:
      setLEDs(LEDON, LEDON);         
      
      if (!isInfraSensorON())  {           
          state = STATE_ARMED;
      }
      
      if (isGerconON()) {
        state = STATE_GERCON;  
      }
            
      break;

     case STATE_ARMED:
      setLEDs(LEDOFF, LEDON);        
      
      if (isInfraSensorON())  {           
          state = STATE_PREARMED;                
      } 
      
      if (stateSecs() > ARMED_TIME) {              
          state = STATE_FLUSH;
      }
      
      if (isGerconON()) {
        state = STATE_GERCON;  
      }
            
      break;

    case STATE_FLUSH:
      setLEDs(LEDOFF, LEDOFF);             
      pulse(1);

      if (stateSecs() > FLUSH_TIME) {
        state = STATE_AFTERFLUSH;
      }
      
      if (isGerconON()) {
        state = STATE_FLUSH_GERCON;  
      }         
      break;

    case STATE_FLUSH_GERCON: 
      setLEDs(LEDON, LEDON);
      pulse(1);   
      
      if (stateSecs() > FLUSH_TIME) {
        state = STATE_AFTERFLUSH;
      }   
      break;      
      
    case STATE_AFTERFLUSH:
      setLEDs(LEDOFF, LEDOFF);          
      pulse(0);

      if (stateSecs() > AFTERFLUSH_TIME) {                
        state = STATE_READY;
      }
      
      break;

    case STATE_GERCON:
      setLEDs(LEDBLINK, LEDBLINK);      
                
      if (stateSecs() > GERCON_TIME) {
        state = STATE_AFTERFLUSH;
      }

      if (!isGerconON()) {
        state = STATE_IDLE;
      }
      break;

    case STATE_IDLE:
      setLEDs(LEDOFF, LEDBLINK);      
      
      if (stateSecs() > IDLE_TIME) {            
        state = STATE_READY;
      }
      break;
  }
  
  if (oldState != state) {
    stateStart = currentTime;
  }

  delayMicroseconds(20000); 
}

void setLEDs(int redState, int greenState) 
{
  setLED(REDLED_PIN, redState);
  setLED(GREENLED_PIN, greenState);
}

int isInfraSensorON()
{
  return digitalRead(RADAR_PIN);
}


int isGerconON()
{
  return !digitalRead(GERCON_PIN);  
}

void setLED(int led, int ledState) 
{
  switch(ledState) {
    case LEDOFF:
      digitalWrite(led, 0);
      break;
      
    case LEDON:
      digitalWrite(led, 1);
      break;

    case LEDBLINK:
      digitalWrite(led, (currentTime / 500000) % 2);
      break;
  }
}

int stateSecs()
{
  return (currentTime - stateStart) / 1000000;
}


void pulse(boolean isOn) 
{
    digitalWrite(SERVO_PIN, 1);
    delayMicroseconds(isOn ? 1850 : 1250);
    digitalWrite(SERVO_PIN, 0);
}
