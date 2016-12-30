/******************************************************************
*  PS2 controller library for AVR
*
*  Loosly based off of code by Shutter on Arduino Forums, and modified 
*  (added analog stick support) by Bill Porter (http://www.billporter.info/?p=219)
*
*  Converted to generic AVR library without C++ or Arduino references, added 
*  the ability to use any pins for input / output, fixed numerous bugs, actually
*  follow proper PS2 controller protocols, etc by Wyatt Olson (http://digitalcave.ca)
*
*  Thanks to http://store.curiousinventor.com/guides/PS2/#high_level for protocol details.
*
*  Lib version history
*   ????    0.1    Bill Porter    -Made into Arduino library
*                    -Added analog stick support
*   2010-07  0.2    Wyatt Olson    -Converted to generic AVR-GCC library
*                    -Added port / pin initializers (psx_init())
*                    -Changed to use uintx_t data types instead of char
*                    -Fixed many bugs (including glaring protocol violations)
*                    -Code cleanup
*   2011-04  0.3    Wyatt Olson    -Fixed a number of logic bugs, where the impl did not match spec
*                    -Code cleanup (merged methods, fixed comments, etc)
*
*
* Usage information:
*  Connect PSX controller wires to AVR:
*    Brown:  Data
*    Blue:  Clock
*    Orange: Command
*    Yellow: Attention
*  Call psx_init() function with the port / pins which you are using
*  Call psx_read_gameport() regularly to poll and update.
*  Call psx_button() function with PSB_X argument to read the value of the given button (see
*         psx.h for possible buttons, e.g. PSB_SELECT, PSB_L3, PSB_PAD_UP, etc)
*  Call psx_stick() function with PSS_X argument to read the analog stick value (possible
*         values are PSS_RX, PSS_RY, PSS_LX, PSS_LY for right / left and X / Y)
* 
******************************************************************/


#include "psx.h"

//Define ports used
static volatile uint8_t *_data_port;
static volatile uint8_t *_clock_port;
static volatile uint8_t *_command_port;
static volatile uint8_t *_attention_port;

//Define the input
static volatile uint8_t *_data_in;

//Define ddr used
static volatile uint8_t *_data_ddr;
static volatile uint8_t *_clock_ddr;
static volatile uint8_t *_command_ddr;
static volatile uint8_t *_attention_ddr;

//Define pins to use
static uint8_t _clock_pin;
static uint8_t _command_pin;
static uint8_t _attention_pin;
static uint8_t _data_pin;

//Common structure for last read data
static uint8_t _data[21];


/* Private.  The actual serial transfer.  Handles clock.  The PSX controller is full 
 * duplex, so this will send a byte as well as receive one.
 */
 
uint8_t _psx_gamepad_shift(uint8_t transmit_byte) {
  uint8_t received_byte = 0;
  for(uint8_t i = 0; i < 8; i++) {
    //Drop the clock...
    *_clock_port &= ~_BV(_clock_pin);
  
    //...set the command (outgoing) pin...
    if (transmit_byte & (_BV(i))) {
      *_command_port |= _BV(_command_pin);
    }
    else {
      *_command_port &= ~_BV(_command_pin);
    }
    
    //...wait half the clock cycle...
    _delay_us(CTRL_CLK);
    
    //...raise the clock to HIGH...
    *_clock_port |= _BV(_clock_pin);
    
    //...at which point you read the data...
    if(*_data_in & _BV(_data_pin)) {
      received_byte |= _BV(i);
    }
    
    //...and wait the other half of the clock cycle
    _delay_us(CTRL_CLK);
  }

  //Clock should already be high at this point, but just to be sure...
  *_clock_port |= _BV(_clock_pin);
  
  return received_byte;
}

/* Private.  Sends a command using the shift method.
 */
void _psx_send_command(uint8_t send_data[], uint8_t size){
  //Before you submit each command packet, you must set attention low; once
  // you are done each packet, return it high.  You have to toggle the line before
  // you submit another command.
  *_attention_port &= ~(_BV(_attention_pin));
  *_command_port |= _BV(_command_pin);

  //Clock should always be high; it is an active low line...
  *_clock_port |= _BV(_clock_pin);
    
  for (uint8_t i = 0; i < size; i++){
    send_data[i] = _psx_gamepad_shift(send_data[i]);
  }
  
  *_attention_port |= _BV(_attention_pin);
}



/* Reads the gamepad.  You need to call this whenever you want updated state.
 */
void psx_read_gamepad() {
  _data[0] = 0x01;
  _data[1] = 0x42;
  for (uint8_t i = 2; i < 21; i++){
    _data[i] = 0x00;
  }
  _psx_send_command(_data, 21);
}

/* Initialize the pins, and set the controller up to the correct mode.  This must be called
 * before any other psx_* functions are called.
 * (TODO: separate the config options into optional functions, and call based on flags,
 * to allow calling programs to determine what options to use)
 *
 * Note: We extrapolate the DDR and PIN registers based off of the associated PORT 
 * register.  This assumes that the DDR and PIN registers come directly after the PORT
 * equivalents, with DDRX at the next address after PORTX and PINX coming at the next
 * address after DDRX.  This is valid for all the chips I have looked at; however, it is
 * highly recommended that you check any new chips which you want to use this library with.
 */
void psx_init(volatile uint8_t *data_port, uint8_t data_pin,
        volatile uint8_t *clock_port, uint8_t clock_pin,
        volatile uint8_t *command_port, uint8_t command_pin,
        volatile uint8_t *attention_port, uint8_t attention_pin){

  //Store the ports...
  _data_port = data_port;
  _clock_port = clock_port;
  _command_port = command_port;
  _attention_port = attention_port;
  
  //... and data direction registers...
  _data_ddr = data_port - 0x1;
  _clock_ddr = clock_port - 0x1;
  _command_ddr = command_port - 0x1;
  _attention_ddr = attention_port - 0x1;

  //... and data input register...
  _data_in = data_port - 0x2;

  //... and pin numbers.
  _clock_pin = clock_pin;
  _command_pin = command_pin;
  _attention_pin = attention_pin;
  _data_pin = data_pin;
  
  
  //Set clock, attention, and command pins to output mode
  *_clock_ddr |= _BV(_clock_pin);
  *_attention_ddr |= _BV(_attention_pin);
  *_command_ddr |= _BV(_command_pin);

  //Set data pin to input mode, and set pullup resistor
  *_data_ddr &= ~(_BV(_data_pin));
  *_data_port |= _BV(_data_pin);

  //Initialize game pad
  *_clock_port |= _BV(_clock_pin);
  *_command_port |= _BV(_command_pin);
  
  
  //Init by polling once
  psx_read_gamepad();


  //Enter Config Mode
  uint8_t enter_config_command[] = {0x01, 0x43, 0x00, 0x01, 0x00};
  _psx_send_command(enter_config_command, 5);

  // Lock to Analog Mode on Stick
  uint8_t lock_analog_mode_command[] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
  _psx_send_command(lock_analog_mode_command, 9);

  //Exit config mode
  uint8_t exit_config_command[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
  _psx_send_command(exit_config_command, 9);
}


uint16_t psx_buttons() {
  uint16_t buttons = *(uint16_t*)(_data + 3); //Get 2 bytes, comprising data positions 3 and 4.
  return ~buttons;
}

uint8_t psx_button(uint16_t button) {
  uint16_t buttons = psx_buttons();
  return ((buttons & button) > 0);
}

uint8_t psx_stick(unsigned int stick) {
  return _data[stick];
} 