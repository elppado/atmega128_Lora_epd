/*
 * keyscan.h
 *
 * Created: 2022-07-22 오후 10:53:52
 *  Author: chaedoyun
 */ 


#ifndef KEYSCAN_H_
#define KEYSCAN_H_


#define CR 0x0D
#define LF 0x0A
#define BS 0x08
#define SPB 0x20
#define ALT 0x11
#define SHIFT 0x12

unsigned char Key_Scan_sel,key_in_v,key_scan_line = 0;

unsigned char alt_flag = 0;

unsigned char kbdq10[3][36] = {
    {'\0','q','w','\0','a',ALT,SPB,'\0','e','s','d','p','x','z',SHIFT,'r','g','t',SHIFT,'v','c','f','u','h','y',CR,'b','n','j','o','l','i',BS,'$','m','k'},
    {'\0','Q','W','\0','A',ALT,SPB,'\0','E','S','D','P','X','Z',SHIFT,'R','G','T',SHIFT,'V','C','F','U','H','Y',CR,'B','N','J','O','L','I',BS,'=','M','K'},
    {'\0','#','1','\0','*',ALT,SPB,0x30,0x32,'4','5','@','8','7',SHIFT,'3','/','(',SHIFT,'?','9','6','_',':',')',CR,'!',',',';','+','"','-',BS,'=','.',0x60},
};



unsigned char keyScan(void){
	//키패드로 입력받는 함수
	unsigned char key_num = 0;
	unsigned char getPinData = 0;
	
	key_scan_line = 0xf7;

	for(Key_Scan_sel = 0; Key_Scan_sel < 5; Key_Scan_sel++){
		
		PORTA = key_scan_line;
        _delay_us(100);
        getPinData = (~PINC & 0x7f);
        
		if(getPinData != 0){
            
			switch(getPinData){
                    
				case 0x01:
                    key_num = (Key_Scan_sel*7) + 1;
                    break;
				
				case 0x02:
                    key_num = (Key_Scan_sel*7) + 2;
                    break;
				
				case 0x04:
                    key_num = (Key_Scan_sel*7) + 3;
                    break;
				
				case 0x08:
                    key_num = (Key_Scan_sel*7) + 4;
                    break;
				
				case 0x10:
                    key_num = (Key_Scan_sel*7) + 5;
                    break;
				
				case 0x20:
                    key_num = (Key_Scan_sel*7) + 6;
                    break;
				
				case 0x40:
                    key_num = (Key_Scan_sel*7) + 7;
                    break;
				
				default:
                    break;
			}
			return key_num;
		}
		key_scan_line = ((key_scan_line << 1)|0x01);
	}
	return 0;
}


unsigned char keypad_in(){
	// 스위치를 눌렀을때, 때기전까지 입력을 지연시켜,
	// 누르는 동안 여러번 눌리는 것을 방지하는 함수
	unsigned char Key_data,tmp = 0;
	
	Key_data=keyScan();
	tmp = keyScan();

	if(Key_data) {
    
		while(tmp != 0 ){
			// 눌려있는 동안 루프가
			tmp=keyScan();
            alt_flag = 0;
		}
        if (kbdq10[alt_flag][Key_data] == ALT) {
            alt_flag = 2;
            return 0;
        }
        else if(kbdq10[alt_flag][Key_data] == SHIFT){
            alt_flag = 1;
            return 0;
        }
        
	}
	return Key_data;
}




#endif /* KEYSCAN_H_ */
