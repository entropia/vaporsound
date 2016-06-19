#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <time.h>

#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LED 0x08
#define DATA_SHIFT 4

// 2 0x27
int i2c_open(unsigned int bus, uint8_t device) {
	char buf[64];
	int fd;

	snprintf(buf, sizeof(buf), "/dev/i2c-%u", bus);
	if((fd = open(buf, O_RDWR)) < 0) {
		perror("opening i2c dev failed");
		return -1;
	}

	if(ioctl(fd, I2C_SLAVE, device) < 0) {
		perror("selecting i2c slave failed");
		return -1;
	}

	return fd;
}

int pcf8574_read(int fd, uint8_t *out) {
	if(read(fd, out, 1) < 0) {
		perror("reading byte from i2c failed");
		return -1;
	}

	return 0;
}

int pcf8574_write(int fd, uint8_t byte) {
	if(write(fd, &byte, 1) < 0) {
		perror("writing byte to i2c failed");
		return -1;
	}

	return 0;
}

struct hd44780 {
	int fd;

	bool led;
};

struct hd44780 *hd44780_open(unsigned int bus, uint8_t address_bits) {
	int fd = i2c_open(bus, 0x20 | address_bits);

	if(fd < 0)
		return NULL;

	struct hd44780 *out = calloc(sizeof(*out), 1);
	out->fd = fd;
	out->led = false;

	return out;
}

enum hd44780_register {
	HD44780_CMD,
	HD44780_DATA
};

int hd44780_write(struct hd44780 *lcd, enum hd44780_register reg, uint8_t byte) {
	uint8_t out;

	out = (reg == HD44780_CMD) ? 0 : RS;
	out |= lcd->led ? LED : 0;

	if(pcf8574_write(lcd->fd, out | ((byte >> 4) << DATA_SHIFT) | EN) < 0)
		return -1;

	if(pcf8574_write(lcd->fd, out | ((byte >> 4) << DATA_SHIFT)) < 0)
		return -1;

	if(pcf8574_write(lcd->fd, out | ((byte & 0x0F) << DATA_SHIFT) | EN) < 0)
		return -1;

	if(pcf8574_write(lcd->fd, out | ((byte & 0x0F) << DATA_SHIFT)) < 0)
		return -1;

	return 0;
}

int hd44780_reset(struct hd44780 *lcd) {
	uint8_t raw_cmds[] = {
		0x03, // select 8 bit mode
		0x03,
		0x03,
		0x02, // select 4 bit mode
	};

	uint8_t out = lcd->led ? LED : 0;

	for(int i = 0; i < sizeof(raw_cmds); i++) {
		if(pcf8574_write(lcd->fd, out | (raw_cmds[i] << DATA_SHIFT) | EN) < 0)
			return -1;

		if(pcf8574_write(lcd->fd, out | (raw_cmds[i] << DATA_SHIFT)) < 0)
			return -1;

		usleep(5000);
	}

	uint8_t init_cmds[] = {
		0x28, // 2 lines, 5x8 pixels
		0x08, // turn off display
		0x01, // clear display
		0x06, // cursor l2r, no shift
		0x0C // turn on display
	};

	for(int i = 0; i < sizeof(init_cmds); i++) {
		if(hd44780_write(lcd, HD44780_CMD, init_cmds[i]) < 0)
			return -1;
	}

	return 0;
}

int hd44780_clear(struct hd44780 *lcd) {
	return hd44780_write(lcd, HD44780_CMD, 0x01);
}

int hd44780_putchar(struct hd44780 *lcd, unsigned char c) {
	return hd44780_write(lcd, HD44780_DATA, c);
}

int hd44780_puts(struct hd44780 *lcd, uint8_t line, const char *str) {
	if(hd44780_write(lcd, HD44780_CMD, 0x80 | ((line == 1) ? 0x40 : 0)) < 0)
		return -1;

	while(*str) {
		if(hd44780_putchar(lcd, *str) < 0)
			return -1;

		str++;
	}

	return 0;
}

int hd44780_led(struct hd44780 *lcd, bool on) {
	lcd->led = on;

	uint8_t out = lcd->led ? LED : 0;

	return pcf8574_write(lcd->fd, out);
}

int hd44780_cursor(struct hd44780 *lcd, bool visible, bool blinking) {
	uint8_t out = 0x0C;

	if(visible) {
		out |= 0x02;

		// setting "blinking" but not "visible" causes the cursor to be
		// visible (and blinking), so we only ever set the bit when "visible" is true

		if(blinking)
			out |= 0x01;
	}

	return hd44780_write(lcd, HD44780_CMD, out);
}

int hd44780_custom_char(struct hd44780 *lcd, uint8_t id, uint8_t data[static 8]) {
	if(id > 8)
		return -1;

	uint8_t base_addr = id * 0x08;

	if(hd44780_write(lcd, HD44780_CMD, 0x40 | base_addr) < 0)
		return -1;

	for(int i = 0; i < 8; i++) {
		if(hd44780_write(lcd, HD44780_DATA, data[i]) < 0)
			return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
	struct hd44780 *lcd = hd44780_open(2, 0x7);

	hd44780_led(lcd, true);
	hd44780_reset(lcd);

	hd44780_cursor(lcd, false, false);

	int a = time(0);

	char buf[16];
	sprintf(buf, "%08u", a++);

	hd44780_puts(lcd, 0, "                ");
	hd44780_puts(lcd, 1, "   vaporsound!  ");

	while(1) {
		for(int i = 0; i < 80; i++) {
			int cnt = i / 5;
			int partial = i % 5;

			if(cnt > 0 && partial == 0) {
				cnt--;
				partial = 5;
			}

			printf("%d %d\n", cnt, partial);

			uint8_t data[8];
			for(int j = 0; j < 8; j++)
				data[j] = (uint8_t)(~((1 << (8-partial)) - 1)) >> 3;

			printf("0x%02x\n", data[0]);

			hd44780_custom_char(lcd, 1, data);

			char buf[16];
			memset(buf, 0xFF, 16);

			if(cnt < 16) {
				buf[cnt] = 0x01;
				for(int j = cnt+1; j < 16; j++)
					buf[j] = ' ';
			}

			hd44780_puts(lcd, 0, buf);

			usleep(100000);
		}
	}

	return EXIT_SUCCESS;
}
