#include <io.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lcd.h>
#asm
.equ __lcd_port=0x15
#endasm

unsigned char key();
unsigned char key_value;

void key_scan();
void lcd_cls();
void var_init();

void main(void)
{
    int index=0,result=0;
    int check = 0; 
    unsigned char left[10], right[10];
    unsigned char sbuf[64],pbuf[2];    // sbuf=�ӽ� Ű �ޱ�,. pbuf=lcd����
    DDRA=0x0f;
    SFIOR=0x00;
    PORTA=0xff;

    lcd_cls();
    while (1)
    {
        key_scan();
        if(key_value != 0xf0 && key_value != 0x0e)
        {                                          
            sprintf(pbuf,"%d",key_value);
            
            if(!strcmp(pbuf,"10"))
            {
                strcpy(pbuf,"+");
                strcpy(left, sbuf);
                strcpy(sbuf, "");
                index = 0;
                check = 1;
            }
            else if(!strcmp(pbuf,"11"))
            {
                strcpy(pbuf,"-");
                strcpy(left, sbuf);
                strcpy(sbuf, "");
                index = 0;
                check = 2;
            }
            else if(!strcmp(pbuf,"12"))
            {
                strcpy(pbuf,"*");  
                strcpy(left, sbuf);
                strcpy(sbuf, "");
                index = 0;
                check = 3;
            }
            else if(!strcmp(pbuf,"13"))
            {
                strcpy(pbuf,"/");  
                strcpy(left, sbuf);
                strcpy(sbuf, "");
                index = 0;
                check = 4;
            }
            else if(!strcmp(pbuf,"15"))
            {
                strcpy(pbuf,"=");   
                index = 0;
            }                                                      
            else
                strcat(sbuf, pbuf);
                             
            lcd_puts(pbuf);

            if(key_value == 0x0f)
            { 
                lcd_cls();                    
                strcpy(right, sbuf); 
                  
                switch(check)
                {
                    case 1: result=atoi(left)+atoi(right); break;
                    case 2: result=atoi(left)-atoi(right); break;
                    case 3: result=atoi(left)*atoi(right); break;
                    case 4: result=atoi(left)/atoi(right); break;
                }                       
                
                sprintf(pbuf, "%d", result);
                lcd_puts(pbuf);            
                
                var_init();
            }
            ++index;                               
        }   
        else if(key_value == 0x0e){
            lcd_cls();
            var_init();
        }
    }
}

unsigned char key()
{
    unsigned char a,key_buf;
    unsigned char row=0xfe;

    PORTA=0xff;     // lcd ����

    for(a=0;a<4;a++)
    {
        PORTA=row;
        delay_us(20);
        key_buf=PINA;
        switch(PINA)
        {
            case 0xd7: key_buf=0x00; break;     // 0
            case 0xee: key_buf=0x01; break;     // 1
            case 0xde: key_buf=0x02; break;     // 2
            case 0xbe: key_buf=0x03; break;     // 3
            case 0xed: key_buf=0x04; break;     // 4
            case 0xdd: key_buf=0x05; break;     // 5
            case 0xbd: key_buf=0x06; break;     // 6
            case 0xeb: key_buf=0x07; break;     // 7
            case 0xdb: key_buf=0x08; break;     // 8
            case 0xbb: key_buf=0x09; break;     // 9
            case 0x7e: key_buf=0x0a; break;     // +
            case 0x7d: key_buf=0x0b; break;     // -
            case 0x7b: key_buf=0x0c; break;     // *
            case 0x77: key_buf=0x0d; break;     // /
            case 0xe7: key_buf=0x0e; break;     // =
            case 0xb7: key_buf=0x0f; break;     // C
            default : key_buf=0xf0;
        }
        if(key_buf != 0xf0) return key_buf;
        row=(row<<1)|0x01;
    }
    return key_buf;
}
void key_scan()
{
    key_value=key();
    if(key_value != 0xf0)
        while(key() != 0xf0);
}
void lcd_cls()
{
    lcd_init(16);
    lcd_gotoxy(3,0);
    lcd_puts("Calculator");
    lcd_gotoxy(0,1);
}
void var_init()
{
    index=0,check=0,result=0; 
    strcpy(left, ""); strcpy(right, ""); strcpy(sbuf, ""); strcpy(pbuf, "");    
}
