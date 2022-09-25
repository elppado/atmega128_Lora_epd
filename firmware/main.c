/*
 * ATMEGAEPDLORA v1.c
 *
 * Created: 2022-07-17 오후 1:12:52
 * Author : chaedoyun
 */


//#define dev_colour_BLACK
//#define dev_colour_WHITE


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "epd.h"
#include "buzz.h"
#include "keyscan.h"
#include "GPS.h"
//#include "uart.h"

#ifdef dev_colour_WHITE
    #include "font_W.h"

#else
    #include "font_B.h"
#endif



#define MAX_LEN 23



unsigned char TX_str[MAX_LEN];
unsigned char RX_str[MAX_LEN];
const unsigned char erase_str[MAX_LEN] = "                      ";


const unsigned char GPS_comm[] = "$gps";
const unsigned char GPS_dis_comm[]= "$dis";
const unsigned char Time_comm[] = "$time";


unsigned char str_po = 0;
unsigned char RX_po = 0;

unsigned char flag = 0;
unsigned char rx_flag = 1;




void str_to_epd(const unsigned char *str,unsigned char x, unsigned char y){
    
    for (unsigned char i = 0; *(str+i) != '\0'; i++) {
        
        EPD_HW_Init(); // deepsleep 이후에 HW reset으로 꺠우기
        EPD_Dis_Part(x,y+(i*9),epd_font[(*(str+i)-32)],8,16);
        EPD_DeepSleep();

    }
}


void rx_str_sum(unsigned char *str, const unsigned char val){
    
    for (int i = 0; i < MAX_LEN; i++){
        
        if (*(str+i) == '\0'){
            *(str+i) = val;
            *(str+i+1) = '\0';
            
            return;
        }
    }
}

void str_sum(unsigned char *str, const unsigned char val){
        
    *(str+flag) = val;
    *(str+flag+1) = '\0';
    
    flag++;
}


void init_UART1(unsigned char baud){
    
    UBRR1H = (unsigned char)(baud >> 8);
    UBRR1L = (unsigned char)baud;
    
    UCSR1A &= ~(1 << U2X1);
    UCSR1B |= (1 << TXEN1) | (1 << RXEN1) | ( 1 << RXCIE1);
    UCSR1B &= ~(1 << UCSZ12);
    UCSR1C &= ~( 1 << UMSEL1) & ~( 1 << UPM11) & ~( 1 << UPM10) & ~( 1 << USBS1);
    UCSR1C |= (1<<UCSZ11) | (1 << UCSZ10);
    
}


void word_UART1(const unsigned char data){
    
    while( !(UCSR1A & (1<<UDRE1)));
    UDR1 = data;
}


void str_UART1(const unsigned char *str){
    
    while (*str != '\0') {
        word_UART1(*str);
        str++;
    }
    word_UART1(CR);
    word_UART1(LF);
    
}



void comm_find(void){
    
    switch (RX_str[0]) {
            
        case DC1:
            GPS_return();
            break;
            
        case DC2:
            for (unsigned char i = 0; RX_str[i] != 0; i++)
                you_GPS_lati[i] = RX_str[i+1];
            GPS_return_logi_flag = 0;
            break;
            
        case DC3:
            for (unsigned char i = 0; RX_str[i] != 0; i++)
                you_GPS_logi[i] = RX_str[i+1];
            GPS_return_lati_flag = 0;
            break;
            
        default:
            
            SSound(Do);
            SSound(Re);
            str_to_epd(RX_str,RX_x,RX_y);
            break;
    }
    
    for (unsigned char i = 0; i < MAX_LEN; i++)
        RX_str[i] = '\0';
}



/* 수신 > 문자열 저장
 문자열 시작이 DC1|DC2|DC3 > GPS처리
 아니면 일반문자열로 처리 (화면 출력)
 */


ISR(USART1_RX_vect){ //  UART RX 인터럽트
    unsigned char ch = UDR1;
    
    if ((ch != CR) && (ch != LF)) {
        rx_str_sum(RX_str,ch);
    }
    
    else if (ch == LF){
        
        comm_find();
    }
}



void init_port(){
    
    DDRA = 0xff;
    DDRD |= (1 << PD7);
    DDRD &= ~(1 << PD1);
    PORTD |= (1 << PD1);
    DDRB = 0x7f;
    DDRC = 0x00;
}


void init_BOOT(){
    interrupt_init();
    init_port();
    PORTC = 0xff;
    init_UART1(47); // baud rate 47, 9600bps, fosc = 7.3728 MHz
    init_UART0(47);
    
    SSound(Do);
    
    EPD_HW_Init();
    EPD_SetRAMValue_BaseMap(gImage_1);
    EPD_DeepSleep();

    SSound(Mi);
    SSound(Sol);
    // 부팅 완료;
}


int main(void){
    unsigned char kval = '\0';
    
    init_BOOT();
    
    while (1) {
        if (keyScan()) {
            
            kval = kbdq10[alt_flag][keypad_in()];
            
            if (kval != '\0'){ // 리턴값이 \0 (사용하지 않는 키패드) 이면서
                EPD_HW_Init();
                
                
                switch (kval) {
                        
                    case CR:
                        
                        
                        if (TX_str[0] == '$') {
                            
                            if ((TX_str[1] == GPS_comm[1]) && (TX_str[2] == GPS_comm[2]) && (TX_str[3] == GPS_comm[3]))  {
                                EPD_SetRAMValue_BaseMap(gImage_1);
                                EPD_DeepSleep();
                                GPS();
                                
                                
                            }
                            else if ((TX_str[1] == GPS_dis_comm[1]) && (TX_str[2] == GPS_dis_comm[2]) && (TX_str[3] == GPS_dis_comm[3])){
                                EPD_SetRAMValue_BaseMap(gImage_1);
                                EPD_DeepSleep();
                                GPS_dis();
                            }
                            
                            else if ((TX_str[1] == Time_comm[1]) && (TX_str[2] == Time_comm[2]) && (TX_str[3] == Time_comm[3]) && (TX_str[4] == Time_comm[4])){
                                EPD_SetRAMValue_BaseMap(gImage_1);
                                EPD_DeepSleep();
                                get_time();
                            }
                        }
                        
                        else{
                            //word_UART1(DC4);
                            str_UART1(TX_str); //UART 송신
                            SSound(Re);//송신 완료 알림
                            SSound(Do);
                            
                            _delay_ms(3000);
                            EPD_SetRAMValue_BaseMap(gImage_1);
                            EPD_DeepSleep();
                        }
                        
                        for (unsigned char i = 0; i < MAX_LEN; i++){
                            TX_str[i] = '\0'; // TX, 디스플레이 후 문자열 버퍼 초기화
                        }
                        str_po = 0;
                        flag = 0;
                        break;
                    
                    case BS:
                        str_po--; //커서--
                        
                        if (str_po < 0)
                            str_po = 0;
                        
        
                        flag--; //문자열 합치기--
                        
                        if (flag < 0)
                            flag = 0;
                        
                        
                        EPD_HW_Init();
                        EPD_Dis_Part(TX_x,TX_y+(str_po*9),epd_font[0],8,16);
                        EPD_DeepSleep();
                        break;
                        
                    case SPB:
                        str_sum(TX_str,kval); // 문자열 뒤에 추가.
                        str_po++;// 커서 위치
                        break;
                        
                    default:
                        str_sum(TX_str,kval); // 문자열 뒤에 추가.
                        EPD_Dis_Part(TX_x,TX_y+(str_po*9),epd_font[(kval-32)],8,16); // 키보드 입력한 한글자만 화면 출력
                        //word_UART1(kval);
                        EPD_DeepSleep();
                        str_po++; // 커서 위치
                        break;
                }
            }
        }
    }
}

// 커서구현 타이머 카운터로 3s마다 커서 표시 flag, (TX_x - 10)위치에 막대/
// EPD_DeepSleep(); 및 EPD_HW_Init(); 위치
