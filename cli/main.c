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
#include "sha256.h"

#define VIAL_ID_SIZE 8
#define FLASH_PAGE_SIZE 64

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

static int usb_read(hid_device *device, uint8_t *buffer, size_t len) {
	while (len > 0) {
		int ret = hid_read(device, buffer, len);
		if (ret < 0)
			return ret;
		len -= ret;
		buffer += ret;
		usleep(100 * 1000);
	}

	return 0;
}

/* calculate sha256 hash of the data and check that it matches the recorded hash; returns 0 if check passed, 1 otherwise */
int check_hash(void *data, size_t size, void *hash) {
	uint8_t calculated[32];
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, size);
	sha256_final(&ctx, calculated);
	return memcmp(calculated, hash, sizeof(calculated)) != 0;
}

/* searches for a compatible vial device in infinite loop */
hid_device *search_device(void *vial_uid) {
	return hid_open(0x1234, 0x5678, NULL);
}

int main(int argc, char **argv) {
	uint8_t hid_buffer[129];
	uint8_t CMD_BOOTLOADER_IDENT[8] = {'V','C',0x00};
	uint8_t CMD_GET_VIAL_ID[8] = {'V','C',0x01};
	uint8_t CMD_FLASH[8] = {'V','C',0x02};
	uint8_t CMD_REBOOT[8] = {'V','C',0x03};
	hid_device *handle = NULL;
	FILE *firmware_file = NULL;
	void *file_buffer = NULL;
	void *firmware_buffer = NULL;
	void *vial_id = NULL;
	int error = 0;
	long file_size, firmware_size;
	int firmware_pages;

	setbuf(stdout, NULL);

	printf("HID-Flash v1.4a - STM32 HID Bootloader Flash Tool\n");
	printf("(c) 04/2018 - Bruno Freitas - http://www.brunofreitas.com/\n\n");

	if(argc != 2) {
		printf("Usage: hid-flash <firmware_bin_file>\n");

		return 1;
	}

	hid_init();

	firmware_file = fopen(argv[1], "rb");
	if(!firmware_file) {
		printf("Error opening firmware file: %s\n", argv[1]);
		error = 1;
		goto exit;
	}

	/* Get firmware size */
	fseek(firmware_file, 0, SEEK_END);
	file_size = ftell(firmware_file);
	fseek(firmware_file, 0, SEEK_SET);

	if (file_size < 64) {
		printf("Firmware file is too small to be valid!\n");
		error = 1;
		goto exit;
	}

	/* Load firmware into memory */
	if (!(file_buffer = malloc(file_size))) {
		printf("Failed to allocate memory for firmware data.\n");
		error = 1;
		goto exit;
	}

	if (fread(file_buffer, 1, file_size, firmware_file) != file_size) {
		printf("Failed to read the firmware.\n");
		error = 1;
		goto exit;
	}

	if (memcmp(file_buffer, "VIALFW00", 8) == 0) {
		/* is this a vial firmware package? if so, check hash and keep track of vial UID */
		vial_id = (char*)file_buffer + 8;
		firmware_buffer = (char*)file_buffer + 64;
		firmware_size = file_size - 64;
		if (check_hash(firmware_buffer, firmware_size, (char*)file_buffer + 32)) {
			printf("Firmware doesn't pass hash check. The file is corrupt.\n");
			error = 1;
			goto exit;
		}
	} else {
		/* otherwise it's a plain bin containing the entire firmware package */
		firmware_buffer = file_buffer;
		firmware_size = file_size;
		printf("\nWARNING: flashing a plain binary firmware. This is not recommended, please switch to the .vfw format!\n\n\n");
	}

	handle = search_device(vial_id);

	if (!handle) {
		printf("Unable to open device.\n");
		error = 1;
		goto exit;
	}

	// Identify bootloader version and feature flags
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_BOOTLOADER_IDENT, sizeof(CMD_BOOTLOADER_IDENT));
	if(!usb_write(handle, hid_buffer, 9)) {
		printf("Error while asking for bootloader ident\n");
		error = 1;
		goto exit;
	}

	error = usb_read(handle, hid_buffer, 8);
	if (error != 0) {
		printf("Error while retrieving bootloader ident\n");
		error = 1;
		goto exit;
	}
	if (hid_buffer[0] != 0) {
		printf("Error: unsupported bootloader version: %d\n", hid_buffer[0]);
		error = 1;
		goto exit;
	}

	// Get keyboard ID
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_GET_VIAL_ID, sizeof(CMD_GET_VIAL_ID));
	if(!usb_write(handle, hid_buffer, 9)) {
		printf("Error while asking for Vial ID\n");
		error = 1;
		goto exit;
	}

	error = usb_read(handle, hid_buffer, 8);
	if (error != 0) {
		printf("Error while retrieving Vial ID\n");
		error = 1;
		goto exit;
	}

	if (vial_id && memcmp(vial_id, hid_buffer, VIAL_ID_SIZE) != 0) {
		printf("Unexpected Vial ID\n");
		error = 1;
		goto exit;
	}

	if (firmware_size % FLASH_PAGE_SIZE != 0)
		firmware_size += FLASH_PAGE_SIZE - firmware_size % FLASH_PAGE_SIZE;
	firmware_pages = firmware_size / FLASH_PAGE_SIZE;

	// Send flash command to put HID bootloader in initial stage...
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_FLASH, sizeof(CMD_FLASH));
	/* number of pages to flash as little-endian */
	hid_buffer[4] = firmware_pages % 256;
	hid_buffer[5] = firmware_pages / 256;

	printf("Sending flash pages command...\n");

	// Flash is unavailable when writing to it, so USB interrupt may fail here
	if(!usb_write(handle, hid_buffer, 9)) {
		printf("Error while sending flash pages command.\n");
		error = 1;
		goto exit;
	}

	memset(hid_buffer, 0, sizeof(hid_buffer));

	// Send Firmware File data
	printf("Flashing firmware...\n");

	size_t written = 0;
	void *buf = firmware_buffer;

	while (written < firmware_size) {
		size_t chunk_sz = FLASH_PAGE_SIZE;
		if (chunk_sz > firmware_size - written)
			chunk_sz = firmware_size - written;

		memcpy(&hid_buffer[1], buf, chunk_sz);

		// Flash is unavailable when writing to it, so USB interrupt may fail here
		if(!usb_write(handle, hid_buffer, 1 + FLASH_PAGE_SIZE)) {
			printf("Error while flashing firmware data.\n");
			error = 1;
			goto exit;
		}

		written += chunk_sz;
		buf = (char*)buf + chunk_sz;
		printf("\r[%d/%d]: %d%%", (int)written, (int)firmware_size, 100 * (int)written / (int)firmware_size);
	}
	printf("\n");

	printf("Rebooting...\n");
	/* Reboot */
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_REBOOT, sizeof(CMD_REBOOT));
	usb_write(handle, hid_buffer, 9);

	printf("Ok!\n");

	exit:

	if(handle) {
		hid_close(handle);
	}

	hid_exit();

	if(firmware_file) {
		fclose(firmware_file);
	}

	if (file_buffer) {
		free(file_buffer);
	}

	return error;
}
