#include <avr/io.h>
#include <util/delay.h>
void initADC()
{
  /* this function initialises the ADC 

        ADC Prescaler Notes:
	--------------------

	   ADC Prescaler needs to be set so that the ADC input frequency is between 50 - 200kHz.
  
           For more information, see table 17.5 "ADC Prescaler Selections" in 
           chapter 17.13.2 "ADCSRA – ADC Control and Status Register A"
          (pages 140 and 141 on the complete ATtiny25/45/85 datasheet, Rev. 2586M–AVR–07/10)

           Valid prescaler values for various clock speeds
	
	     Clock   Available prescaler values
           ---------------------------------------
             1 MHz   8 (125kHz), 16 (62.5kHz)
             4 MHz   32 (125kHz), 64 (62.5kHz)
             8 MHz   64 (125kHz), 128 (62.5kHz)
            16 MHz   128 (125kHz)

           Below example set prescaler to 128 for mcu running at 8MHz
           (check the datasheet for the proper bit values to set the prescaler)
  */
  //Guuddygit
  // 8-bit resolution
  // set ADLAR to 1 to enable the Left-shift result (only bits ADC9..ADC2 are available)
  // then, only reading ADCH is sufficient for 8-bit results (256 values)
  
  ADMUX =
            (1 << ADLAR) |     // left shift result
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (1 << MUX1)  |     // use ADC2 for input (PB4), MUX bit 1
            (0 << MUX0);       // use ADC2 for input (PB4), MUX bit 0

  ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (1 << ADPS2) |     // set prescaler to 64, bit 2 
            (1 << ADPS1) |     // set prescaler to 64, bit 1 
            (0 << ADPS0);      // set prescaler to 64, bit 0  
}

int main(void) 
{
DDRB = 0b00001111; //Set Data Direction Register so PB 0,1,2,3 are outputs while  Pb 4 is an Input
  
  initADC();

  while(1)
  {
 // take 100x 8-bit samples and calculate a rolling average of the last 15 samples
  
 float voltage_fl;        // real battery voltage (0-5V) with decimals
 float adc_step=0.01953;  // (0-5V over 256 values)
 int sample_loop;

 for (sample_loop=100; sample_loop > 0 ; sample_loop --)
 {

   ADCSRA |= (1 << ADSC);         // start ADC measurement
   while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 

   voltage_fl = voltage_fl + (((ADCH * adc_step) - voltage_fl) / 15);  // integrated last 15-sample rolling average

 }

 
    if (voltage_fl > 3.9) // Close to Full at 1C load; Topmost Segment
    {
      
			PORTB &= ~(1 << PB1); // PB1 goes low 

    } else {
     
			PORTB |= (1 << PB1); // PB1 goes high  

    }
     if (voltage_fl > 3.7) // Middle Segment
    {
       
			PORTB &= ~(1 << PB2); // PB2 goes low

    }
     else 
     {
			PORTB |= (1 << PB2); // PB2 goes high 
     }
     
     
     if (voltage_fl > 3.5) // Bottomost Segment
     { 
			PORTB |= (1 << PB3); // PB3 goes high
     }
     else
     {		
			PORTB &= ~(1 << PB3); // PB3 goes low
     }
          
     if (voltage_fl < 3.3) // Blink
     {
			PORTB &= ~(1 << PB3); // PB3 goes low
			_delay_ms(50);
			PORTB |= (1 << PB3); // PB3 goes high
			_delay_ms(50);
      }
      else 
      {
			PORTB &= ~(1 << PB3); // PB3 goes low
	  }
     
    //Fin
   
  }

  return 0;
}
