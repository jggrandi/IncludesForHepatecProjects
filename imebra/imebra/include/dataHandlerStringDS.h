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

/*! \file dataHandlerStringDS.h
    \brief Declaration of the class dataHandlerStringDS.

*/

#if !defined(imebraDataHandlerStringDS_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
#define imebraDataHandlerStringDS_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_

#include "dataHandlerString.h"


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
/// \brief Handles the Dicom type "DS" (decimal string)
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dataHandlerStringDS : public dataHandlerString
{
public:
	// Overwritten to use getDouble()
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLong() const;

	// Overwritten to use getDouble()
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLong() const;

	// Overwritten to use setDouble()
	///////////////////////////////////////////////////////////
	virtual void setSignedLong(const imbxInt32 value);

	// Overwritten to use setDouble()
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLong(const imbxUint32 value);

	// Overwritten to use the exponential form if needed
	///////////////////////////////////////////////////////////
	virtual void setDouble(const double value);

	// Get the padding byte
	///////////////////////////////////////////////////////////
	virtual imbxUint8 getPaddingByte() const;

	// Get the element size
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnitSize() const;

protected:
	// Return the maximum string's length
	///////////////////////////////////////////////////////////
	virtual imbxUint32 maxSize() const;

};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerStringDS_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)