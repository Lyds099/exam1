#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2);
//DigitalIn up_button(D4);
//DigitalIn down_button(D5);
//DigitalIn confirm_button(D6);
InterruptIn up_button(D4);
InterruptIn down_button(D5);
InterruptIn confirm_button(D6);
DigitalIn mypin(USER_BUTTON);

AnalogOut Aout(D7);
AnalogIn Ain(A0);

static int choose_freq = 0;//0--4
static int set_freq = 0;
static int frequency = 20;
static int freq_change = 1;
static int freq_set = 0;

float ADCdata[250];
int sample_i;
int sample_rate = 400;

Thread t;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

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

void selection_done()
{
    set_freq = choose_freq;
    if(choose_freq==0) frequency = 1;
    else if(choose_freq==1) frequency = 2;
    else if(choose_freq==2) frequency = 4;
    else if(choose_freq==3) frequency = 8;
    freq_set = 1;

    queue.call(generation);
}

void generation()
{
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

void press_userbutton()
{
    queue.call(sampling);
}

void sampling()
{
    for(int i=0; i<200; i+=2){
        printf("%f\r\n", ADCdata[i]);
        ThisThread::sleep_for(10ms);
    }
}

void check_button(){//discard
    if(up_button){
        if(choose_freq>0){
            choose_freq--;
        }
        freq_change = 1;
    }else if(down_button){
        if(choose_freq<3){
            choose_freq++;
        }
        freq_change = 1;
    }else if(confirm_button){
        set_freq = choose_freq;
        if(choose_freq==0) frequency = 1;
        else if(choose_freq==1) frequency = 2;
        else if(choose_freq==2) frequency = 4;
        else if(choose_freq==3) frequency = 8;
        freq_set = 1;
    }
}

void display_freq(){//discard
    if(freq_change || freq_set){
        uLCD.cls();
        uLCD.text_width(2);
        uLCD.text_height(2);
        uLCD.textbackground_color(BLACK);
        uLCD.color(WHITE);
        uLCD.locate(2,1);
        uLCD.printf("Menu");
        for(int i=0; i<3; i++){
            if(freq_change && choose_freq==i) uLCD.textbackground_color(0x00CC66);
            else if(freq_set && set_freq==i) uLCD.textbackground_color(0x3333FF);
            else uLCD.textbackground_color(BLACK);
            uLCD.locate(2,2+i);
            if(i==0) uLCD.printf("%d",20);
            else if(i==1) uLCD.printf("%d",40);
            else if(i==2) uLCD.printf("%d",100);
            uLCD.printf("Hz");
        }
        freq_change = 0;
        freq_set = 0;
    }
}

int main()
{
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    up_button.rise(&flip_up);
    down_button.rise(&flip_down);
    confirm_button.rise(&selection_done);
    mypin.rise(&press_userbutton);

    // while(1){
    //     if(!mypin) break;
    //     check_button();
    //     display_freq();
    //     int period = 1000000/frequency;//set freq
    //     int T = period/20;
    //     if(frequency==20){
    //         sample_i = 0;
    //         for (int p=0; p<20; p++){
    //             for (float i = 0.0f; i < 1.0f; i += 0.1f) {
    //                 Aout = i*0.92;
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //             for (float i = 1.0f; i > 0.0f; i -= 0.1f) {
    //                 Aout = i*0.92;
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //         }
    //     }
    //     else if(frequency==40){
    //         sample_i = 0;
    //         for (int p=0; p<40; p++){
    //             for (float i = 0.0f; i < 1.0f; i += 0.1f) {
    //                 Aout = i*0.92;
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //             for (float i = 1.0f; i > 0.0f; i -= 0.1f) {
    //                 Aout = i*0.92;  
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //         }
    //     }
    //     else if(frequency==100){
    //         sample_i = 0;
    //         for (int p=0; p<100; p++){
    //             for (float i = 0.0f; i < 1.0f; i += 0.1f) {
    //                 Aout = i*0.92;
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //             for (float i = 1.0f; i > 0.0f; i -= 0.1f) {
    //                 Aout = i*0.92;
    //                 ADCdata[sample_i++] = Ain;
    //                 wait_us(T);
    //             }
    //         }
    //     }
    // }
    // if(frequency==20){
    //     for(int i=0; i<400; i++){
    //         printf("%f\r\n", ADCdata[i]);
    //         ThisThread::sleep_for(10ms);
    //     }
    // }
    // else if(frequency==40){
    //     for(int i=0; i<800; i+=2){
    //         printf("%f\r\n", ADCdata[i]);
    //         ThisThread::sleep_for(10ms);
    //     }
    // }
    // else if(frequency==100){
    //     for(int i=0; i<2000; i+=5){
    //         printf("%f\r\n", ADCdata[i]);
    //         ThisThread::sleep_for(10ms);
    //     }
    // }
}