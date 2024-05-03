#include<reg51.h>
#include<intrins.h>

void i2c_start(void);
void i2c_stop(void);
void i2c_ACK(void);
void i2c_write(unsigned char);
void i2c_DevWrite(unsigned char);
void lcd_send_cmd(unsigned char);
void lcd_send_data(unsigned char);
void lcd_send_str(unsigned char *);
void lcd_slave(unsigned char );
void delay_ms(unsigned int);

unsigned char slave1=0x4E;
unsigned char slave_add;
unsigned int count = 0;   // Counter variable

sbit IR_SENSOR = P1^0;    // IR sensor input pin
sbit BUZZ = P1^1;


sbit scl=P2^1;
sbit sda=P2^0;

void i2c_start(void)
{
sda=1;_nop_();_nop_();
scl=1;_nop_();_nop_();
sda=0;_nop_();_nop_();
}

void i2c_stop(void)
{
scl=0;
sda=0;
scl=1;
sda=1;
}

void lcd_slave(unsigned char slave)
{
slave_add=slave;
}

void i2c_ACK(void)
{
scl=0;
sda=1;
scl=1;
while(sda);
}

void i2c_write(unsigned char dat)
{
unsigned char i;
for(i=0;i<8;i++)
{
scl=0;
sda=(dat&(0x80)>>i)?1:0;
scl=1;
}
}

void lcd_send_cmd(unsigned char cmd)
{
unsigned char cmd_l,cmd_u;
cmd_l=(cmd<<4)&0xf0;
cmd_u=(cmd &0xf0);
i2c_start();			 //BL EN RW RS 1 1 0 0
i2c_write(slave_add);
i2c_ACK();
i2c_write(cmd_u|0x0C);
i2c_ACK();
delay_ms(1);
i2c_write(cmd_u|0x08); // 1 0 0 0
i2c_ACK();
delay_ms(10);
i2c_write(cmd_l|0x0C);	 // 1 1 0 0
i2c_ACK();
delay_ms(1);
i2c_write(cmd_l|0x08);
i2c_ACK();
delay_ms(10);
i2c_stop();
}

void lcd_send_data(unsigned char dataw)// 1 1 0 1
{
unsigned char dataw_l,dataw_u;
dataw_l=(dataw<<4)&0xf0;
dataw_u=(dataw &0xf0);
i2c_start();
i2c_write(slave_add);
i2c_ACK();
i2c_write(dataw_u|0x0D);//BL EN RW RS   1 1 0 1
i2c_ACK();
delay_ms(1);
i2c_write(dataw_u|0x09);// 1 0 0 1
i2c_ACK();
delay_ms(10);
i2c_write(dataw_l|0x0D);
i2c_ACK();
delay_ms(1);
i2c_write(dataw_l|0x09);
i2c_ACK();
delay_ms(10);
i2c_stop();
}

void lcd_send_str(unsigned char *p)
{
while(*p != '\0')
lcd_send_data(*p++);
}


void delay_ms(unsigned int n)
{
unsigned int m;
for(n;n>0;n--)
{
  for(m=121;m>0;m--);
    _nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
}

void lcd_init()
{
lcd_send_cmd(0x02);	// Return home
lcd_send_cmd(0x28);	// 4 bit mode
lcd_send_cmd(0x0C);	// Display On , cursor off
lcd_send_cmd(0x06);	// Increment Cursor (shift cursor to right)
lcd_send_cmd(0x01); // clear display
}
void update_counter() {
    count++;  // Increment count
    lcd_send_cmd(0xC8);  // Set cursor to the position to display the count
    lcd_send_data((count / 100) + '0');  // Display hundreds digit
    lcd_send_data(((count / 10) % 10) + '0');  // Display tens digit
    lcd_send_data((count % 10) + '0');  // Display units digit
}


void main(void)
{
lcd_slave(slave1);
lcd_init();
lcd_send_cmd(0x80);
lcd_send_str("Object Counter");
lcd_send_cmd(0xC0);
lcd_send_str("Count : 000");

BUZZ=0;
   while (1) {            // Infinite loop
        if (IR_SENSOR == 0) {
						delay(50);

            while (IR_SENSOR == 0){
							BUZZ=1;// Check if IR sensor detects an object
							};  // Wait until the object is removed to avoid counting multiple times
            update_counter();
						BUZZ=0;// Update the counter and display it on the LCD
        }
    }
}

