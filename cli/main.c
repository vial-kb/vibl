/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "hidapi.h"

static int usb_write(hid_device *device, uint8_t *buffer, int len) {
	int retries = 20;
	int retval;

	while(((retval = hid_write(device, buffer, len)) < len) && --retries) {
		if(retval < 0) {
			usleep(100 * 1000); // No data has been sent here. Delay and retry.
		} else {
			return 0; // Partial data has been sent. Firmware will be corrupted. Abort process.
		}
	}

	if(retries <= 0) {
		return 0;
	}

	return 1;
}

int main(int argc, char **argv) {
	uint8_t page_data[1024];
	uint8_t hid_buffer[129];
	uint8_t CMD_FLASH[8] = {'B','T','L','D','C','M','D', 0x01};
	uint8_t CMD_REBOOT[8] = {'B','T','L','D','C','M','D', 0x02};
	hid_device *handle = NULL;
	size_t read_bytes;
	FILE *firmware_file = NULL;
	int error = 0;
	long firmware_size;
	int firmware_pages;

	setbuf(stdout, NULL);

	printf("HID-Flash v1.4a - STM32 HID Bootloader Flash Tool\n");
	printf("(c) 04/2018 - Bruno Freitas - http://www.brunofreitas.com/\n\n");

	if(argc != 2) {
		printf("Usage: hid-flash <firmware_bin_file>\n");

		return 1;
	}

	hid_init();

	handle = hid_open(0x1209, 0xBABE, NULL);

	if (!handle) {
		printf("Unable to open device.\n");
		error = 1;
		goto exit;
	}

	firmware_file = fopen(argv[1], "rb");
	if(!firmware_file) {
		printf("Error opening firmware file: %s\n", argv[1]);
		error = 1;
		goto exit;
	}

	fseek(firmware_file, 0, SEEK_END);
	/* Get firmware size and number of pages */
	firmware_size = ftell(firmware_file);
	if (firmware_size % 1024 != 0)
		firmware_size += 1024 - firmware_size % 1024;
	firmware_pages = firmware_size / 1024;

	// Send flash command to put HID bootloader in initial stage...
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_FLASH, sizeof(CMD_FLASH));
	/* number of pages to flash as little-endian */
	hid_buffer[9] = firmware_pages % 256;
	hid_buffer[10] = firmware_pages / 256;

	printf("Sending flash pages command...\n");

	// Flash is unavailable when writing to it, so USB interrupt may fail here
	if(!usb_write(handle, hid_buffer, 129)) {
		printf("Error while sending flash pages command.\n");
		error = 1;
		goto exit;
	}

	memset(hid_buffer, 0, sizeof(hid_buffer));

	fseek(firmware_file, 0, SEEK_SET);

	// Send Firmware File data
	printf("Flashing firmware...\n");

	memset(page_data, 0, sizeof(page_data));
	read_bytes = fread(page_data, 1, sizeof(page_data), firmware_file);

	while(read_bytes > 0) {

		for(int i = 0; i < 1024; i += 128) {
			memcpy(&hid_buffer[1], page_data + i, 128);

			// Flash is unavailable when writing to it, so USB interrupt may fail here
			if(!usb_write(handle, hid_buffer, 129)) {
				printf("Error while flashing firmware data.\n");
				error = 1;
				goto exit;
			}
		}

		memset(page_data, 0, sizeof(page_data));
		read_bytes = fread(page_data, 1, sizeof(page_data), firmware_file);
	}

	printf("Rebooting...\n");
	/* Reboot */
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_REBOOT, sizeof(CMD_REBOOT));
	if(!usb_write(handle, hid_buffer, 129)) {
		printf("Error while sending reboot command.\n");
		error = 1;
		goto exit;
	}

	printf("Ok!\n");

	exit:

	if(handle) {
		hid_close(handle);
	}

	hid_exit();

	if(firmware_file) {
		fclose(firmware_file);
	}

	return error;
}
