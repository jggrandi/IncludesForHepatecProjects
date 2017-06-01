/*

Imebra v. 0.0.48

Imebra: a C++ dicom library.
Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009  by Paolo Brandoli

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU AFFERO GENERAL PUBLIC LICENSE Version 3 for more details.

You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3
 along with this program; If not, see http://www.gnu.org/licenses/

-------------------

If you want to use Imebra commercially then you have to buy the commercial
 license available at http://puntoexe.com
 
After you buy the commercial license then you can use Imebra according
 to the terms described in the Imebra Commercial License Version 1.
A copy of the Imebra Commercial License Version 1 is available in the 
 documentation pages.

Imebra is available at http://puntoexe.com

The author can be contacted by email at paolo@puntoexe.com or by mail at
 the following address:
 Paolo Brandoli
 Preglov trg 6
 1000 Ljubljana
 Slovenia


*/

/*! \file dataHandler.cpp
    \brief Implementation of the base class for the data handlers.

*/

#include "../../base/include/exception.h"
#include "../include/dataHandler.h"

namespace puntoexe
{

namespace imebra
{

namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// imebraDataHandler
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Disconnect the handler
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool dataHandler::preDelete()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::preDelete");

	if(!m_bCommitted)
	{
		lockObject lockAccess(m_buffer.get());

		copyBack();
		commit();
	}
	return true;

	PUNTOEXE_FUNCTION_END();
}


void dataHandler::copyBack()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::copyBack");

	if(m_buffer == 0)
	{
		return;
	}
	m_buffer->copyBack(this);

	PUNTOEXE_FUNCTION_END();
}

void dataHandler::commit()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::copyBack");

	if(m_buffer == 0)
	{
		return;
	}

	m_buffer->commit();
	m_bCommitted = true;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Discard all the changes made on a writing handler
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::abort()
{
	m_buffer.release();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Increase the data pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::skip(const int skip)
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::skip");

	for( int skipCount(skip); skipCount != 0; --skipCount)
	{
		incPointer();
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve an element's size
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandler::getUnitSize() const
{
	return 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the padding byte
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint8 dataHandler::getPaddingByte() const
{
	return 0;
}



///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the data element as a signed long and increase
//  the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 dataHandler::getSignedLongIncPointer()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::getSignedLongIncPointer");

	imbxInt32 value=getSignedLong();
	incPointer();
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the data element as an unsigned long and
//  increase the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandler::getUnsignedLongIncPointer()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::getUnsignedLongIncPointer");

	imbxUint32 value=getUnsignedLong();
	incPointer();
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the data element as a double and increase
//  the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
double dataHandler::getDoubleIncPointer()
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::getDoubleIncPointer");

	double value=getDouble();
	incPointer();
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the data element as a signed long and increase the
//  data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::setSignedLongIncPointer(const imbxInt32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::setSignedLongIncPointer");

	setSignedLong(value);
	incPointer();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the data element as an unsigned long and increase
//  the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::setUnsignedLongIncPointer(const imbxUint32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::setUnsignedLongIncPointer");

	setUnsignedLong(value);
	incPointer();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the data element as a double and increase the data
//  element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::setDoubleIncPointer(const double value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::setDoubleIncPointer");

	setDouble(value);
	incPointer();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the data 's type
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataHandler::getDataType() const
{
	return m_bufferType;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Parse the buffer's content
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::parseBuffer(const imbxUint8* pBuffer, const imbxUint32 bufferLength)
{
	PUNTOEXE_FUNCTION_START(L"dataHandler::parseBuffer");

	ptr<memory> tempMemory(memoryPool::getMemoryPool()->getMemory(bufferLength));
	if(pBuffer && bufferLength)
	{
		tempMemory->assign(pBuffer, bufferLength);
	}
	parseBuffer(tempMemory);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the date
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::getDate(
		imbxInt32* pYear, 
		imbxInt32* pMonth, 
		imbxInt32* pDay, 
		imbxInt32* pHour, 
		imbxInt32* pMinutes,
		imbxInt32* pSeconds,
		imbxInt32* pNanoseconds,
		imbxInt32* pOffsetHours,
		imbxInt32* pOffsetMinutes) const
{
	*pYear = 0;
	*pMonth = 0;
	*pDay = 0;
	*pHour = 0;
	*pMinutes = 0;
	*pSeconds = 0;
	*pNanoseconds = 0;
	*pOffsetHours = 0;
	*pOffsetMinutes = 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the date
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandler::setDate(
		imbxInt32 /* year */, 
		imbxInt32 /* month */, 
		imbxInt32 /* day */, 
		imbxInt32 /* hour */, 
		imbxInt32 /*minutes */,
		imbxInt32 /*seconds */,
		imbxInt32 /*nanoseconds */,
		imbxInt32 /*offsetHours */,
		imbxInt32 /*offsetMinutes */)
{
	return;
}

void dataHandler::setCharsetsList(charsetsList::tCharsetsList* /* pCharsetsList */)
{
	// Intentionally empty
}

void dataHandler::getCharsetsList(charsetsList::tCharsetsList* /* pCharsetsList */) const
{
	// Intentionally empty
}


} // namespace handlers

} // namespace imebra

} // namespace puntoexe
