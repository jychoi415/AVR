// Receive analog value and show it by 7-segment

#include <io.h>
#include <delay.h>

void main(void)
{
    int da_value;    

    DDRA=0xff;
    ADCSRA=0x87;    
    ADMUX=0x01;
    
    while (1)
    {
        ADCSRA=ADCSRA | 0x40;
        while((ADCSRA & 0x10) ==0);
        da_value=(int)ADCL+((int)ADCH<<8);
        da_value=(int)(da_value/1023.0*9);
        
        PORTA=da_value;
        delay_ms(500);
    }
}
