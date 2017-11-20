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
 * MQTT 5 Low Level Packet Library
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

#include "PktPublish.hpp"

#include <cstring>
#include <cerrno>

namespace m5 {

PktPublish::PktPublish() : Packet(PktType::PUBLISH, 0x00)
{
}

PktPublish::~PktPublish()
{
}

void PktPublish::topic(const uint8_t *data, uint16_t size)
{
	if (data == nullptr || size < 1) {
		throw std::invalid_argument("Invalid input buffer");
	}

	this->_topic.assign(data, data + size);
}

void PktPublish::topic(const char *str)
{
	topic((const uint8_t *)str, strlen(str));
}

void PktPublish::payload(const uint8_t *data, uint16_t size)
{
	if (data == nullptr || size < 1) {
		return;
	}

	this->_payload.assign(data, data + size);
}

void PktPublish::payloadFormatIndicator(bool v)
{
	properties.payloadFormatIndicator(v);
}

bool PktPublish::payloadFormatIndicator(void) const
{
	return properties.payloadFormatIndicator();
}

void PktPublish::publicationExpiryInterval(uint32_t v)
{
	properties.publicationExpiryInterval(v);
}

uint32_t PktPublish::publicationExpiryInterval(void) const
{
	return properties.publicationExpiryInterval();
}

void PktPublish::contentType(const uint8_t *data, uint16_t size)
{
	properties.contentType(data, size);
}

void PktPublish::contentType(const char *str)
{
	properties.contentType(str);
}

const ByteArray &PktPublish::contentType(void) const
{
	return properties.contentType();
}

void PktPublish::responseTopic(const uint8_t *data, uint16_t size)
{
	properties.responseTopic(data, size);
}

void PktPublish::responseTopic(const char *str)
{
	properties.responseTopic(str);
}

const ByteArray &PktPublish::responseTopic(void) const
{
	return properties.responseTopic();
}

void PktPublish::subscriptionIdentifier(uint32_t v)
{
	properties.subscriptionIdentifier(v);
}

uint32_t PktPublish::subscriptionIdentifier(void) const
{
	return properties.subscriptionIdentifier();
}

void PktPublish::correlationData(const uint8_t *data, uint16_t size)
{
	properties.correlationData(data, size);
}

const ByteArray &PktPublish::correlationData(void) const
{
	return properties.correlationData();
}

void PktPublish::topicAlias(uint16_t v)
{
	properties.topicAlias(v);
}

uint16_t PktPublish::topicAlias(void) const
{
	return properties.topicAlias();
}

void PktPublish::userProperty(const uint8_t *key, uint16_t keySize,
			      const uint8_t *value, uint16_t valueSize)
{
	properties.userProperty(key, keySize, value, valueSize);
}

void PktPublish::userProperty(const char *key, const char *val)
{
	properties.userProperty(key, val);
}

const UserProperty &PktPublish::userProperty(void) const
{
	return properties.userProperty();
}

uint8_t PktPublish::headerFlags(void)
{
	uint8_t flags = 0;

	flags += this->dup() ? 1 << 3 : 0;
	flags += ((uint8_t)this->QoS() & 0x03) << 1;
	flags += this->retain() ? 1 : 0;

	return flags;
}

enum StatusCode PktPublish::writeVariableHeader(AppBuf &buf)
{
	buf.writeBinary(this->topic());
	if (this->QoS() != PktQoS::QoS0) {
		buf.writeNum16(this->packetId());
	}

	if (properties.write(buf) == 0) {
		return StatusCode::PROPERTY_WRITE_ERROR;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktPublish::writePayload(AppBuf &buf)
{
	if (this->payload().size() > 0) {
		buf.write(this->payload().data(), this->payload().size());
	}

	return StatusCode::SUCCESS;
}

uint32_t PktPublish::writeTo(AppBuf &buf)
{
	if (this->QoS() != PktQoS::QoS0 && this->packetId() == 0) {
		throw std::out_of_range("Invalid packet Id for this QoS level");
	}

	if (this->topic().size() == 0) {
		throw std::out_of_range("Invalid topic name");
	}

	Packet::hasProperties = true;
	Packet::fixedHeaderReserved = headerFlags();
	Packet::variableHeaderSize = stringLenSize + topic().size();
	if (this->QoS() != PktQoS::QoS0) {
		Packet::variableHeaderSize += 2;
	}
	Packet::payloadSize = this->payload().size();

	return Packet::writeTo(buf);
}

enum StatusCode PktPublish::fixedHeaderFlags(uint8_t flags)
{
	this->retain(flags & 0x01);
	this->QoS((PktQoS)((flags & 0x06) >> 1));
	this->dup(flags & 0x08);

	if ((int)this->QoS() >= 0x03) {
		return StatusCode::INVALID_QOS;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktPublish::readVariableHeader(AppBuf &buf)
{
	StatusCode rc;

	rc = buf.readBinary(this->_topic);
	if (rc != StatusCode::SUCCESS) {
		return rc;
	}

	if (this->topic().size() == 0) {
		return StatusCode::INVALID_TOPIC_NAME;
	}

	if(this->QoS() != PktQoS::QoS0) {
		if (buf.bytesToRead() < 2) {
			return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
		}

		this->packetId(buf.readNum16());
		if (this->packetId() == 0) {
			return StatusCode::INVALID_PACKET_ID;
		}
	}

	properties.read(buf);

	return StatusCode::SUCCESS;
}

enum StatusCode PktPublish::readPayload(AppBuf &buf)
{
	if (buf.bytesToRead() < payloadSize) {
		return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
	}

	if (payloadSize > 0) {
		buf.read(this->_payload, payloadSize);
	}

	return StatusCode::SUCCESS;
}

uint32_t PktPublish::readFrom(AppBuf &buf)
{
	Packet::minBufferSize = 6;
	Packet::minRemLen = 4;

	return Packet::readFrom(buf);
}

}

