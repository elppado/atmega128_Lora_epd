//
//  GPS.h
//  epdtest
//
//  Created by 채도윤 on 2022/08/26.
//

#ifndef GPS_h
#define GPS_h

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265
#define radius 6371
#define ConvertDtoR(deg) ((deg)*PI/180)

#define GPS_off 0
#define GPS_on 1

#define GPS_con 1
#define Time_con 0


#define TX_x 23
#define TX_y 20
#define RX_x 75
#define RX_y 40

#define DC1 0x11//GPS call
#define DC2 0x12//GPS lati
#define DC3 0x13//GPS logi
//#define DC4 0x14//text str

extern void str_to_epd(const unsigned char *str,unsigned char x, unsigned char y);
extern void word_UART1(const unsigned char data);
extern void str_UART1(const unsigned char *str);

unsigned char GPS_GPGLL[] = "$GPGLL";
unsigned char GPS_i = 0;

float lati_real = 0;
float logi_real = 0;


typedef struct{
    
    int lati_h;
    long lati_l_int;
    float lati_l;
    int logi_h;
    long logi_l_int;
    float logi_l;
    unsigned char Valid; //GPS 파싱용
    
}GPS_data_str;

typedef struct {
    int Time_hour;
    int Time_min;
    int Time_sec;
    unsigned char GPS_time[10];
}GPS_time_str;


GPS_data_str GPS_data = {0,0,0,0,0,0,'v'};
GPS_time_str Time_data = {0,0,0,"99h99m99s"};


char GPS_raw[55] = " ";

unsigned char GPS_error[] = "GPS not connected!";
unsigned char no_response[] = "No response!";


unsigned char GPS_lati[15] = " ";
unsigned char GPS_logi[15] = " ";

unsigned char you_GPS_lati[15] = "";
unsigned char you_GPS_logi[15] = "";

unsigned char GPGLL_flag = 1;
unsigned char GPS_return_logi_flag = 1;
unsigned char GPS_return_lati_flag = 1;


void init_UART0(unsigned char baud){
    
    UBRR0H = (unsigned char)(baud >> 8);
    UBRR0L = (unsigned char)baud;
    
    UCSR0A &= ~(1 << U2X0);
    UCSR0B &= ~(1 << RXEN0) & ~( 1 << RXCIE0);
    
    UCSR0B &= ~(1 << UCSZ02);
    UCSR0C &= ~( 1 << UMSEL0) & ~( 1 << UPM01) & ~( 1 << UPM00) & ~( 1 << USBS0);
    UCSR0C |= (1<<UCSZ01) | (1 << UCSZ00);
    
}

void onoff_UART0(unsigned char flag){
    
    if (flag) {
        UCSR0B |= (1 << RXEN0) | ( 1 << RXCIE0);
    }
    else {
        UCSR0B &= ~(1 << RXEN0) & ~( 1 << RXCIE0);
    }
}



ISR(USART0_RX_vect){ //  UART RX 인터럽트
    char ch = UDR0;
    
    if ((ch != CR) || (ch != LF)) {
        GPS_raw[GPS_i] = ch;
        GPS_i++;
    }
    
    if (GPS_i < 6) {
        if(GPS_raw[GPS_i-1] != GPS_GPGLL[GPS_i-1]){
            GPS_i = 0;
        }
    }
    
    if ((ch == CR) & (GPS_i > 10)) {
        
        GPS_raw[GPS_i] = 0;
        GPS_i = 0;
        GPGLL_flag = 0;
        onoff_UART0(GPS_off);//인터럽트 비활성화
    }
}


void and_you(void){
    
    word_UART1(DC1);
    word_UART1(CR);
    word_UART1(LF);
    
}


void GPS_data_logiloti_con(void){
    
    int tmp  = 0;
    
    tmp = GPS_data.lati_h;
    GPS_data.lati_h /= 100;
    GPS_data.lati_l = ((float)GPS_data.lati_l_int/100000) + tmp % 100;;
    lati_real = GPS_data.lati_h + (GPS_data.lati_l/60.00000);
    dtostrf(lati_real,8,5,GPS_lati);
    

    tmp =GPS_data.logi_h;
    GPS_data.logi_h /= 100;
    GPS_data.logi_l = ((float)GPS_data.logi_l_int/100000) + (tmp % 100);
    logi_real = GPS_data.logi_h + (GPS_data.logi_l/60.00000);
    dtostrf(logi_real,9,5,GPS_logi);

}


void GPS_data_Time_con(void){
    
    unsigned char tmp_str[4] = "";
    
    Time_data.Time_hour += 9;
    Time_data.Time_hour %= 24;
    
    itoa(Time_data.Time_hour,tmp_str,10);
    Time_data.GPS_time[0] = tmp_str[0];
    Time_data.GPS_time[1] = tmp_str[1];
    
    itoa(Time_data.Time_min,tmp_str,10);
    Time_data.GPS_time[3] = tmp_str[0];
    Time_data.GPS_time[4] = tmp_str[1];
    
    itoa(Time_data.Time_sec,tmp_str,10);
    Time_data.GPS_time[6] = tmp_str[0];
    Time_data.GPS_time[7] = tmp_str[1];
    Time_data.GPS_time[8] = '\0';
    
}



double LatLonDis(const double Lat1, const double Lon1, const double Lat2,  const double Lon2){
    
    double dLat = ConvertDtoR((Lat1 - Lat2));
    double dlon = ConvertDtoR((Lon1 - Lon2));
    double Lat1_R = ConvertDtoR(Lat1);
    double Lat2_R = ConvertDtoR(Lat2);
    
    double distance = 2 * radius * asin(sqrt(pow((sin(dLat/2)),2) + cos(Lat1_R)*cos(Lat2_R)*pow((sin(dlon/2)),2)));
    
    return distance;
}



unsigned char GPS_get_data(const unsigned char flag){
    
    onoff_UART0(GPS_on);
    
    int cnt = 0;
    
    while (GPGLL_flag) {
        _delay_ms(10);
        cnt++;
        
        if (cnt >500) {
            break;
        }
    }
    GPGLL_flag = 1;
    

    sscanf(GPS_raw,"$GPGLL,%d.%ld,%*c,%d.%ld,%*c,%2d%2d%2d.00,%c,%*s",&GPS_data.lati_h,&GPS_data.lati_l_int,&GPS_data.logi_h,&GPS_data.logi_l_int,&Time_data.Time_hour,&Time_data.Time_min,&Time_data.Time_sec,&GPS_data.Valid);
    
    
    if (GPS_data.Valid != 'A') {
        
        str_to_epd(GPS_error,RX_x,RX_y);
        return GPS_off ;
    }
    
    if (flag) {
        GPS_data_logiloti_con();
    }
    else{
        GPS_data_Time_con();
    }
    
    return GPS_on;
}

void GPS(void){
    unsigned char lati[] = "lati:";
    unsigned char logi[] = "logi:";
    
    if (GPS_get_data(GPS_con) == GPS_off){
        return;
    }
    
    SSound(Do);
    
    str_to_epd(lati,RX_x,RX_y);
    str_to_epd(GPS_lati,RX_x,RX_y+5*9);
    str_to_epd(logi,TX_x,TX_y);
    str_to_epd(GPS_logi,TX_x,TX_y+5*9);
}


void GPS_return(void){
    int cnt = 0;
    
    
    onoff_UART0(GPS_on);
    
    while (GPGLL_flag) {
        _delay_ms(10);
        cnt++;
        
        if (cnt >500) {
            break;
        }
    }
    GPGLL_flag = 1;
    
    
    sscanf(GPS_raw,"$GPGLL,%d.%ld,%*c,%d.%ld,%*c,%2d%2d%2d.00,%c,%*s",&GPS_data.lati_h,&GPS_data.lati_l_int,&GPS_data.logi_h,&GPS_data.logi_l_int,&Time_data.Time_hour,&Time_data.Time_min,&Time_data.Time_sec,&GPS_data.Valid);
    
    
    if (GPS_data.Valid != 'A') {
        
        str_UART1(GPS_error);
        return;
    }
    
    GPS_data_logiloti_con();
    
    word_UART1(DC2); // lati 알림 플래그
    str_UART1(GPS_lati);
    _delay_ms(100);
    word_UART1(DC3); // logi 알림 플래그
    str_UART1(GPS_logi);
}


void get_time(void){
    
    if (GPS_get_data(Time_con) == GPS_off){
        str_to_epd(GPS_error,RX_x,RX_y);
        return;
    }
    
    SSound(Do);
    
    str_to_epd(Time_data.GPS_time,RX_x,RX_y);
}


void GPS_dis(void){
    SSound(Do);
    
    int you_lati_h = 0;
    long you_lati_l = 0;
    
    int you_logi_h = 0;
    long you_logi_l = 0;
    
    double you_lati_real = 0;
    double you_logi_real = 0;
    double tmp = 0;
    
    double you_lati_l_d = 0;
    double you_logi_l_d = 0;
    
    int cnt = 0;
    
    unsigned char tmp_dis_str[10] = " ";
    

    
    GPS_get_data(GPS_con);
    and_you();
    
    _delay_ms(1000);
    
    while (GPS_return_lati_flag && GPS_return_logi_flag) {
        _delay_ms(10);
        cnt ++;
        
        if (cnt >1000) {
            str_to_epd(no_response,RX_x,RX_y);
            return;
        }
    }
    
    GPS_return_lati_flag = 1;
    GPS_return_logi_flag = 1;

    
    sscanf(you_GPS_lati,"%d.%ld",&you_lati_h, &you_lati_l);
    sscanf(you_GPS_logi,"%d.%ld",&you_logi_h, &you_logi_l);

    you_lati_l_d = you_lati_l/100000.00000;
    you_logi_l_d = you_logi_l/100000.00000;
    

    you_lati_real = you_lati_h + you_lati_l_d;
    you_logi_real = you_logi_h + you_logi_l_d;
    
    
    tmp = LatLonDis(lati_real, logi_real, you_lati_real, you_logi_real);
    
    dtostrf(tmp,7,3,tmp_dis_str);
    tmp_dis_str[7] = 'k';
    tmp_dis_str[8] = 'm';
    tmp_dis_str[9] = '\0';
    
    str_to_epd(tmp_dis_str,RX_x,RX_y);
    
    SSound(Mi);
    SSound(Si);
}




#endif /* GPS_h */
