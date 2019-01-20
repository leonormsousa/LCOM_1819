#include "serialPort.h"

int hook_serialPort=4;
uint8_t scan_code_received=0;
static int message_byte = 0;
static uint8_t character;
static uint8_t message_received[3];

/** Subscribes the UART interruptions
 * Returns the hook_id used
 */
int serialPort_subscribe_int(uint8_t *bit_no)
{
	*bit_no = hook_serialPort;   	
	if(sys_irqsetpolicy(SERIALPORT_COM1_IRQ , IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_serialPort) != OK)
		return 1;
	return 0; 
}
/** Unsubscribes the UART interruptions
 */
int serialPort_unsubscribe_int()
{
	if(sys_irqdisable(&hook_serialPort) != OK)
		return 1;
	if(sys_irqrmpolicy(&hook_serialPort)!=OK)
		return 1;
	return 0;
}
/** Reads the Line Status Register
 */
int read_LSR(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS+5 ,(uint32_t*)data) != 0)            
		return 1;  
	return 0;
}
/** Reads the Receiver Buffer Register
 */
int read_RBR(uint8_t *data)
{
	uint8_t info;
	for (int i=0;i<5;i++)
	{
		if(read_LSR(&info)!=0)
						return 1;
		if(info & BIT(0))
		{
			if (sys_inb(COM1_INIT_ADDRESS , (uint32_t*)data) != 0)            
				return 1;
			printf("char: %x\n" , *data);
			tickdelay(100);
			return 0;
		}
	}
	return 1;
}
/** Reads the Interrupt Identification Register
 */
int read_IIR(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS +2, (uint32_t*)data) != 0)            
		return 1; 
	return 0;
}

/** Writes to the Transmitter Holding Register
 */
int write_THR(uint8_t data)
{
	uint8_t info;
	for (int i=0;i<5;i++)
	{
		if(read_LSR(&info)!=0)
			return 1;
		if(info & BIT(5))
		{
			if(sys_outb(COM1_INIT_ADDRESS, (uint32_t)data)!= 0)
				return 1;
			printf("char transmitted: %x\n", data);        
			return 0;
		}
	}
	return 1;
}

/** Writes to the Interrupt Enable Register
 */
int write_IER(uint8_t data)
{
  if(sys_outb(COM1_INIT_ADDRESS+1, (uint32_t)data)!= 0)
		return 1;
	return 0;
}

/** Reads the Interrupt Enable Register
 */
int read_IER(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS +1 , (uint32_t*)data) != 0)            
		return 1;  
	return 0;
}

/** Reads the Line Control Register
 */
int read_LCR(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS+3 , (uint32_t*)data) != 0)            
		return 1;
	return 0;
}

/** Reads the Division Latch Less significant byte
 */
int read_DLL(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS , (uint32_t*)data) != 0)            
		return 1; 
	return 0;
}

/** Reads the Division Latch Most significant byte
 */
int read_DLM(uint8_t *data)
{
	if (sys_inb(COM1_INIT_ADDRESS+1 , (uint32_t*)data) != 0)            
		return 1;
	return 0;
}

/** Writes to the FIFO Control Register
 */
int write_FCR(uint8_t data)
{
  if(sys_outb(COM1_INIT_ADDRESS+2, (uint32_t)data)!= 0)
    return 1;
  return 0;
}

/** Writes to the Line Control Register
 */
int write_LCR(uint8_t data)
{
  if(sys_outb(COM1_INIT_ADDRESS+3, (uint32_t)data)!= 0)
		return 1;
	return 0;
}

/** Writes to the Division Latch Less significant byte
 */
int write_DLL(uint8_t data)
{
  if(sys_outb(COM1_INIT_ADDRESS, (uint32_t)data)!= 0)
		return 1;
	return 0;
}

/** Writes to the Division Latch Most significant byte
 */
int write_DLM(uint8_t data)
{
	if(sys_outb(COM1_INIT_ADDRESS+1, (uint32_t)data)!= 0)
		return 1;
	return 0;
}

/** Enables the DLAB
 */
int enable_access_to_DL()
{
	uint8_t data;
	if(read_LCR(&data) != 0)
		return 1;  
	data = data | BIT(7);
	if(write_LCR(data)!=0)
		return 1;
	return 0;
}

/** Disables the DLAB
 */
int disable_access_to_DL()
{
	uint8_t data;
	if(read_LCR(&data) != 0)
		return 1;  
	data = data & 0x7F;
	if(write_LCR(data)!=0)
		return 1;
	return 0;
}

/** Sets the UART bit rate to the value passed as argument after setting the DLAB
 */
int set_bit_rate(uint16_t rate)
{
	uint16_t bit_rate = 115200/rate;
	uint8_t bit_rate_lsb = bit_rate & 0x00FF;
	uint8_t bit_rate_msb = ((bit_rate & 0xFF00) >> 8);
	if (enable_access_to_DL()!=0)
		return 1;
	if (write_DLL(bit_rate_lsb) !=0)
		return 1;
	if (write_DLM(bit_rate_msb) !=0)
		return 1;
	if (disable_access_to_DL() !=0)
		return 1;
	return 0;
}

/** Sends a character through the serial port using the THR
 */
int write_character(uint8_t character)
{
	uint8_t data;
	for (int i=0; i<5;i++)
	{
		if(read_LSR(&data)!=0)
			return 1;
		if(data & BIT(5))
		{
			if(write_THR(character)!=0)
				return 1;
			printf("character: %x \n", character);
			return 0;
		}
		tickdelay(100);
	}
	return 1;
}

/** Reads the acknowledgement byte sent by the device at the other end of the serial port that confirms the message sent was successfully received
 */
uint8_t get_acknowledgement_byte()
{
	uint8_t data=0;
	uint8_t acknowledgement_byte=0;
	for(int i=0; i<5; i++)
	{
		if (read_LSR(&data) != 0)
			return 0xFC;
		if (data & BIT(0))
		{
			if (read_RBR(&acknowledgement_byte) != 0)
				return 1;
			printf("ack byte: %x \n", acknowledgement_byte);
			if (acknowledgement_byte == 0xFA || acknowledgement_byte == 0xFE) 
				return acknowledgement_byte;    
			else
				return 0xFC;
		}
	tickdelay(100); 
	}
	return 0xFC;
}

/** Sends a message where the first character sent is a header and the last is a trailer byte. This way its easier to acknowledge the begging and ending of a given message. For every character sent, the device awaits for the corresponding acknowledgement byte
 */
int write_message(uint8_t message)
{
	if(write_character(0x55) != 0)
		return 1;
	if(get_acknowledgement_byte()!=0xFA)
		return 1;
	if (write_character(message) != 0)
		return 1;
	if(get_acknowledgement_byte()!=0xFA)
		return 1;
	if (write_character(0x88) != 0)
		return 1;
	if(get_acknowledgement_byte()!=0xFA)
		return 1;
	return 0;
}

/** Checks if the character received is suppose to be the header, the message or the trailer. If it is the header or the trailer is checks if it equals to 0x55 and 0x88 respectively
 */
int parse_message()
{
	if(message_byte == 0)
	{
		if (character == 0x55)
		{  
			message_received[0] = character;
			message_byte++;
			return 0;  
		}
		return 1;
	} 
	if(message_byte == 1)
	{
		message_received[1] = character;
		message_byte++;
		return 0;      
	}
	if(message_byte==2)
	{        
	
		if (character == 0x88)
		{  
			message_received[2] = character;
			message_byte=0;
			return 0;  
		}
		else
		{
			message_byte = 0;
			return 1;
		}
	}
	return 1;
}

/** Handles any UART interruption received. Depending on the success or not of the parsing process, it sends the corresponding acknowledgement byte (0xFA if it succeded and 0xFE otherwise)
 */
void serialPort_ih()
{
	uint8_t data;
	if (read_IIR(&data) != 0)
		return;
	if (data & BIT(0))
		return;  
	data = data & (BIT(1) | BIT(2) | BIT(3));
	if(data==(BIT(1) | BIT(2)))
	{
		if (read_LSR(&data) != 0)
			return;
		if (data & BIT(0))
			read_RBR(&data);
		write_character(0xFE);
	}
	if(data==BIT(2))
	{
		if (read_RBR(&character) != 0)
			return;
		if(parse_message()!=0)
			write_character(0xFE);
		if (message_byte == 2)
		{;
			write_character(0xFA);
			scan_code_received=message_received[1];
		}
		else
			write_character(0xFA);
	}
}

/** Configures the UART so that both ends of the serial port have the same settings and are able to communicate between them
 */
int configure_serialPort()
{
	//Configuration of 8 bytes per character, 2 stop bits, even parity and a standard bit rate of 9600
	uint8_t data;
	uint8_t IER_byte = BIT(0) | BIT(2);
	uint8_t LCR_byte = ((BIT(0) | BIT(1)) | BIT(2)  | (BIT(3) | BIT(4)));
	if (write_IER(IER_byte) != 0)
		return 1;
	if (write_LCR(LCR_byte) != 0)
		return 1;
	if (set_bit_rate(9600) != 0)
		return 1;
	if (read_LSR(&data) != 0)
		return 1;
	if (data & BIT(0))
		read_RBR(&data);
	return 0;
}
