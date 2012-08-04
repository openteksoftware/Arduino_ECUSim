/*  Fal(lgal)gen - physikalischer Fallversuch */
//////////////////////////////
#include <avr/pgmspace.h>
//////////////////////////////
//////////////////////////////
// MODE 01                  //
//////////////////////////////
float p01m0C_range[2] = {
  0, 16383.75};
int   p01m0D_range[2] = {
  0, 255};
//////////////////////////////
bool gotDTC = true;
//////////////////////////////
prog_char empty[]   PROGMEM  = "";

prog_char help_head[]    PROGMEM = "ELM327-SIMU by OBDX (http://obdx.blogspot.de)";
prog_char help_content[] PROGMEM = "The following commands are available:";
prog_char help_cmd1[]    PROGMEM = " * HELP              ->  Zeigt dieses Menue an";
prog_char help_end[]     PROGMEM = "Make sure there are spaces between the commands and options!";

prog_char OBDXX[]   PROGMEM = "XX";
prog_char OBD01[]   PROGMEM = "01";
prog_char OBD02[]   PROGMEM = "02";
prog_char OBD03[]   PROGMEM = "03";
prog_char OBD04[]   PROGMEM = "04";
prog_char OBD05[]   PROGMEM = "05";
prog_char OBD06[]   PROGMEM = "06";
prog_char OBD07[]   PROGMEM = "07";
prog_char OBD08[]   PROGMEM = "08";
prog_char OBD09[]   PROGMEM = "08";
prog_char ELM327[]  PROGMEM = "AT";

PROGMEM const char *serial_list[] = {
  OBDXX,
  OBD01,
  OBD02,
  OBD03,
  OBD04,
  OBD05,
  OBD06,
  OBD07,
  OBD08,
  OBD09,
  ELM327,
};

PROGMEM const char *help_list[]    = {   
  help_head,
  empty,
  empty,
  help_content,
  empty,

  help_cmd1,

  empty,
  help_end,
  empty,
  empty
};

const int len_help = 9;
//////////////////////////////

char* fSerialFrame    ;
char* sSerialFrame    ;
char* tSerialFrame    ;

bool serialIn(){
  const int waitBuff = 250;
  int prevBuff = 0;
  char string[1000]     ;
  if (Serial.available()){
    while (Serial.available() != prevBuff) {
      delay(waitBuff);
      prevBuff = Serial.available();
    }
    while (Serial.available()) {
      for(int i=0; i<prevBuff; i++)
      {
        string[i] = Serial.read();
        string[i+1] = '\0'; // Append a null
      }
      char *p = string;
      fSerialFrame = strtok_r(p, " ", &p);
      sSerialFrame = strtok_r(p, " ", &p);
      tSerialFrame = strtok_r(p, " ", &p);
    };
    return true;
  } 
  else return false;
}

char* ramstr(const char** list, int pos){
  char ram2str[64];
  strcpy_P(ram2str, (char*)pgm_read_word(&(list[pos])));
  return ram2str;
}

String int2hex(int num) {
  String string = String(num, HEX);
  string.toUpperCase();
  if (string.length() < 2){
    String string_ =  String("0");
    string_.concat(string);
    string = string_;
  }
  return string;
}

void elmRepeat() {
  Serial.print(fSerialFrame);
  Serial.print(" ");
  Serial.print(sSerialFrame);
  Serial.print(" \r"); 
  return;
}
void elmArrow() {
  Serial.print("\r\r>");
  return;
}

void elmPrint(int pid, int mode, String retr) {
  Serial.print(pid+40);
  Serial.print(" ");
  Serial.print(int2hex(mode));
  Serial.print(" ");
  Serial.print(retr);
  elmArrow();
  return;
}

int* p01m0C(float number) {
  int ganz;
  float gleit;
  number = number * 4;
  ganz = number / 256;
  gleit = number / 256;
  gleit = gleit - ganz;
  gleit = 256 * gleit;
  /*Serial.print("##");
   Serial.print(number);
   Serial.print("##");
   Serial.print(ganz);
   Serial.print("##");
   Serial.print(gleit);
   Serial.print("##");*/
  int result[2] = {
    ganz, gleit            };
  return result;
}

void elm_p01m0C(int pid, int mode, float num) {
  /*Serial.print("##");
   Serial.print(num);
   Serial.print("##");*/
  int* value = p01m0C(num);
  String printing;
  printing += String(int2hex(value[0]));
  printing += String(" ");
  printing += String(int2hex(value[1]));
  elmPrint(pid, mode, printing);
  return;
}

void elm_p01m0C_rand(int pid, int mode) {
  float randomized = (float)random(p01m0C_range[0], p01m0C_range[1]) + random(0,4)/4.;
  /*Serial.print("##");
   Serial.print(randomized);
   Serial.print("##");*/
  elm_p01m0C(pid, mode, randomized);
  return;
}

void elm_p01m0D(int pid, int mode, float num) {
  elmPrint(pid, mode, int2hex(num));
  return;
}


void elmQuestPrint() {
  Serial.print("?\r\r");
  elmArrow();
  return;
}

void elmNoData() {
  Serial.print("NO DATA\r\r");
  elmArrow();
  return;
}

void setup() {
  Serial.begin(9600);
  Serial.print("\r\r\rELM327 v1.4b");
  elmArrow();
}

void loop() {
  if (serialIn()) {
    elmRepeat();
    if (strcmp(fSerialFrame, ramstr(serial_list, 0)) == 0) {
      if (strcmp(sSerialFrame, "HELP") == 0) {
        for (int i = 0; i <= len_help; i++)
        {
          Serial.println( ramstr(help_list, i));
        };
      } 
      else elmQuestPrint(); // When no MODE matches for PID XX
    } 
    else {
      if (strcmp(fSerialFrame, ramstr(serial_list, 1)) == 0) {
        if (strcmp(sSerialFrame, "00") == 0) {
          elmPrint(1, 0, "00 10 00 00");
        } 
        else {
          if (strcmp(sSerialFrame, "0C") == 0) {
            elm_p01m0C_rand(1, 12);
          } 
          else {
            if (strcmp(sSerialFrame, "0D") == 0) {
              elm_p01m0D(1, 13, 100);
            } 
            else elmNoData(); // When no MODE matches for PID 01
          }
        }
      }
      else {
        if (strcmp(fSerialFrame, ramstr(serial_list, 2)) == 0) {
          if (strcmp(sSerialFrame, "00") == 0) {
            if (gotDTC) {
              Serial.print("42 00 E8 00 00 00 00");
              elmArrow();
            } 
            else {
              Serial.print("42 00 00 00 00 00 00");
              elmArrow();
            }
          } 
          else elmNoData(); // When no MODE matches for PID 02
        }
        else {
          if (strcmp(fSerialFrame, ramstr(serial_list, 3)) == 0) {
            if (strcmp(sSerialFrame, "00") == 0) {
              if (gotDTC) {
                Serial.print("43 02 02 02 04 00 00");
                elmArrow();
              } 
              else {
                Serial.print("43 00 00 00 00 00 00");
                elmArrow();
              }
            } 
            else elmNoData(); // When no MODE matches for PID 03
          }
          else {
            if (strcmp(fSerialFrame, ramstr(serial_list, 4)) == 0) {
              if (strcmp(sSerialFrame, "00") == 0) {
                if (gotDTC) {
                  gotDTC = false;
                  Serial.print("44");
                  elmArrow();
                } 
                else {
                  gotDTC = true;
                  Serial.print("44");
                  elmArrow();
                }
              } 
              else elmNoData(); // When no MODE matches for PID 04
            }
            else {
              if (strcmp(fSerialFrame, ramstr(serial_list, 5)) == 0) {
                if (strcmp(sSerialFrame, "00") == 0) {
                  Serial.print("7F 05 22 ");
                  elmArrow();
                } 
                else elmNoData(); // When no MODE matches for PID 05
              }
              else {
                if (strcmp(fSerialFrame, ramstr(serial_list, 6)) == 0) {
                  if (strcmp(sSerialFrame, "00") == 0) {
                    Serial.print("46 00 FF 80 00 00 00 ");
                    elmArrow();
                  } 
                  else elmNoData(); // When no MODE matches for PID 06
                }
                else {
                  if (strcmp(fSerialFrame, ramstr(serial_list, 7)) == 0) {
                    if (strcmp(sSerialFrame, "00") == 0) {
                      Serial.print("47 01 35 01 15 03 00 \r47 03 04 03 02 00 00");
                      elmArrow();
                    } 
                    else elmNoData(); // When no MODE matches for PID 07
                  }
                  else {
                    if (strcmp(fSerialFrame, ramstr(serial_list, 8)) == 0) {
                      if (strcmp(sSerialFrame, "00") == 0) {
                        Serial.print("48 00 00 00 00 00 00 ");
                        elmArrow();
                      } 
                      else elmNoData(); // When no MODE matches for PID 08
                    }
                    else {
                      if (strcmp(fSerialFrame, ramstr(serial_list, 9)) == 0) {
                        if (strcmp(sSerialFrame, "00") == 0) {
                          Serial.print("7F 09 11"); //???
                          elmArrow();
                        } 
                        else elmNoData(); // When no MODE matches for PID 09
                      }
                      else {
                        if (strcmp(fSerialFrame, ramstr(serial_list, 10)) == 0) {
                          if (strcmp(sSerialFrame, "Z") == 0) {
                            setup();
                            elmArrow();
                          } 
                          else elmNoData(); // When no MODE matches for PID AT
                        }
                        else elmQuestPrint(); // When no PID matches
                      }
                    }
                  }
                }
              }
            }
          }
        }
      };
    }
  };
}

/*
EEPROM.write(0,(menuDelay/500));
 menuDelay    = (EEPROM.read(0)*500);
 
 int freemem(){
 extern int  __bss_end;
 extern int  *__brkval;
 int free_memory;
 if((int)__brkval == 0)
 free_memory = ((int)&free_memory) - ((int)&__bss_end);
 else
 free_memory = ((int)&free_memory) - ((int)__brkval);
 return free_memory;
 } 
 
 bool v4051Read(int shift, int pin){
 if (pin == 0) {                   
 digitalWrite(v4051s0, LOW );  
 digitalWrite(v4051s1, LOW );  
 digitalWrite(v4051s2, LOW );  
 }
 if (pin == 1) {                   
 digitalWrite(v4051s0, HIGH);  
 digitalWrite(v4051s1, LOW );  
 digitalWrite(v4051s2, LOW );  
 }
 if (pin == 2) {                   
 digitalWrite(v4051s0, LOW );  
 digitalWrite(v4051s1, HIGH);  
 digitalWrite(v4051s2, LOW );       
 }
 if (pin == 3) {                  
 digitalWrite(v4051s0, HIGH);  
 digitalWrite(v4051s1, HIGH);  
 digitalWrite(v4051s2, LOW );  
 }
 if (pin == 4) {                 
 digitalWrite(v4051s0, LOW );  
 digitalWrite(v4051s1, LOW );  
 digitalWrite(v4051s2, HIGH);    
 }
 if (pin == 5) {                  
 digitalWrite(v4051s0, HIGH);  
 digitalWrite(v4051s1, LOW );  
 digitalWrite(v4051s2, HIGH);  
 }
 if (pin == 6) {                   
 digitalWrite(v4051s0, LOW );  
 digitalWrite(v4051s1, HIGH);  
 digitalWrite(v4051s2, HIGH);  
 }
 if (pin == 7) {                  
 digitalWrite(v4051s0, HIGH);  
 digitalWrite(v4051s1, HIGH);  
 digitalWrite(v4051s2, HIGH);  
 }
 return (analogRead(shift) >= 1010); // 1010, da mänche Eingänge noch hohe Werte ausgeben
 }
 
 /// MENUFUERUNG VIA SCHALTER
 
 bool isMenu() {
 return v4051Read(v4051IN2,ledtaster);
 }
 
 if (isMenu()){
 int counter=0;
 int timecounter=0;
 while ((timecounter <=(menuDelay*2))&&(isMenu())){
 analogWrite(ledTaster,   0);  
 analogWrite(ledStat, 255);
 delay(1);
 timecounter++;
 }
 while (isMenu()){
 if (isMenu()){
 analogWrite(ledTaster, 255);  
 analogWrite(ledStat,   0);
 delay(menuDelay);
 counter++;
 }
 if (isMenu()){
 analogWrite(ledTaster,   0);  
 analogWrite(ledStat, 255);
 delay(menuDelay);
 counter++;
 }
 };
 
 if (counter==1) {
 if (EEPROM.read(0)>1){
 EEPROM.write(0,(EEPROM.read(0)-1));
 menuDelay    = (EEPROM.read(0)+1)*500;
 };
 };
 /// WEITERE MENUEINTRAEGE counter == x
 
 if (counter>10&&counter<=15) {
 analogWrite(ledTaster, 255);
 analogWrite(ledStat,   0);
 runExperiment(true);
 };
 if (counter>15) {
 setStandarts();
 };
 
 }
 else{
 for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=2) {
 analogWrite(ledTaster, fadeValue);  
 analogWrite(ledStat, ((fadeValue-255)*-1));  
 delayMicroseconds(100*chillDelay);                            
 } 
 
 for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=2) {
 analogWrite(ledTaster, fadeValue);  
 analogWrite(ledStat, ((fadeValue-255)*-1));  
 delayMicroseconds(100*chillDelay);                            
 }
 };
 };
 
 */

















