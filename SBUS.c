
void SBUS_Decode(volatile uint8_t *sbus, volatile uint16_t *channel, uint8_t size)
{
	uint8_t bits = 0;
	uint8_t channelIndex = 0;
	uint32_t buffer = 0;

	for( int index = 1; index < size; index += 2)
	{
		buffer |= sbus[index]<<bits;
		bits += 8;
		buffer |= sbus[index+1]<<bits;
		channel[channelIndex++] = buffer & 0x07FF;
		buffer >>= 11;
		if( (bits -= 3) >= 11)
		{
			channel[channelIndex++] = buffer & 0x07FF;
			buffer >>= 11;
			bits -= 11;

		}
	}
}


void SBUS_Encode(volatile uint16_t* channel, uint8_t size, volatile uint8_t *sbus)
{
	uint8_t bits = 0;	// bit counter for staging buffer.
	uint8_t busIndex = 1;	// current bucket being assigned
	uint32_t buffer = 0;	// staging buffer of bits, will append 11 bits per channel.

	sbus[0] = 0x0f;		// set SBUS flag (0x0F).

	for (int i = 0; i < size; i++)
	{
		buffer |= (channel[i] & 0x07ff)<<bits;	// Append 11 bits of channel data MSB.
		sbus[busIndex++] = buffer & 0xff;	// assign 8 LSB to sbus bucket byte.
		buffer >>= 8;				// Shift 8 bits to the right, those bits were assigned to sbus bucket.
		if ((bits += 3) >= 9)			// Increment bit count in buffer of bits and compare to 9.
		{										//
			sbus[busIndex++] = buffer & 0xff;// assign 8 bits to next sbus bucket to prevent overflow.
			buffer >>= 8;			 // Shift 8 bits to the right, those bits were assigned to sbus bucket.
			bits -= 8;			 // decrement bit count in buffer of bits.
		}

	}
	sbus[busIndex++] = buffer & 0xff;		// assign left over bits to next two buckets
	sbus[busIndex] = (buffer>>8) & 0xff;

	sbus[23] = 0;					//assign las 2 bytes incase they were assigned by mistake.
	sbus[24] = 0;

}
