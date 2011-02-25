//Copyright (C) 2009-2010  Darron Baida and Patrick J. McCarty.
//Licensed under X11 License. See LICENSE.txt for details.

/*! @file
    Implements basic master-only support for the I2C (Inter-Integrated Circuit) bus, also known as TWI (Two-Wire Interface).

    This library provides a set of functions to make it easier to communicate with
    I2C/TWI slave devices, and is intended to cover a large variety of devices. These
    functions are currently all implemented in the least-efficient fashion, using
    blocking while-loops that wait for flags to be set. They should be sufficient
    for most simple applications, but if you need the best possible performance you
    should write I2C interrupt handlers or a non-blocking task loop instead.
    However, even if you choose to implement a better alternative, these functions
    should provide some hints about how to write I2C code, though you should also
    refer to the datasheet for your ATmega chip. Please be aware that due to the
    blocking nature of this implementation, it is possible for these functions to
    get stuck waiting for a flag condition that may never occur, effectively
    halting your program. This can happen if the I2C device is not communicating
    properly.

    To decide which function is appropriate for your I2C device, read the datasheet
    for the device to see if it uses addressable registers. If not, then you can
    simply read and write to the device using:
    i2cReadByteFromDevice() and i2cSendByteToDevice() or i2cSend2BytesToDevice().

    If your device uses 1-byte addressable registers, use:
    i2cSendDataToRegisters() and i2cReadDataFromRegisters().

    If your device uses 2-byte addressable registers, use:
    i2cSendDataToRegisters2() and i2cReadDataFromRegisters2().

    It is recommended to pass in the I2C slave device address with the read/write bit (LSB)
    set to 0 (an even number). However the functions will adjust the read/write bit as necessary.
*/

#include "globals.h"
#include <util/twi.h>

/*! Initialize I2C clock rate.
    Normally called only by the initialize() function in utility.c.
 */
inline void i2cInit()
{
	//Configure TWI Prescaler bits in TWI Status Register to yield a prescaler value of 4.
	cbi(TWSR, TWPS1);
	sbi(TWSR, TWPS0);

	//set I2C clock (drives SCL pin at this rate when operating as master)
	TWBR = 18; //100kHz
	//TWBR = 3; //400kHz
}

/*! Issues a START condition to initiate transmission to a slave device.
    @return Status with prescaler bits masked out.
 */
u08 i2cStart()
{
	//Set TWSTA to issue a START condition, clear TWINT flag (by writing 1 to it) to start operation.
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);
	//Wait for TWINT flag to be set.
	while (!(TWCR & (1<<TWINT)));
	//Check value of TWI Status Register. Mask prescaler bits. Return status.
	return (TWSR & 0xF8);
}

//! Issues a STOP condition to finalize transmission to a slave device.
inline void i2cStop()
{
	//Set TWSTO to issue a STOP condition, clear TWINT flag (by writing 1 to it) to start operation.
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	//STOP condition has no status.
}

/*! Transmits the specified byte on the I2C bus.
    @param byteToSend The byte to transmit on the I2C bus.
    @return Status with prescaler bits masked out.
 */
u08 i2cByteTransmit(const u08 byteToSend)
{
	//Load the byte to be sent into the TWI Data Register.
	TWDR = byteToSend;
	//Clear TWINT flag (by writing 1 to it) to start transmission of byte.
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for TWINT flag to be set.
	while (!(TWCR & (1<<TWINT)));
	//Check value of TWI Status Register. Mask prescaler bits. Return status.
	return (TWSR & 0xF8);
}

/*! Receives one byte of data and sends an ACK acknowledgment bit.
    @param dataPtr Pointer to where the received data should be stored, if no error.
    @return FALSE = Error, TRUE = Success
 */
bool i2cByteReceiveACK(u08 *const dataPtr)
{
	//Send an ACK after the data is received.
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);

	//Wait for TWINT flag to be set.
	while (!(TWCR & (1<<TWINT)));

	//Check value of TWI Status Register. Mask prescaler bits. If status is not "data received, ACK returned" return error.
	if ((TWSR & 0xF8) != TW_MR_DATA_ACK)
		return FALSE;

	//No error, so store received data byte
	*dataPtr = TWDR;
	return TRUE;
}

/*! Receives one byte of data and sends a NACK acknowledgment bit.
    @param dataPtr Pointer to where the received data should be stored, if no error.
    @return FALSE = Error, TRUE = Success
 */
bool i2cByteReceiveNACK(u08 *const dataPtr)
{
	//Send a NACK after the data is received.
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait for TWINT flag to be set.
	while (!(TWCR & (1<<TWINT)));

	//Check value of TWI Status Register. Mask prescaler bits. If status is not "data received, NACK returned" return error.
	if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
		return FALSE;

	//No error, so store received data byte
	*dataPtr = TWDR;
	return TRUE;
}

/*! Transmits one byte of data to the specified I2C slave address.
    @param address The address of the slave device.
    @param data The data byte to send to the slave device.
    @return FALSE = Error, TRUE = Success
 */
bool i2cSendByteToDevice(const u08 address, const u08 data)
{
	u08 status;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Send data byte
	status = i2cByteTransmit(data);
	//If the status is not "data transmitted, ACK received " return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Transmits two bytes of data (a 16-bit value) to the specified I2C slave address.
    @param address The address of the slave device.
    @param data The two bytes to transmit, MSB first.
    @return FALSE = Error, TRUE = Success
 */
bool i2cSend2BytesToDevice(const u08 address, const u16 data)
{
	u08 status;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Transmit the upper 8 data bits.
	status = i2cByteTransmit((u08)(data >> 8));
	//If the status is not "data transmitted, ACK received " return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Transmit the lower 8 data bits.
	status = i2cByteTransmit((u08)data);
	//If the status is not "data transmitted, ACK received " return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Reads one byte of data from the specified I2C slave address.
    @param address The address of the slave device.
    @param dataPtr Pointer to where the received data should be stored, if no error.
    @return FALSE = Error, TRUE = Success
 */
bool i2cReadByteFromDevice(const u08 address, u08 *const dataPtr)
{
	u08 status;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit set to indicate a read.
	status = i2cByteTransmit(address | 0x01);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MR_SLA_ACK)
		return FALSE;

	//Return NACK because only one byte is being read.
	if (!i2cByteReceiveNACK(dataPtr))
		return FALSE;

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Writes data to an I2C slave device with one-byte internal register numbers.
    @param address The address of the slave device.
    @param reg The register number to write to.
    @param byteCount The number of data bytes to write.
    @param dataPtr Pointer to the first data byte to write into the register.
    @return FALSE = Error, TRUE = Success
 */
bool i2cSendDataToRegisters(const u08 address, const u08 reg, const u08 byteCount, const u08 *const dataPtr)
{
	u08 status, i;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Send the register number to write to
	status = i2cByteTransmit(reg);
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	for (i = 0; i < byteCount; i++)
	{
		status = i2cByteTransmit(dataPtr[i]);
		//If the status is not "data transmitted, ACK received" return error.
		if (status != TW_MT_DATA_ACK)
			return FALSE;
	}

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Reads data from an I2C slave device with one-byte internal register numbers.
    @param address The address of the slave device.
    @param reg The register number to read from.
    @param byteCount The number of data bytes to read. Must be at least 1.
    @param dataPtr Pointer to where the received data should be stored, if no error.
    @return FALSE = Error, TRUE = Success
 */
bool i2cReadDataFromRegisters(const u08 address, const u08 reg, const u08 byteCount, u08 *const dataPtr)
{
	u08 status, i;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Send the register number to read
	status = i2cByteTransmit(reg);
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send REPEATED START condition.
	status = i2cStart();
	//If the status is not "repeated start sent" return error.
	if (status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit set to indicate a read.
	status = i2cByteTransmit(address | 0x01);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MR_SLA_ACK)
		return FALSE;

	//Receive all but the last byte returning an ACK each time.
	for (i = 0; i < (byteCount-1); i++)
	{
		if (!i2cByteReceiveACK(dataPtr + i))
			return FALSE;
	}
	//Receive the last byte and return a NACK, to indicate that we are done reading.
	if (!i2cByteReceiveNACK(dataPtr + i))
		return FALSE;

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Writes data to an I2C slave device with two-byte internal register numbers.
    @param address The address of the slave device.
    @param reg The register number to write to.
    @param byteCount The number of data bytes to write.
    @param dataPtr Pointer to the first data byte to write into the register.
    @return FALSE = Error, TRUE = Success
 */
bool i2cSendDataToRegisters2(const u08 address, const u16 reg, const u08 byteCount, const u08 *const dataPtr)
{
	u08 status, i;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Send the upper 8 register bits.
	status = i2cByteTransmit((u08)(reg>>8));
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send the lower 8 register bits.
	status = i2cByteTransmit((u08)reg);
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	for (i = 0; i < byteCount; i++)
	{
		status = i2cByteTransmit(dataPtr[i]);
		//If the status is not "data transmitted, ACK received" return error.
		if (status != TW_MT_DATA_ACK)
			return FALSE;
	}

	//Send STOP condition.
	i2cStop();
	return TRUE;
}

/*! Reads data from an I2C slave device with two-byte internal register numbers.
    @param address The address of the slave device.
    @param reg The register number to read from.
    @param byteCount The number of data bytes to read. Must be at least 1.
    @param dataPtr Pointer to where the received data should be stored, if no error.
    @return FALSE = Error, TRUE = Success
 */
bool i2cReadDataFromRegisters2(const u08 address, const u16 reg, const u08 byteCount, u08 *const dataPtr)
{
	u08 status, i;
	//Send START condition.
	status = i2cStart();
	//If the status is not "start sent" or "repeated start sent" return error.
	if (status != TW_START && status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit cleared to indicate a write.
	status = i2cByteTransmit(address & 0xFE);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MT_SLA_ACK)
		return FALSE;

	//Send the upper 8 register bits.
	status = i2cByteTransmit((u08)(reg>>8));
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send the lower 8 register bits.
	status = i2cByteTransmit((u08)reg);
	//If the status is not "data transmitted, ACK received" return error.
	if (status != TW_MT_DATA_ACK)
		return FALSE;

	//Send REPEATED START condition.
	status = i2cStart();
	//If the status is not "repeated start sent" return error.
	if (status != TW_REP_START)
		return FALSE;

	//Send slave address with the R/W bit set to indicate a read.
	status = i2cByteTransmit(address | 0x01);
	//If the status is not "slave address + R/W bit transmitted, ACK received" return error.
	if (status != TW_MR_SLA_ACK)
		return FALSE;

	//Receive all but the last byte returning an ACK each time.
	for (i = 0; i < (byteCount-1); i++)
	{
		if (!i2cByteReceiveACK(dataPtr + i))
			return FALSE;
	}
	//Receive the last byte and return a NACK, to indicate that we are done reading.
	if (!i2cByteReceiveNACK(dataPtr + i))
		return FALSE;

	//Send STOP condition.
	i2cStop();
	return TRUE;
}
