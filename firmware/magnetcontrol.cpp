#include "mbed.h"

DigitalOut d1(p11);
DigitalOut d2(p12);
DigitalOut d3(p13);

int main( void ){
    char   recvbuf;
    Serial sl( USBTX , USBRX );
    sl.baud(115200);
    sl.format(8,Serial::None);
    while( 1 ){
        if( sl.readable() ){
            recvbuf = sl.getc();
            d1 = (int)(recvbuf&0x01);
            d2 = (int)(recvbuf&0x02);
            d3 = (int)(recvbuf&0x04);
            sl.putc( 'a' );
        }
    }
}