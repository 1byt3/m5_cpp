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

#ifndef __PKT_UNSUBSCRIBE_HPP__
#define __PKT_UNSUBSCRIBE_HPP__

#include "Properties.hpp"
#include "Packet.hpp"
#include "Common.hpp"

#include <list>

namespace m5 {

class PktUnsubscribe : public Packet
{
private:
	std::list<ByteArray *> _topics;
	uint32_t payloadSize = 0;
	uint16_t _packetId = 0;

public:
	PktUnsubscribe() {}
	PktUnsubscribe(AppBuf &buf);
	virtual ~PktUnsubscribe();

	void packetId(uint16_t id) { this->_packetId = id; }
	uint16_t packetId(void) { return this->_packetId; }

	void append(const char *str);
	const std::list<ByteArray *> &topics() const { return _topics; }

	uint32_t writeTo(AppBuf &buf) override;
	uint32_t readFrom(AppBuf &buf) override;
	uint32_t getId(void) const override { return (uint32_t)PktType::UNSUBSCRIBE; }
};

}

#endif

