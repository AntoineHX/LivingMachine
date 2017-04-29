#include "mbed.h"

DigitalOut led2(P0_5);
DigitalOut led1(LED1);
PwmOut servo1(P2_5);
PwmOut servo2(P2_4);
//Serial pc(USBTX,USBRX);
Serial pc(USBTX,USBRX);
Thread mythread;

void console_thread(){
   char buff[255];
   char *c0;
   char *c;
   int vals[3];
   int num=0;
   //pc.printf("Ask for servos with XXX,YYY \n\r(where XXX and YYY are integers 1/1000 of 20ms)\n\r");   //Commented in RELEASE
   
    while (true) {
        c0=buff;
        c=buff;
        for(num=0;num<2;){
            
            *c=pc.getc(); // echo input back to terminal
            pc.putc(*c);
            if(*c == 'q')
                break ; 
            if((*c == '\n') || (*c == '\r') || (*c == '\0') || (*c == ' ') || (*c == ','))
            {
                //
                *c='\0'; 
                vals[num]=atoi(c0);
                c0=c+1;
                num++;
            }
            c++;
          }
          float s_vals[2];
          int idx=0;
          for(idx=0;idx<2;idx++){
              if(vals[idx]<0) vals[idx]=0;
              if(vals[idx]>1000) vals[idx]=1000;
              s_vals[idx]=vals[idx]*0.001;
            }
          servo1=s_vals[0];
          servo2=s_vals[1];
          
          pc.printf("%d,%d \n",vals[0],vals[1]);
          wait(0.5);
     
          

    }
}

int main() {
    mythread.start(console_thread);
    int mystate=0;
    servo1.period_ms(20);
    servo2.period_ms(20);
    while (true) {
        led2 = !led2;
        led1 = !led1;
        mystate = !mystate;
        wait(0.5);
    }
}


