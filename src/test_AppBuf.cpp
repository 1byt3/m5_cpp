/*
 *                                   1byt3
 *
 *                              License Notice
 *
 * 1byt3 provides a commercial license agreement for this software. This
 * commercial license can be used for development of proprietary/commercial
 * software. Under this commercial license you do not need to comply with the
 * terms of the GNU Affero General Public License, either version 3 of the
 * License, or (at your option) any later version.
 *
 * If you don't receive a commercial license from us (1byt3), you MUST assume
 * that this software is distributed under the GNU Affero General Public
 * License, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Contact us for additional information: customers at 1byt3.com
 *
 *                          End of License Notice
 */

/*
 * Tests for the MQTT 5 Low Level Packet Library
 *
 * Copyright (C) 2017 1byt3, customers at 1byt3.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AppBuf.hpp"

#include <iostream>
#include <iomanip>
#include <cstring>

#define error_exit(msg)						\
	std::cout << __func__ << ": " << (msg) << " error"	\
		  << std::endl;					\
	return 1;

#define test_rc(rc, msg)				\
	std::cout << (((rc) != 0) ? "ERROR\t" : "OK\t")	\
		  << (msg) << std::endl;		\
	return 0;

int test(void)
{
	const char str[] = "Hello, World!";
	char str2[sizeof(str)];
	m5::AppBuf *buf;
	uint16_t len;

	buf = new m5::AppBuf(64);

	for (std::size_t i = 0; i < buf->size(); i++) {
		if (buf->bytesToWrite() != (buf->size() - i)) {
			error_exit(": bytesToWrite error");
		}

		buf->writeNum8((uint8_t)i);
	}

	for (std::size_t i = 0; i < buf->size(); i++) {
		buf->readNum8();
	}

	if (buf->bytesToRead() != 0) {
		error_exit("bytesToRead");
	}

	if (buf->bytesToWrite() != 0) {
		error_exit("bytesToWrite");
	}

	buf->rewind();
	if (buf->bytesToRead() != buf->length()) {
		error_exit("rewind");
	}

	/* Buffer data is 0x00 0x01 0x02 0x03 0x04 0x05 ... */
	if (buf->readNum16() != 0x0001) {
		error_exit("readNum16");
	}

	if (buf->bytesToRead() != (buf->length() - sizeof(uint16_t))) {
		error_exit("readNum16");
	}

	if (buf->readNum32() != 0x02030405) {
		error_exit("readNum32");
	}

	if (buf->bytesToRead() != (buf->length() - (sizeof(uint32_t) + sizeof(uint16_t)))) {
		error_exit("readNum32");
	}

	buf->reset();
	if (buf->length() != 0 || buf->bytesToWrite() != buf->size()) {
		error_exit("reset");
	}

	buf->writeNum16(0x0A0B);
	/* Now buf is 0x0A 0x0B */
	if (buf->readNum16() != 0x0A0B) {
		error_exit("writeNum16");
	}

	buf->writeNum32(0x0C0D0E0F);
	/* Now buf is 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F */
	if (buf->readNum32() != 0x0C0D0E0F) {
		error_exit("writeNum32");
	}

	buf->writeString(str);
	buf->readBinary((uint8_t *)str2, len, sizeof(str2));
	if (memcmp(str, str2, strlen(str)) != 0) {
		error_exit("readBinary");
	}

	delete buf;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "AppBuf");

	return 0;
}