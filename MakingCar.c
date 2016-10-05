#include <delay.h>
#include <stdio.h>
#include <io.h>
#include <lcd.h>
#asm
.equ __lcd_port = 0x1B // PORTA
#endasm

#define SW PINF.2
#define TRIG PORTE.6

char buf[20];
int a,b,c,d;
int catch, distance, danger;
void lcd_display();

int adc_input(int port)
{
    int value;
    
    ADMUX = port | 0x40;
    ADCSRA = ADCSRA | 0x40;             //start 신호
    while((ADCSRA & 0x10) == 0);
    value = (int)ADCL + ((int)ADCH<<8);
    value = ((int)value/1023.0)*255;
    
    return value;
}
int speed(void)
{
    return adc_input(0);    
}
int LR(void)
{
    return adc_input(1);
}
void ultra_stop(void)
{
    char info[16];
    
    lcd_clear();
    lcd_gotoxy(0,0);
    lcd_puts("FIND OBSTACLE!");
    lcd_gotoxy(0,1);
    sprintf(info,"Distance : %d",distance);
    lcd_puts(info);
}
void way(void)         
{
    int nSpeed = speed();
    int nWay = LR();
    
    if (danger == 1)
    {
        OCR1A = 0;
        OCR3AL = 0;
        OCR1B = 0;
        OCR3BL = 0;

        return;
    }
    if (SW == 1)
    {
        lcd_gotoxy(15,0);
        lcd_puts("F");

        if (nWay >= 120)
        {
            a = nSpeed + (120 - nWay);
            if (a<0)a=0;
            b = a;
            c = nSpeed;
            d = c;           
        }
        else if (nWay <= 100)
        {
            a = nSpeed;
            b = a;
            c = nSpeed - (100 - nWay);
            if (c<0)c=0;
            d = c;
        }
    }
    else
    {
        lcd_gotoxy(15,0);
        lcd_puts("B");
            
        if (nWay >= 120)
        {
            a = nSpeed;
            b = a;
            c = nSpeed + (120 - nWay);
            if (c<0)c=0;
            d = c;
        }
        else if (nWay <= 100)
        {
            a = nSpeed - (100 - nWay);
            if (a<0)a=0;
            b = a;
            c = nSpeed;
            d = c;
        }
    }
    if (SW == 1)
    {
        b=0;
        d=0;
    }
    else if (SW == 0)
    {
        a=0;
        c=0;
    }
    OCR1A = a;
    OCR1B = b;
    OCR3AL = c;
    OCR3BL = d;    
}
void display(void)
{
    if (danger == 1)
        ultra_stop();
    else
    {
        sprintf(buf,"1A:%3d 1B:%3d ",OCR1A,OCR1B);        
        lcd_gotoxy(0,0);
        lcd_puts(buf);  
        sprintf(buf,"3A:%3d 3B:%3d",OCR3AL,OCR3BL);
        lcd_gotoxy(0,1);
        lcd_puts(buf);
    } 
}
void main(void)
{
    DDRF = 0x00;            // 속도 가변 입력
    DDRB = 0b01100000;
    DDRE = 0b01011000;
    
    ADCSRA = 0x87;
    ADMUX = 0x00;           // PF0   
   
    EIMSK=0b00100000;   // IN5 enable(초음파센서)
    EICRB=0b00001000;   // INT5 하강 에지
    
    TIMSK=0x01;    
    TCCR0=0x00;
    
    TCNT1 = 0x00;
    TCCR1A = 0b10100001;    // OCRnX 출력 클리어, TOP에서 출력 셋                                   
    TCCR1B = 0b00001101;    // 분주비 1024, Fast PWM 8bit 
    
    TCNT3H = 0x00;
    TCNT3L = 0x00;
    TCCR3A = 0b10100001;
    TCCR3B = 0b00001101;
    OCR3AH = 0;
    OCR3BH = 0;
             
    TRIG = 0;
    lcd_init(16);
     
    while (1)
    {           
        SREG=0x00;
        distance=0;
        
        TRIG=1;
        delay_us(10);
        TRIG=0;
        
        TCNT0=139;
        TCCR0=0x02;   
        SREG=0x80;
        
        catch=0;
        while(1)
        {
            if (catch==1)
            {
                danger = 1;
                break;
            }
            else if (distance >= 20)
            {
                danger = 0;
                break;
            }
        }             
        way();  // 전후좌우 컨트롤
        display();  // lcd                        
    }
}
interrupt [TIM0_OVF] void time0_ovf(void)
{
    TCNT0 = 139;
    distance++;
}
interrupt [EXT_INT5] void ext_int5(void)
{
    TCCR0 = 0x00;
    catch = 1;    
}