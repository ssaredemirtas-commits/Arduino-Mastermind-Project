
#include "libADC.hpp"

void ADC_Init(void){
    
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    ADMUX  = (1<<REFS0); 
}

int ADC_conversion(void){
    ADCSRA |= (1<<ADSC);        
    while(ADCSRA & (1<<ADSC)); 
    return ADC;
}