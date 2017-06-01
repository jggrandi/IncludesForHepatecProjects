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

/*! \file dataHandlerString.h
    \brief Declaration of the base class used by the string handlers.

*/

#if !defined(imebraDataHandlerString_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
#define imebraDataHandlerString_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_

#include "dataHandler.h"
#include <vector>
#include <string>


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class for all the data handlers
///         that manage strings.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dataHandlerString : public dataHandler
{
public:
	// Constructor
	///////////////////////////////////////////////////////////
	dataHandlerString(): m_elementNumber(0){}

public:
	// Set the data element's pointer
	///////////////////////////////////////////////////////////
	virtual void setPointer(imbxUint32 const elementNumber);

	// Increase the data element's pointer
	///////////////////////////////////////////////////////////
	virtual void incPointer();

	// Returns true if the pointer is valid
	///////////////////////////////////////////////////////////
	virtual bool pointerIsValid() const;

	// Get the data element as a signed long
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLong() const;

	// Get the data element as an unsigned long
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLong() const;
	
	// Get the data element as a double
	///////////////////////////////////////////////////////////
	virtual double getDouble() const;

	// Get the data element as a string
	///////////////////////////////////////////////////////////
	virtual std::string getString() const;

	// Get the data element as an unicode string
	///////////////////////////////////////////////////////////
	virtual std::wstring getUnicodeString() const;

	// Retrieve the data element as a string
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getSize() const;
	
	// Set the data element as a signed long
	///////////////////////////////////////////////////////////
	virtual void setSignedLong(const imbxInt32 value);
	
	// Set the data element as an unsigned long
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLong(const imbxUint32 value);
	
	// Set the data element as a double
	///////////////////////////////////////////////////////////
	virtual void setDouble(const double value);
	
	// Set the data element as a string
	///////////////////////////////////////////////////////////
	virtual void setString(const std::string& value);

	// Set the data element as an unicode string
	///////////////////////////////////////////////////////////
	virtual void setUnicodeString(const std::wstring& value);
	
	// Set the buffer's size, in data elements
	///////////////////////////////////////////////////////////
	virtual void setSize(const imbxUint32 elementsNumber);

	// Parse the buffer
	///////////////////////////////////////////////////////////
	virtual void parseBuffer(const ptr<memory>& memoryBuffer);

	// Build the buffer
	///////////////////////////////////////////////////////////
	virtual void buildBuffer(const ptr<memory>& memoryBuffer);

	// Copy the content of an array of imbxInt32 values
	//         into the buffer controlled by the handler.
	///////////////////////////////////////////////////////////
	virtual void copyFromInt32(const imbxInt32* pSource, const imbxUint32 length);

	// Copy the content of the buffer to an array of imbxInt32
	//         values.
	///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////
	virtual void copyToInt32(imbxInt32* pDest, const imbxUint32 length) const;


protected:
	// Convert a string to unicode, without using the dicom 
	//  charsets
	///////////////////////////////////////////////////////////
	virtual std::wstring convertToUnicode(const std::string& value) const;

	// Convert a string from unicode, without using the dicom 
	//  charsets
	///////////////////////////////////////////////////////////
	virtual std::string convertFromUnicode(const std::wstring& value, charsetsList::tCharsetsList* pCharsetsList) const;

	// Return the maximum string's length
	///////////////////////////////////////////////////////////
	virtual imbxUint32 maxSize() const;

	// Return the separator
	///////////////////////////////////////////////////////////
	virtual wchar_t getSeparator() const;

	imbxUint32 m_elementNumber;

	std::vector<std::wstring> m_strings;

};


} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerString_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
