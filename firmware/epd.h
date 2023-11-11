/*
 * epd.h
 *
 * Created: 2022-07-17 오후 1:14:07
 *  Author: chaedoyun
 */ 


#ifndef EPD_H_
#define EPD_H_


#define SCK_Pin PB1
#define SDI_Pin PB2
#define CS_Pin PB4
#define DC_Pin PB5
#define RES_Pin PB6
#define BUSY PB7


#define EPD_W21_MOSI_0	PORTB &= ~( 1<< SDI_Pin)
#define EPD_W21_MOSI_1	PORTB |= ( 1<< SDI_Pin)

#define EPD_W21_CLK_0	PORTB &= ~( 1<< SCK_Pin)
#define EPD_W21_CLK_1	PORTB |= ( 1<< SCK_Pin)

#define EPD_W21_CS_0	PORTB &= ~( 1<< CS_Pin)
#define EPD_W21_CS_1	PORTB |= ( 1<< CS_Pin)

#define EPD_W21_DC_0	PORTB &= ~( 1<< DC_Pin)
#define EPD_W21_DC_1	PORTB |= ( 1<< DC_Pin)

#define EPD_W21_RST_0	PORTB &= ~( 1<< RES_Pin)
#define EPD_W21_RST_1	PORTB |= ( 1<< RES_Pin)


//250*122///////////////////////////////////////


#define MAX_LINE_BYTES 16// =128/8
#define MAX_COLUMN_BYTES  250

#define ALLSCREEN_GRAGHBYTES  4000





////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay(unsigned long xms);
void SPI_Delay(unsigned char xrate);


//EPD
void Epaper_READBUSY(void);
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char datas);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Part_Init(void); //Local refresh initialization

void EPD_Part_Update(void);
void EPD_Update(void);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display
void EPD_WhiteScreen_ALL(const unsigned char * datas);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);





void driver_delay_us(unsigned int xus)  //1us
{
	for(;xus>1;xus--);
}//us delay


void driver_delay(unsigned long xms) //1ms
{
	unsigned long i = 0 , j=0;

	for(j=0;j<xms;j++)
	{
		for(i=0; i<256; i++);
	}
}



void EPD_WhiteScreen_ALL(const unsigned char * datas)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();
}




//////////////////////SPI///////////////////////////////////
void SPI_Delay(unsigned char xrate)
{
	unsigned char i;
	while(xrate)
	{
		for(i=0;i<2;i++);
		xrate--;
		
	}
}


void SPI_Write(unsigned char value)
{
	unsigned char i;
	SPI_Delay(1);
	for(i=0; i<8; i++)
	{
		EPD_W21_CLK_0;
		SPI_Delay(1);
		if(value & 0x80)
		EPD_W21_MOSI_1;
		else
		EPD_W21_MOSI_0;
		value = (value << 1);
		SPI_Delay(1);
		driver_delay_us(1);
		EPD_W21_CLK_1;
		SPI_Delay(1);
	}
}


void Epaper_Write_Command(unsigned char cmd)
{
	SPI_Delay(1);
	EPD_W21_CS_0;
	EPD_W21_DC_0;   // command write
	SPI_Write(cmd);
	EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char datas)
{
	SPI_Delay(1);
	EPD_W21_CS_0;
	EPD_W21_DC_1;   // data write
	SPI_Write(datas);
	EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1680
void EPD_HW_Init(void){
    
    EPD_W21_RST_0;  // Module reset
    _delay_ms(10); //At least 10ms delay
    EPD_W21_RST_1;
    _delay_ms(10); //At least 10ms delay

    Epaper_READBUSY();
    Epaper_Write_Command(0x12);  //SWRESET
    Epaper_READBUSY();
       
     Epaper_Write_Command(0x01); //Driver output control
     Epaper_Write_Data(0xF9);
     Epaper_Write_Data(0x00);
     Epaper_Write_Data(0x00);

     Epaper_Write_Command(0x11); //data entry mode
     Epaper_Write_Data(0x01);

     Epaper_Write_Command(0x44); //set Ram-X address start/end position
     Epaper_Write_Data(0x00);
     Epaper_Write_Data(0x0F);    //0x0F-->(15+1)*8=128

     Epaper_Write_Command(0x45); //set Ram-Y address start/end position
     Epaper_Write_Data(0xF9);   //0xF9-->(249+1)=250
     Epaper_Write_Data(0x00);
     Epaper_Write_Data(0x00);
     Epaper_Write_Data(0x00);

     Epaper_Write_Command(0x3C); //BorderWavefrom
     Epaper_Write_Data(0x05);
         
     Epaper_Write_Command(0x21); //  Display update control
     Epaper_Write_Data(0x00);
     Epaper_Write_Data(0x80);
         
     Epaper_Write_Command(0x18); //Read built-in temperature sensor
     Epaper_Write_Data(0x80);

     Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
     Epaper_Write_Data(0x00);
     Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
     Epaper_Write_Data(0xF9);
     Epaper_Write_Data(0x00);
     Epaper_READBUSY();
}

/*
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char * datas)
{
	unsigned int i;
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
	{
		Epaper_Write_Data(pgm_read_byte(&datas[i]));
	}
	EPD_Update();
}
*/
/////////////////////////////////////////////////////////////////////////////////////////

void EPD_Update(void)
{
	Epaper_Write_Command(0x22); //Display Update Control
	Epaper_Write_Data(0xF7);
	Epaper_Write_Command(0x20); //Activate Display Update Sequence
	Epaper_READBUSY();

}
void EPD_Part_Update(void)
{
	Epaper_Write_Command(0x22); //Display Update Control
	Epaper_Write_Data(0xFF);
	Epaper_Write_Command(0x20); //Activate Display Update Sequence
	Epaper_READBUSY();
}
void EPD_DeepSleep(void)
{
	Epaper_Write_Command(0x10); //enter deep sleep
	Epaper_Write_Data(0x01);
    //Epaper_READBUSY();
    _delay_ms(100);
}
void Epaper_READBUSY(void)
{
    while(PINB & 0x80);

}
///////////////////////////Part update//////////////////////////////////////////////
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
	unsigned int i;
	const unsigned char  *datas_flag;
	datas_flag=datas;

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM

	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        
		Epaper_Write_Data(pgm_read_byte(&datas[i]));
	}

	datas=datas_flag;

	Epaper_Write_Command(0x26);   //Write Black and White image to RAM

	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
	{
		Epaper_Write_Data(pgm_read_byte(&datas[i]));
	}

	EPD_Update();
}

void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
    unsigned int i;
      unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
      x_start=x_start/8;//
      x_end=x_start+PART_LINE/8-1;
      
      y_start1=0;
      y_start2=y_start;
      if(y_start>=256)
      {
        y_start1=y_start2/256;
        y_start2=y_start2%256;
      }
      y_end1=0;
      y_end2=y_start+PART_COLUMN-1;
      if(y_end2>=256)
      {
        y_end1=y_end2/256;
        y_end2=y_end2%256;
      }
      //Reset
      EPD_W21_RST_0;  // Module reset
      _delay_ms(10);//At least 10ms delay
      EPD_W21_RST_1;
      _delay_ms(10); //At least 10ms delay
        
      Epaper_Write_Command(0x3C); //BorderWavefrom
      Epaper_Write_Data(0x80);
 
      Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
      Epaper_Write_Data(x_start);    // RAM x address start at 00h;
      Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
      Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
      Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
      Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
      Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
      Epaper_Write_Data(y_end1);    // ????=0


      Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
      Epaper_Write_Data(x_start);
      Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
      Epaper_Write_Data(y_start2);
      Epaper_Write_Data(y_start1);
      
      
       Epaper_Write_Command(0x24);   //Write Black and White image to RAM
       for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
       {
         Epaper_Write_Data(pgm_read_byte(&datas[i]));
       }
       EPD_Part_Update();


}




/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_White(void)

{
	unsigned int i,k;
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(k=0;k<250;k++)
	{
		for(i=0;i<16;i++)
		{
			Epaper_Write_Data(0xff);
		}
	}
	EPD_Update();
}


#endif /* EPD_H_ */
