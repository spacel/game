#include <iostream>
#include "networkprotocol.h"

namespace spacel {
namespace engine {
namespace network {

NetworkPacket::NetworkPacket(const uint16_t o): Deserializer(2)
{
	WriteUShort(o);
}

unsigned NetworkPacket::Read(void *dest, unsigned size)
{
	if (size + position_ > size_) {
		size = size_ - position_;
	}

	if (!size) {
		return 0;
	}

	unsigned char *srcPtr = &buffer_[position_];
	unsigned char *destPtr = (unsigned char *)dest;
	position_ += size;

	unsigned copySize = size;
	while (copySize >= sizeof(unsigned)) {
		memmove(destPtr, srcPtr, sizeof(unsigned));
		srcPtr += sizeof(unsigned);
		destPtr += sizeof(unsigned);
		copySize -= sizeof(unsigned);
	}

	if (copySize & sizeof(unsigned short)) {
		memmove(destPtr, srcPtr, sizeof(unsigned short));
		srcPtr += sizeof(unsigned short);
		destPtr += sizeof(unsigned short);
	}

	if (copySize & 1) {
		memmove(destPtr, srcPtr, 1);
	}

	return size;
}

// Opcode is stored at the beginning of the packet, go to begin, read and return back to
// position_
const uint16_t NetworkPacket::GetOpcode()
{
	unsigned saved_position = position_;
	Seek(0);
	uint16_t opcode = ReadUShort();
	Seek(saved_position);
	return opcode;
}

unsigned NetworkPacket::Seek(unsigned position)
{
	if (position > size_)
		position = size_;

	position_ = position;
	return position_;
}

unsigned NetworkPacket::Write(const void *data, unsigned size)
{
	if (size + position_ > buffer_.size()) {
		buffer_.resize(size + position_);
		size_ = size + position_;
	}

	unsigned char *srcPtr = (unsigned char *)data;
	unsigned copySize = size;

	while (copySize >= sizeof(unsigned)) {
		memmove(&buffer_[position_], srcPtr, sizeof(unsigned));
		srcPtr += sizeof(unsigned);
		copySize -= sizeof(unsigned);
		position_ += sizeof(unsigned);
	}

	if (copySize & sizeof(unsigned short)) {
		memmove(&buffer_[position_], srcPtr, sizeof(unsigned short));
		srcPtr += sizeof(unsigned short);
		position_ += sizeof(unsigned short);
	}

	if (copySize & 1) {
		memmove(&buffer_[position_], srcPtr, 1);
		position_++;
	}

	return size;
}

}
}
}
