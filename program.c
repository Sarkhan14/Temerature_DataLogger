/**	
	@author Thomas Grunenberg
	@author Sarkhan Orujlu
	@version 0.1
	@file main.c
	@brief Main programm for temperature data logger
*/


#define F_CPU 8000000UL


#define TEMPSENOR_OFFSET 1356 //500 1356


#define DS1307_I2C_ADR 0xD0 



//
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c_master.h"
#include "init.h"
#include "lcd.h"

//


//
/* GLOBAL MEMORY                                                              */
//
char* dayofweek[8] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Err"};

// Global Time memory
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint8_t year;
uint8_t weekday;




//



//
/* FUNCTIONS                                                                  */
//

/**
	@brief Convert from BCD to Binary
	@param in BCD input (00-99)
	@return Binary output
*/
uint8_t ds1307_decodeBcd(uint8_t in){
	return(((in>>4)*10)+(in&0xF));
}
//

/**
	@brief Convert from Binary to BCD
	@param in Binary input (0-99)
	@return BCD output
*/
uint8_t ds1307_encodeBcd(uint8_t in){
	return ((in / 10) << 4 ) | (in % 10); 
}
//


/**
	@brief Show time/date with the LCD
*/
void display_standby(uint16_t t){
	char str[16];
	
	// Time and Year
	snprintf(str, 16, "%02d:%02d:%02d  20%02d", hour, minute,
			second, year);
	
	lcd_clear();
	lcd_string(str);
	
	
	// Date and Temperature
	snprintf(str, 16, "%02d.%02d  %d.%d C", day, month, t/10, t%10);
	
	lcd_setcursor(0,2);
	lcd_string(str);

	return;
}
//

/**
	@brief Write a row byte to the DS1307
	@param adr address to write
	@param data byte to write
*/
void ds1307_write(uint8_t adr, uint8_t data){
	
	if (i2c_master_open_write(DS1307_I2C_ADR))
		return;
	
	i2c_master_write(adr);
	i2c_master_write(data);
	
	i2c_master_close();
}
//

/**
	@brief Read a row byte from the DS1307
	@param adr address to read
	@return the received byte
*/
uint8_t ds1307_read(uint8_t adr){
	uint8_t ret;

	if (i2c_master_open_write(DS1307_I2C_ADR))
		return 0;
	
	i2c_master_write(adr);
	i2c_master_open_read(DS1307_I2C_ADR);
	ret = i2c_master_read_last();
	
	i2c_master_close();

	return ret;

}
//

/**
	@brief Start or freeze the clock of the DS1307
	@param run zero for stop, all other for run
*/
void ds1307_rtc(uint8_t run){
	
	uint8_t readout;
	
	// Read current value
	readout = ds1307_read(0x00);
	
	
	// Set CH bit
	if (run)
		readout &= ~(0x80);
	else
		readout |= 0x80;
		
	// Write value back
	ds1307_write(0x00, readout);
}
//

/*
	@brief Write the current time to the DS1307
	@return zero for no error, one for communication error
*/
uint8_t ds1307_setTime(void){
	uint8_t chbit = ds1307_read(0x00) & 0x80;
second=8;
minute=00;
hour=8;
day=1;
month=4;
year=21;
weekday=7;
	// Open device for write
	if (i2c_master_open_write(DS1307_I2C_ADR))
		return 1;

	i2c_master_write(0x00);
	if (chbit)
		i2c_master_write(ds1307_encodeBcd(second) | 0x80);
	else
		i2c_master_write(ds1307_encodeBcd(second) & 0x7F);		
	
	i2c_master_write(ds1307_encodeBcd(minute));
	i2c_master_write(ds1307_encodeBcd(hour));
	
	i2c_master_write(weekday);		
	
	i2c_master_write(ds1307_encodeBcd(day));
	i2c_master_write(ds1307_encodeBcd(month));
	i2c_master_write(ds1307_encodeBcd(year));		
	
	
	// Close I2C bus
	i2c_master_close();
	
	return 0;
}
//*/

/**
	@brief Get the current time from the DS1307
	@return zero for no error, one for communication error
*/
uint8_t ds1307_getTime(void){

	// Open device for write
	if (i2c_master_open_write(DS1307_I2C_ADR))
		return 1;
	
	// select reading position (0x00)
	i2c_master_write(0x00);
	
	// (Re-)Open device for read
	i2c_master_open_read(DS1307_I2C_ADR);
	
	// Read value
	second = ds1307_decodeBcd(i2c_master_read_next() & 0x7F);
	// TODO minute, hour, ...
	minute = ds1307_decodeBcd(i2c_master_read_next());
	hour = ds1307_decodeBcd(i2c_master_read_next());
	weekday = i2c_master_read_next();
	day = ds1307_decodeBcd(i2c_master_read_next());
	month = ds1307_decodeBcd(i2c_master_read_next());
	year = ds1307_decodeBcd(i2c_master_read_last());
	
	
	// Close I2C bus
	i2c_master_close();
	
	return 0;
}
//


void nexttime(void){
	uint8_t days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

		second = 0;
	
	// Goto next minute
	minute++;
	if (minute >= 60){
		minute = 0;
		hour++;
	} else {
		return;
	}
	
	// Check hour
	if (hour >= 24){
		hour = 0;
		day++;
	} else {
		return;
	}
if (((year%4==0)||(year%400==0))&&(year%100!=0)){
	// TODO 
	days[1] = 29;
	} else {
        return;
	}
	
	// Check day
	if (day > days[month]){
		day= 1;
		month++;
	}
	
	// Check month
	if (month >= 12){
		month = 1;
		year++;
	}
}



/**
	@brief Load 8 bit value from the EEPROM
	@return loaded value
*/

uint8_t load_value8bit(uint8_t pos){
	uint8_t value;

	/* TODO */
	i2c_master_open_write(0xA0) ;
	 i2c_master_write(pos) ;
	 i2c_master_open_read(0xA1) ;
	 value=i2c_master_read_last() ;
	i2c_master_close();		
	return value;
}
/******************************************************************************/


/**

*/

uint16_t load_value16bit(uint8_t pos){
	uint8_t highbyte, lowbyte;

	/* TODO */
	i2c_master_open_write(0xA0) ;
	 i2c_master_write(pos) ;
	 i2c_master_open_read(0xA1) ;
	 lowbyte = i2c_master_read_next() ;
	 highbyte = i2c_master_read_last() ;
	i2c_master_close();		
	return highbyte * 256 + lowbyte;
}
//

/**
	@brief Save a 8 bit value to the EEPROM
	@param tosave value to save
*/

void save_value8bit(uint8_t tosave, uint8_t pos){

	/* TODO */
	
	
	 i2c_master_open_write(0xA0) ;
	 i2c_master_write(pos) ;
	 i2c_master_write(tosave) ;
	 i2c_master_close();
	_delay_ms(10); // wait 10ms to make sure that data is written
}
//


/**
	@brief Save a 16 bit value to the EEPROM
	@param tosave value to save
*/
void save_value16bit(uint16_t tosave, uint8_t pos){
	uint8_t highbyte, lowbyte;
	highbyte = tosave/256 ;
	lowbyte = tosave%256  ;
	
	 i2c_master_open_write(0xA0) ;
	 i2c_master_write(pos) ;
	 i2c_master_write(lowbyte) ;
	 i2c_master_write(highbyte) ;

	

	i2c_master_close();

	_delay_ms(10); // wait 10ms to make sure that data is written	
}
//


/**
	@brief Read the temperature with the internal analog sensor
	@return temperature in 1/10 deg. Celsius
*/
uint16_t adc_temperature_oversample(void){
	uint8_t i;
	uint32_t sum = 0;
	
	for (i = 0; i < 128; i++){
		ADCSRA |= (1 << ADSC)| (1 << ADEN); // Start ADC

	
		while( ADCSRA & (1<<ADSC)); 
		
		//TODO // wait for ADC complete DONE
	
		sum += ADCW;
	}
	

	sum /= 32;

	// substract offset
	sum -= TEMPSENOR_OFFSET;
	
	// TODO: Calculate value in 1/10 deg. Celsius DONE
	sum=sum*0.269;
	return sum;
}
//


void log_data(void){
	char str[17];
uint8_t flag = 0;
uint8_t count=6;	

		while (second !=0){
			_delay_ms(100);
			ds1307_getTime();
			display_standby(adc_temperature_oversample());
		}
		save_value8bit(year, 1);
		save_value8bit(month, 2);
		save_value8bit(day,3);
		save_value8bit(hour, 4);
		save_value8bit(minute, 5);
		flag = 1;
		
while (flag){
	_delay_ms(100);
	ds1307_getTime();
	display_standby(adc_temperature_oversample());
			
			
if (second==00){
		save_value8bit((count-6)/2+1, 0);
		save_value16bit(adc_temperature_oversample(), count);
		count=count+2;
		lcd_clear();
		lcd_setcursor(0,1);
		lcd_string("Recording");
			
		snprintf(str, 17, "T: %d.%d V:%d", (load_value16bit(count-2))/10, (load_value16bit(count-2))%10 , load_value8bit(0));
		lcd_setcursor(0,2);
		lcd_string(str);
		_delay_ms(1500);
}
		if (count>=255){
			lcd_clear();
			lcd_setcursor(0,1);
			lcd_string("The memory is full");
		return;
		}
	if ((~PINB & (1 << PB1))&&(~PINB & (1 << PB0))){
		lcd_clear();
	lcd_setcursor(0,1);
	lcd_string("Exiting log");
	_delay_ms(1500);
	
	return;}
}
}
		

	// TODO
//


void show_data(void){
	char str[17];
	year=load_value8bit(1);
	month=load_value8bit(2);
	day=load_value8bit(3);
	hour=load_value8bit(4);
	minute=load_value8bit(5);
	
uint16_t value; 
uint8_t s=1,flag1=1;
uint8_t flag=1;
uint16_t i;
uint8_t count=6;
uint8_t flag3;
	while(flag)
	{
		if (load_value8bit(0)==0)
		{
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("The memory is");
			lcd_setcursor(0, 2);
			lcd_string("empty");
			_delay_ms(1500);
			
			return;
		}
		else 
		{			
					lcd_clear();
                    snprintf(str, 17, "Value num: %2d", load_value8bit(0));
                    lcd_setcursor(0, 2);
                    lcd_string(str);
                    _delay_ms(1500);
			while (flag1)
		{
			value= load_value16bit(count);
		
			lcd_clear();
			lcd_setcursor(0,1);
			snprintf(str, 17, "%d.%d.%d %d:%d", day, month, year, hour,minute);
			lcd_string(str);
			lcd_setcursor(0, 2);
			snprintf(str, 17, "T:%d %d.%d C", s, load_value16bit(count)/10,load_value16bit(count)%10);
			lcd_string(str);
			_delay_ms(50);
		if (~PINB & (1<<PB0))
		{
			_delay_ms(50);
			s++;
			nexttime();
			count=count+2;
		
		}
		while ((~PINB & (1 << PB0))||(~PINB & (1<< PB1))){
				_delay_ms(250);
                if ((~PINB & (1 << PB0)) && (~PINB & (1 << PB1))){
					lcd_clear();
					lcd_string("Memory reset");
					_delay_ms(1500);
                  for(i=0;i<=255;i++)
					{
					  save_value8bit(0,i);
					}
					
						flag=0;
						flag1=0;
						lcd_clear();
						lcd_setcursor(0, 1);
						lcd_string("Exiting");
						lcd_setcursor(0, 2);
						lcd_string("Read mode");
						
						_delay_ms(1500);
						break;
					
					}
				
                else{
                    ;
                }
            }
	if((count-6)/2>=load_value8bit(0)&& flag)
		{
		flag=0;
		flag1=0;
		flag3=0;
		lcd_clear();
		lcd_string("Number of values");
		lcd_setcursor(0,2);
		lcd_string("exceeded");
		_delay_ms(1000);
						lcd_clear();
						lcd_setcursor(0, 1);
						lcd_string("Exiting");
						lcd_setcursor(0, 2);
						lcd_string("Read mode");
		_delay_ms(1500);
		
		}
		
		
	
	
	}
		}
	}
}
//}


/**
	@brief Main function
	@return only a dummy to avoid a compiler warning, not used
*/
int main(void){
	uint16_t nowtemp;
	char str[17];
	init(); 	// Function to initialise I/Os
	lcd_init(); // Function to initialise LCD display
	i2c_master_init(1, 10); // Init TWI
	uint16_t i;
	
	// Analog Input
	ADMUX = 0;//TODO // 1.1V as reference
	ADCSRA = (1 << ADPS2)| (1 << ADPS1); // ADC Prescale by 64
	//ADCSRA |= (1 << ADSC)| (1 << ADEN); // Start first conversion (dummy read)
	ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << MUX3);
	ds1307_setTime();

	// Loop forever
	for(;;){
		uint8_t flag3=1;
		// Short delay
		_delay_ms(100);
		
		
		// Mesure temperature
		nowtemp = adc_temperature_oversample();
		
		
		// Load current time/date from DS1307
		ds1307_getTime();
		
		// Show current time/date
		display_standby(nowtemp);

		// Show recorded data
		if ( ~PINB & (1 << PB0) ){
			_delay_ms(100);
			lcd_clear();
			lcd_string("Read mode");
			_delay_ms(1500);

			show_data();
			
		// Start Recording
		}
		if (~PINB & (1 << PB1)){
			
			_delay_ms(100);
			if (load_value8bit(0)==0)
			{
			lcd_clear();
			lcd_string("Log mode on");
			_delay_ms(1500);
			log_data();
			}
			else{	
				lcd_clear();
				lcd_setcursor(0,1);
				lcd_string("Already saved");
				
				lcd_setcursor(0,2);
				snprintf(str, 17, "values is: %d", load_value8bit(0));
				lcd_string(str);
				_delay_ms(1500);
				_delay_ms(1500);
				while(flag3){
					lcd_clear();
				lcd_setcursor(0,1);
				lcd_string("Reset or Read?");
				lcd_setcursor(0,2);
				lcd_string("With B1 and B2");
				_delay_ms(50);
				if (~PINB & (1<<PB0)){
					lcd_clear();
					lcd_string("Memory reset");
					_delay_ms(1500);
                  for(i=0;i<=255;i++)
					{
					  save_value8bit(0,i);
					}
					flag3=0;
				}
				else if ((~PINB & (1<<PB1))){
				show_data();
				flag3=0;
				}
			}
					
					
					
				
				
				
			}
		

	}
	}
	return 0;
}
