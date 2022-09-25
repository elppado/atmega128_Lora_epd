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

#define PI 3.14159265
#define radius 6371
#define ConvertDtoR(deg) (deg*PI/180)

#define GPS_RAW_GPGGL 0;
#define GPS_RAW_lati 1;
#define GPS_RAW_ns 2;
#define GPS_RAW_logi 3;
#define GPS_RAW_ew 4;
#define GPS_RAW_time 5;
#define GPS_RAW_valid 6;
#define GPS_RAW_fix 7;

#define GPS_off 0
#define GPS_on 1

#define GPS_con 0
#define Time_con 1


#define TX_x 23
#define TX_y 20
#define RX_x 75
#define RX_y 40

#define DC1 0x11//GPS call
#define DC2 0x12//GPS lati
#define DC3 0x13//GPS logi
#define DC4 0x14//text str

extern void str_to_epd(const unsigned char *str,unsigned char x, unsigned char y);
extern void word_UART1(const unsigned char data);
extern void str_UART1(const unsigned char *str);

char test[] = "$GPGLL,3720.51002,N,12643.96449,E,111957.00,A,A*67";


unsigned char GPS_GPGLL[] = "$GPGLL";
unsigned char GPS_i = 0;
unsigned char GPGGL_flag = 0;
unsigned char GPS_buf[50] = "    ";


typedef struct{
    
    int lati_h;
    float lati_l;
    int logi_h;
    float logi_l;

    unsigned char n; //GPS 파싱용
    unsigned char e; //GPS 파싱용
    
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


char GPS_raw[50] = "      ";

unsigned char GPS_error[] = "GPS not conneted!";

double lati_real = 0;
double logi_real = 0;

unsigned char GPS_lati[15] = "";
unsigned char GPS_logi[15] = "";

unsigned char you_GPS_lati[15] = "";
unsigned char you_GPS_logi[15] = "";



/*
 
UART0 입력 예시
 
 $GPRMC,111957.00,A,3720.51002,N,12643.96449,E,0.776,11.78,270822,,,A*54
 $GPVTG,11.78,T,,M,0.776,N,1.438,K,A*0A
 $GPGGA,111957.00,3720.51002,N,12643.96449,E,1,05,2.98,61.3,M,18.1,M,,*68
 $GPGSA,A,3,25,32,01,24,12,,,,,,,,3.50,2.98,1.84*08
 $GPGSV,3,1,12,01,02,312,14,10,49,203,18,12,40,053,39,21,01,291,*79
 $GPGSV,3,2,12,22,44,313,24,23,24,177,,24,13,067,35,25,69,107,34*70
 $GPGSV,3,3,12,26,00,211,,29,05,138,,31,33,264,14,32,70,338,21*72
 $GPGLL,3720.51002,N,12643.96449,E,111957.00,A,A*67
 
 */

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
    
    GPS_raw[GPS_i] = ch;
    GPS_i++;
    
    if (GPS_i < 6) {
        if(GPS_raw[GPS_i-1] != GPS_GPGLL[GPS_i-1]){
            GPS_i = 0;
        }
    }
    
    if ((ch == CR) & (GPS_i > 10)) {
        GPS_raw[GPS_i] = '\0';
        GPS_i = 0;
        onoff_UART0(GPS_off);//인터럽트 비활성화
    }
}

/*
ISR(USART0_RX_vect){ //  UART RX 인터럽트
    
    unsigned char ch = UDR0;
    GPS_raw[GPS_i] = ch;
    GPS_i++;
    
    if (ch == CR) {
        GPS_raw[GPS_i] = '\0';
        str_to_epd(GPS_raw,TX_x,TX_y);
        onoff_UART0(GPS_off);//인터럽트 비활성화
    }
}
 */


void and_you(void){
    
    word_UART1(DC1);
    word_UART1(CR);
    word_UART1(LF);
    
}


void GPS_data_logiloti_con(void){
    
    lati_real = GPS_data.lati_h + GPS_data.lati_l/60;
    sprintf(GPS_lati, "%f", lati_real);

    logi_real = GPS_data.logi_h + GPS_data.logi_l/60;
    sprintf(GPS_logi, "%f", logi_real);
    
}


void GPS_data_Time_con(void){
    
    char tmp_str[4] = "";
    
    Time_data.Time_hour += 9;
    Time_data.Time_hour %= 24;
    
    sprintf(tmp_str, "%2d",Time_data.Time_hour);
    
    Time_data.GPS_time[0] = tmp_str[0];
    Time_data.GPS_time[1] = tmp_str[1];
    
    sprintf(tmp_str, "%2d",Time_data.Time_min);
    Time_data.GPS_time[3] = tmp_str[0];
    Time_data.GPS_time[4] = tmp_str[1];
    
    sprintf(tmp_str, "%2d",Time_data.Time_sec);
    Time_data.GPS_time[6] = tmp_str[0];
    Time_data.GPS_time[7] = tmp_str[1];
    
}



double LatLonDis(const double Lat1, const double Lon1, const double Lat2,  const double Lon2){
    
    double dLat = ConvertDtoR((Lat1 - Lat2));
    double dlon = ConvertDtoR((Lon1 - Lon2));
    double Lat1_R = ConvertDtoR(Lat1);
    double Lat2_R = ConvertDtoR(Lat2);
    
    double distance = 2 * radius * asin(sqrt(pow((sin(dLat/2)),2) + cos(Lat1_R)*cos(Lat2_R)*pow((sin(dlon/2)),2)));
    
    return distance;
}



unsigned char GPS_get_data(unsigned char flag){
    unsigned char text = 0;
    
    onoff_UART0(GPS_on);
    
    _delay_ms(5000);
    
    
    
    unsigned char tmp = sscanf(test,"$GPGLL,%2d%f,%*c,%3d%f,%*c,%2d%2d%2d.00,%s,%*s",&GPS_data.lati_h,&GPS_data.lati_l,&GPS_data.logi_h,&GPS_data.logi_l,&Time_data.Time_hour,&Time_data.Time_min,&Time_data.Time_sec,&GPS_data.Valid);
    
    
    while (tmp) {
        SSound(Si);
        tmp--;
    }
    
    
    unsigned char test2[2];
    test2[0] =GPS_data.Valid;
    test2[1] = '\0';
    
    str_to_epd(test2,TX_x,TX_y);
    
    
    if (GPS_data.Valid != 'A') {
        
        str_to_epd(GPS_raw,RX_x,RX_y);
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
    str_to_epd(GPS_lati,RX_x,RX_y+5);
    str_to_epd(logi,RX_x,RX_y+8);
    str_to_epd(GPS_logi,RX_x,RX_y+13);
}


void GPS_return(void){
    
    onoff_UART0(GPS_on);
    
    sscanf(GPS_raw,"$GPGLL,%2d%f,%*c,%3d%f,%*c,%2d%2d%2d.00,%hhu,%*s",&GPS_data.lati_h,&GPS_data.lati_l,&GPS_data.logi_h,&GPS_data.logi_l,&Time_data.Time_hour,&Time_data.Time_min,&Time_data.Time_sec,&GPS_data.Valid);
    
    if (GPS_data.Valid != 'A') {
        
        str_UART1(GPS_error);
        return;
    }
    
    GPS_data_logiloti_con();
    
    word_UART1(DC1); // lati 알림 플래그
    str_UART1(GPS_lati);
    word_UART1(DC2); // logi 알림 플래그
    str_UART1(GPS_logi);
}


void get_time(void){
    
    GPS_get_data(Time_con);
    str_to_epd(Time_data.GPS_time,RX_x,RX_y);
}

void GPS_dis(void){
    
    double you_lati_real = 0;
    double you_logi_real = 0;
    
    double tmp = 0;
    unsigned char tmp_dis_str[10] = "";
    
    GPS_get_data(GPS_con);
    
    and_you();
    _delay_ms(100);
    sscanf(you_GPS_lati,"%lf",&you_lati_real);
    sscanf(you_GPS_logi,"%lf",&you_logi_real);
    
    tmp = LatLonDis(lati_real, logi_real, you_lati_real, you_logi_real);
    sprintf(tmp_dis_str, "%lf", tmp);
    
    str_to_epd(tmp_dis_str,RX_x,RX_y);
}




#endif /* GPS_h */
