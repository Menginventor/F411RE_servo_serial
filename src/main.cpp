#include <mbed.h>
#define servo_max_us  1940
#define servo_min_us  1100
Serial pc (USBTX,USBRX);
PwmOut myservo(PA_9);//D8 pin ,Create a servo object connected to the specified PwmOut pin.
char Serial_data [32];

void servo_write(float percent){
        if(percent<0.0 || percent>1.0) {
                pc.printf("error , value out of range\n" );
                return;
        }
        int pulsewidth_us =  percent* (servo_max_us - servo_min_us)  + servo_min_us;

        pc.printf("write value %f to servo , period = %f ms\n", percent, float(pulsewidth_us)/1000.0);


        myservo.pulsewidth_us(pulsewidth_us);
}

void Rx_interrupt(){
        static int str_index = 0;
        char dataIn = pc.getc();
        if (str_index>=30) {
                str_index = 0;
        }
        if(dataIn == '\n') {
                pc.printf("recieve \"%s\"\n", Serial_data);
                bool error = false;
                int point_index = str_index;
                for(int i = 0; i<str_index; i++) {
                        if (i == 0) {
                                if(!( (Serial_data[i]>='0' && Serial_data[i]<='9') || Serial_data[i] == '-'|| Serial_data[i] == '+'  ) ) {
                                        error = true;
                                        break;
                                }
                        }
                        else{
                                if(!( (Serial_data[i]>='0' && Serial_data[i]<='9') ||  Serial_data[i] == '.'  )) {
                                        error = true;
                                        break;
                                }
                                else if (Serial_data[i] == '.') {
                                        if(point_index == str_index) {
                                                point_index = i;
                                        }
                                        else{
                                                error = true;
                                                break;
                                        }
                                }
                        }
                        //pc.printf("%d : %c\n",i,Serial_data[i] );
                }
                if (point_index == str_index) {
                        Serial_data[point_index] == '.';
                        Serial_data[point_index+1] == '\0';
                        str_index++;
                }
                //pc.printf("%s\n", error ? "true" : "false");
                if(!error) {

                        int point_digit = (str_index-1)-point_index;
                        int mantissa = 0;

                        for (int d = 0; d<str_index; d++) {
                                int digit_index = (str_index-1)-d;
                                int digit = d;
                                if (d > point_digit) digit = d-1;
                                if (d == point_digit) continue;
                                int digit_value = 1;
                                for (int dv = 0; dv<digit; dv++) digit_value*=10;

                                if(Serial_data[digit_index] == '-' ) {
                                        mantissa = -mantissa;
                                }
                                else if( Serial_data[digit_index] == '+') ;
                                else {
                                      //  pc.printf("%d : %d : %c\n",digit,digit_value,Serial_data[digit_index] );
                                        mantissa += (Serial_data[digit_index]-'0')*digit_value;
                                }


                        }
                        int divide_value = 1;
                        for (int dv = 0; dv<point_digit; dv++) divide_value*=10;
                        int int_val = mantissa/divide_value;
                        float float_val = float(mantissa)/float(divide_value);
                        printf("int_val = %d , float_val = %f\n", int_val,float_val );
                        servo_write(float_val);
                }
                Serial_data[0] = '\0';
                str_index = 0;
        }
        else if(dataIn != '\r') {
                Serial_data[str_index] = dataIn;
                Serial_data[str_index+1] = '\0';
                str_index++;
        }

}
int main() {
        myservo.period_ms(20);
        pc.baud(9600);
        pc.attach(&Rx_interrupt, Serial::RxIrq);
        servo_write(0.0);
        pc.printf("enter servo value (0 - 1)\n");
        while(1) {
                // put your main code here, to run repeatedly:
        }
}
