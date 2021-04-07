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

float ADCdata[300];
int sample_i;
int sample_rate = 200;

Thread g_Thread(osPriorityLow);
Thread s_Thread(osPriorityNormal);
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
    if(frequency==1){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0; i < 16; i += 1) {
                    Aout = i*0.0625*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
                Aout = 0.92;
                for(int j=0; j<16; j++){
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }//80
                for (float i = 16; i > 0; i -= 1) {
                    Aout = i*0.0625*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
            }
    }
    else if(frequency==2){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0; i < 8; i += 1) {
                    Aout = i*0.125*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
                Aout = 0.92;
                for(int j=0; j<32; j++){
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }//160
                for (float i = 8; i > 0; i -= 1) {
                    Aout = i*0.125*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
            }
    }
    else if(frequency==4){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0; i < 4; i += 1) {
                    Aout = i*0.25*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
                Aout = 0.92;
                for(int j=0; j<40; j++){
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }//200
                for (float i = 4; i > 0; i -= 1) {
                    Aout = i*0.25*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
            }
    }
    else if(frequency==8){
            sample_i = 0;
            for (int p=0; p<5; p++){
                for (float i = 0; i < 2; i += 1) {
                    Aout = i*0.5*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }
                Aout = 0.92;
                for(int j=0; j<44; j++){
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
                }//220
                for (float i = 2; i > 0; i -= 1) {
                    Aout = i*0.5*0.92;
                    ADCdata[sample_i++] = Ain;
                    wait_us(5000);
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
    for(int i=0; i<200; i+=1){
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
    g_Thread.start(callback(&G_Queue, &EventQueue::dispatch_forever));
    s_Thread.start(callback(&S_Queue, &EventQueue::dispatch_forever));
    
    up_button.rise(&flip_up);
    down_button.rise(&flip_down);
    confirm_button.rise(&selection_done);
    mypin.rise(&press_userbutton);

    while(1){
        display_freq();
    }
}