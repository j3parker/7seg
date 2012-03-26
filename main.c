//#define __AVR_ATmega48P__
#include <avr/io.h>
#include <util/delay_basic.h>

#define PORTB_CONFIG 0b11100001
#define PORTD_CONFIG 0b11100000


static inline void uart_init(uint16_t ubrr);
static inline uint8_t getc();
static inline void shit_raw(uint8_t c);
static inline void shit_encoded(uint8_t n);


static inline void uart_init(uint16_t ubrr) {
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)(ubrr);

  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

static inline uint8_t getc() {
  while(!(UCSR0A & (1<<RXC0)));
  return UDR0;
}

static inline void shit_raw(uint8_t c) {
  // (c & 0b00000001) is always 0
  PORTD = c & 0b11100000;
  PORTB = ((c & 0b00011100) << 3)|((c & 0b00000010)>>1);
}

static inline void shit_encoded(uint8_t n) {
  if( n > 15 ) n = 14;
  switch(n) {
    case 0:  shit_raw(0b11101110); break;
    case 1:  shit_raw(0b00101000); break;
    case 2:  shit_raw(0b01011110); break;
    case 3:  shit_raw(0b01111100); break;
    case 4:  shit_raw(0b10111000); break;
    case 5:  shit_raw(0b11110100); break;
    case 6:  shit_raw(0b11110110); break;
    case 7:  shit_raw(0b01101000); break;
    case 8:  shit_raw(0b11111110); break;
    case 9:  shit_raw(0b11111100); break;
    case 10: shit_raw(0b11111010); break; // A
    case 11: shit_raw(0b10110110); break; // B
    case 12: shit_raw(0b11000110); break; // C
    case 13: shit_raw(0b00111110); break; // D
    case 14: shit_raw(0b11010110); break; // E
    case 15: shit_raw(0b11010010); break; // F
    default: break;
  }
}

int main(void) {
  DDRB = PORTB_CONFIG;
  DDRD = PORTD_CONFIG;

  // run quick test sequence
  uint8_t n = 0;
  for(; n < 16; n++ ) {
    shit_encoded(n);
    _delay_loop_2(30000UL);
  }

  uart_init(12);

  n = 0;
  while(1) {
    if (n & (1<<7)) shit_raw((n & 0b01111111) << 1);
    else            shit_encoded(n);
    n = getc();
  }
	return 0;
}

