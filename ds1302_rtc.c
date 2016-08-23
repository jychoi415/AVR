#include <io.h>
#include <delay.h>
#include <stdio.h>
#asm
    .equ __lcd_port=0x18
#endasm                     // PORTB
#include <lcd.h>

#define sbi(port,bit) (port) |= (1<<(bit))  // PORT*.0=1
#define cbi(port,bit) (port) &= ~(1<<(bit))  // PORT*.0=0

#define RTC_CLK 0
#define RTC_DAT 1
#define RTC_RST 2

#define ds1302_sec 0x80
#define ds1302_min 0x82
#define ds1302_hour 0x84
//#define ds1302_date 0x86
//#define ds1302_mon 0x88
//#define ds1302_day 0x8a
//#define ds1302_year 0x8c
#define ds1302_ctrl 0x8e
#define ds1302_charger 0x90

void ds1302_write(char adr, char data)
{
    char a;
    sbi(DDRA,RTC_DAT);
    sbi(PORTA,RTC_RST);

    for(a=0;a<8;a++)
    {
        cbi(PORTA,RTC_CLK);
        if(adr & 0x01) sbi(PORTA,RTC_DAT);
        else cbi(PORTA,RTC_DAT);
        sbi(PORTA,RTC_CLK);
        adr >>= 1;
    }

    for(a=0;a<8;a++)
    {
        cbi(PORTA, RTC_CLK);
        if(data & 0x01) sbi(PORTA,RTC_DAT);
        else cbi(PORTA,RTC_DAT);
        sbi(PORTA,RTC_CLK);
        data >>= 1;
    }
    cbi(PORTA,RTC_CLK);
    cbi(PORTA,RTC_RST);
}
char ds1302_read(char adr)
{
    char a, data=0;

    ++adr;
    sbi(DDRA,RTC_DAT);
    sbi(PORTA,RTC_RST);

    for(a=0;a<8;a++)
    {
        cbi(PORTA,RTC_CLK);
        if (adr & 0x01) sbi(PORTA,RTC_DAT);
        else cbi(PORTA,RTC_DAT);
        sbi(PORTA,RTC_CLK);
        adr >>= 1;
    }

    cbi(DDRA,RTC_DAT);
    for(a=0;a<8;a++)
    {
        cbi(PORTA,RTC_CLK);
        if(PINA.RTC_DAT) data |= 0x80;
        sbi(PORTA,RTC_CLK);
        data >>= 1;
    }
    cbi(PORTA,RTC_CLK);
    cbi(PORTA,RTC_RST);

    return data;
}
void main()
{
    char sec,min,hour;
    char tmp,TimeBuff[20];

    DDRA=0xff;
    lcd_init(16);
    lcd_gotoxy(5,0);
    lcd_puts("WATCH*");

    ds1302_write(ds1302_ctrl, 0x00);
    ds1302_write(ds1302_sec, 0x50);
    ds1302_write(ds1302_min,0x59);
    ds1302_write(ds1302_hour,0x11);
    ds1302_write(ds1302_ctrl, 0x80);

    while(1)
    {

        tmp=ds1302_read(ds1302_sec);
        sec=(((tmp & 0b01110000) >> 4)*10);
        sec+=(tmp & 0b00000111);

        tmp=ds1302_read(ds1302_min);
        min=(((tmp & 0b01110000) >> 4)*10);
        min+=(tmp & 0b00000111);

        tmp=ds1302_read(ds1302_hour);
        hour=(((tmp & 0b01110000) >> 4)*10);
        hour+=(tmp & 0b00000111);

        lcd_gotoxy(4,1);
        sprintf(TimeBuff,"%02d:%02d:%02d",hour,min,sec);
        lcd_puts(TimeBuff);
    }
}
