/*
 * buzz.h
 *
 * Created: 2022-07-22 오후 10:52:14
 *  Author: chaedoyun
 */ 


#ifndef BUZZ_H_
#define BUZZ_H_


#define ffch 20

#define Do 1908/ffch // 262Hz (3817us) 1908us
#define Re 1700/ffch // 294Hz (3401us) 1701us
#define Mi 1515/ffch // 330Hz (3030us) 1515us
#define Fa 1432/ffch // 349Hz (2865us) 1433us
#define Sol 1275/ffch // 370Hz (2703us) 1351us
#define La 1136/ffch // 440Hz (2273us) 1136us
#define Si 1012/ffch // 494Hz (2024us) 1012us


unsigned char buzz_flag = 1; // 1 = buzzer ON, 0 == buzzer OFF;


void interrupt_init(){
    

    EIMSK |= 0x02; // INT1 인터럽트 활성화
    EICRA |= 0x08; // INT1 하강엣지
    sei();
}


ISR(INT1_vect){ // sKEY 스위치 하강엣지 인터럽트
    
    buzz_flag ^= 1;
}



void myDelay_us(unsigned int delay){
    
    for(int i=0; i<delay; i++){
        _delay_us(1);
    }
}

void SSound(int time) {
    
    if (buzz_flag){
        
        int tim;
        tim = 100000 / time;
        for(int i=0; i<tim; i++){
            PORTD |= (1<<PD7); //buzzer on, PORTG의 4번 핀 off(out 1)
            myDelay_us(time);
            PORTD &= ~(1<<PD7); //buzzer off, PORTG의 4번 핀 on(out 0)
            myDelay_us(time);
        }
        PORTD |= (0<<PD7); //buzzer off, PORTG의 4번 핀 off(out 0)
            
    }
}



#endif /* BUZZ_H_ */
