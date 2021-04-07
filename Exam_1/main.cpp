#include "mbed.h"
#include "uLCD_4DGL.h"
#include "mbed_events.h"
using namespace std::chrono;

uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn up_button(D4);
InterruptIn down_button(D5);
InterruptIn confirm_button(D6);
InterruptIn mypin(USER_BUTTON);

AnalogOut Aout(D7);
AnalogIn Ain(A0);

static int choose_freq = 0;//0--4
static int set_freq = 0;
static int frequency = 1;
static int freq_change = 1;
static int freq_set = 0;

float ADCdata[250];
int sample_i;
int sample_rate = 200;

EventQueue G_Queue;
EventQueue S_Queue;

void display_freq(){
    if(freq_change || freq_set){
        uLCD.cls();
        uLCD.text_width(2);
        uLCD.text_height(2);
        uLCD.textbackground_color(BLACK);
        uLCD.color(WHITE);
        uLCD.locate(2,1);
        uLCD.printf("Menu");
        for(int i=0; i<4; i++){
            if(freq_change && choose_freq==i) uLCD.textbackground_color(0x00CC66);
            else if(freq_set && set_freq==i) uLCD.textbackground_color(0x3333FF);
            else uLCD.textbackground_color(BLACK);
            uLCD.locate(2,2+i);
            if(i==0) uLCD.printf("1");
            else if(i==1) uLCD.printf("1/2");
            else if(i==2) uLCD.printf("1/4");
            else if(i==3) uLCD.printf("1/8");
            uLCD.printf("rate");
        }
        freq_change = 0;
        freq_set = 0;
    }
}

void flip_up()
{
   if(choose_freq>0){
            choose_freq--;
        }
        freq_change = 1;
}

void flip_down()
{
   if(choose_freq<3){
            choose_freq++;
        }
        freq_change = 1;
}

void generation()
{
  while(1){
    if(frequency==8){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0.0f; i < 1.0f; i += (1.0f/16.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
                Aout = 0.92;
                for(int j=0; j<16; j++){
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }//80
                for (float i = 1.0f; i > 0.0f; i -= (1.0f/16.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
            }
        }
    else if(frequency==4){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0.0f; i < 1.0f; i += (1.0f/8.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
                Aout = 0.92;
                for(int j=0; j<32; j++){
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }//160
                for (float i = 1.0f; i > 0.0f; i -= (1.0f/8.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
            }
        }
    else if(frequency==2){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0.0f; i < 1.0f; i += (1.0f/4.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
                Aout = 0.92;
                for(int j=0; j<40; j++){
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }//200
                for (float i = 1.0f; i > 0.0f; i -= (1.0f/4.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
            }
        }
    else if(frequency==1){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0.0f; i < 1.0f; i += (1.0f/2.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
                Aout = 0.92;
                for(int j=0; j<44; j++){
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }//220
                for (float i = 1.0f; i > 0.0f; i -= (1.0f/2.0f)) {
                    Aout = i*0.92;
                    ADCdata[sample_i++] = Ain;
                    ThisThread::sleep_for(5ms);
                }
            }
        }
  }
}

void selection_done()
{
    set_freq = choose_freq;
    if(choose_freq==0) frequency = 1;
    else if(choose_freq==1) frequency = 2;
    else if(choose_freq==2) frequency = 4;
    else if(choose_freq==3) frequency = 8;
    freq_set = 1;
    
    G_Queue.call(&generation);
}

void sampling()
{
    for(int i=0; i<200; i+=2){
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(5ms);
    }
}

void press_userbutton()
{
    S_Queue.call(&sampling);
}

int main()
{
    Thread g_Thread(osPriorityLow);
    g_Thread.start(callback(&G_Queue, &EventQueue::dispatch_forever));

    Thread s_Thread(osPriorityNormal);
    s_Thread.start(callback(&S_Queue, &EventQueue::dispatch_forever));
    
    up_button.rise(&flip_up);
    down_button.rise(&flip_down);
    confirm_button.rise(&selection_done);
    mypin.rise(&press_userbutton);

    while(1){
        display_freq();
    }
}