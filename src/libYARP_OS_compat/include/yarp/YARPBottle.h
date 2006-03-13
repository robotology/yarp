///////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPBottle.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/// This code is based on the old YARPBottle class.
/// Note, this class has the same name but acts differently. The new YARPBottle
/// merges together the YARPBottle and the YARPBottleIterator classes.
/// To read messages within the bottle use the "tryRead" functions. Subsequent calls 
/// to tryRead return the same value, unless moveOn() is called.
/// moveOn() has effect only once after each tryRead and move the internal index right
/// after the data that were read by the last tryRead (if successful).
/// To avoid calling moveOn() use "read" methods.
///
/// July 2003 --by nat 

#ifndef YARPBOTTLE_INC
#define YARPBOTTLE_INC

/**
 * \file YARPBottle.h This is the definition of YARPBottle objects. These are
 * little containers for which a content type is provided.
 */
#include <assert.h>
#include <stdio.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>
#include <yarp/YARPBottleCodes.h>
#include <yarp/YARPString.h>
#include <yarp/YARPList.h>
#include <yarp/YARPNetworkTypes.h>

/**
 * YARPBottle is a container object that can be sent across the network (doesn't do
 * any marshaling though) and it can contain integers, doubles, etc. Provided you
 * know how you filled the bottle you can then unfill it in the same order. You can
 * use bottles to send messages with parameters.
 */
class YARPBottle
{
	/**
	 * Definition of the ID of the bottle. This is a little text that accompanies the
	 * bottle to identify the message type.
	 */
	struct BottleId
	{
		/**
		 * Constructor.
		 */
		BottleId()
		{
		//	text.resize(__maxBottleID);
		}

		/**
		 * Sets the bottle ID.
		 * @param s is the ID (a string).
		 */
		void set(const char *s)
		{ 
			ACE_OS::strncpy(text, s , __maxBottleID-1);
			text[__maxBottleID-1] = '\0';	// paranoid ?
			length = ACE_OS::strlen(text)+1;
		}

		/**
		 * Sets the bottle ID starting from a YBVocab which is a sort of YARPString.
		 * @param s is the string to copy text from.
		 */
		void set(const YBVocab &s)
		{
			set(s.c_str());
		}

		/**
		 * Comparison operator. Useful to check whether a received message has to 
		 * be processed.
		 * @param l is the reference to the object to compare with.
		 * @return true if the two ID's aren't the same.
		 */
		bool operator !=(const BottleId &l) const
		{
			return !operator==(l);
		}
		
		/**
		 * Comparison operator. Useful to check whether a received message has to 
		 * be processed.
		 * @param l is the reference to the object to compare with.
		 * @return true if the two ID's are the same.
		 */
		bool operator==(const BottleId &l) const
		{
			if (ACE_OS::strcmp(text, l.c_str()) == 0)
				return true;
			else
				return false;
		}

		/**
		 * Comparison operator. Useful to check whether a received message has to 
		 * be processed.
		 * @param l is the reference to an YBVocab to compare with.
		 * @return true if the two ID's aren't the same.
		 */
		bool operator !=(const YBVocab &l) const
		{
			return !operator==(l);
		}
		
		/**
		 * Comparison operator. Useful to check whether a received message has to 
		 * be processed.
		 * @param l is the reference to an YBVocab to compare with.
		 * @return true if the two ID's are the same.
		 */
		bool operator==(const YBVocab &l) const
		{
			if (ACE_OS::strcmp(text, l.c_str()) == 0)
				return true;
			else
				return false;
		}

		/**
		 * Similar to YARPString, gets the ID text.
		 * @return the ID of the bottle as text.
		 */
		const char *c_str() const
		{
			return text;
		}

		char text[__maxBottleID];
		NetInt32 length;
	};

public:
	/**
	 * Default constructor.
	 */
	YARPBottle();

	/**
	 * Destructor.
	 */
	virtual ~YARPBottle() {}

	/**
	 * Copy operator for YARPBottle.
	 * @param bottle is a reference to the object to copy from.
	 */
	YARPBottle& operator = (const YARPBottle& bottle)
	{
	  id.set(bottle.id.text);
      top = bottle.top;
	  text = bottle.text;
	  lastReadSeq = bottle.lastReadSeq;
	  index = bottle.index;
	  return *this;
	}
   
	/**
	 * Gets the internal buffer.
	 * @return a reference to the internal buffer which is a YARPVector.
	 */
	YARPVector<char>& getBuffer() { return text; }

	/**
	 * Sets the ID starting from a YBVocab reference.
	 * @param l is the reference to the YBVocab object.
	 */
	void setID(const YBVocab &l) { id.set(l.c_str()); }

	/**
	 * Gets the ID of the bottle.
	 * @return the reference to the internal ID variable.
	 */
	const BottleId &getID() const { return id; }

	/**
	 * Gets the size of the bottle in bytes.
	 * @return the size of the internal vector in bytes.
	 */
	int getSize() const { return text.size(); }

	/**
	 * Returns the position of the last used byte of the array.
	 * @return the last used position within the vector of char.
	 */
	int getTopInBytes() const { return top; }

	/**
	 * Gets a pointer to the data.
	 * @return a pointer to the internal buffer of char.
	 */
	const char *getDataPtr() const { return &text[0]; } 

	/**
	 * Sets the pointer to the data to a externally provided buffer.
	 * @param buf is the pointer to the buffer.
	 * @param l is the length of the buffer in bytes.
	 */
	void setDataPtr(char *buf, int l) { writeRawBlock(buf, l); }
  
	/**
	 * Writes an integer to the buffer. Consecutive writes add data in the buffer.
	 * @param result is the integer to add to the buffer.
	 */
	void writeInt(NetInt32 result)
		{ writeRawInt(YBTypeInt);  writeRawInt(result); }

	/**
	 * Writes a double precision value to the buffer. 
	 * Consecutive writes add data in the buffer.
	 * @param result is the value to add to the buffer.
	 */
	void writeFloat(double result)
		{ writeRawInt(YBTypeDouble);  writeRawFloat(result); }

	/**
	 * Writes a vector of double precision values to the buffer.
	 * Consecurive writes add data in the buffer.
	 * @param *v is the pointer to the vector of values.
	 * @param n is the length of the vector.
	 */
	void writeDoubleVector(double *v, int n)
	  {
	    writeRawInt(YBTypeDoubleVector);
	    writeRawInt(n);
	    int i;
	    for(i=0; i<n; i++)
	      writeRawFloat(v[i]);
	  }
	
        /**                                                                                    
         * Writes a vector of integers to the buffer.                           
         * Consecurive writes add data in the buffer.                                          
         * @param *v is the pointer to the vector of values.                                   
         * @param n is the length of the vector.                                               
         */
        void writeIntVector(int *v, int n)
          {
            writeRawInt(YBTypeIntVector);
            writeRawInt(n);
            int i;
            for(i=0; i<n; i++)
              writeRawInt(v[i]);
          }

	/**
	 * Writes a string (zero terminated) into the buffer. 
	 * Consecutive writes add data in the buffer.
	 * @param result is the string to add to the buffer.
	 */
	void writeText(const char *result)
		{ writeRawInt(YBTypeString);  writeRawText(result); }

	/**
	 * Writes a YBVocab into the buffer.
	 * @param result is the reference to the YBVocab to add.
	 */
	void writeVocab(const YBVocab &result)
	{
		writeRawInt(YBTypeVocab);
		writeRawText(result.c_str());
	}

	/**
	 * Peeks an integer from the bottle.
	 * The rationale is to return true if the type of the next 
	 * variable in the bottle matches the one of the function you call, 
	 * otherwise return false. The output parameter in this case is undefined
	 * (i.e. don't rely on its value).
	 * The internal "index" is not changed, that is the next "tryRead" will occur
	 * on the same data, unless the moveOn() function is called.
	 * This semantic is similar to any peek function when reading a queue.
	 * @param i is a pointer to the value read from the bottle.
	 * @return true if something has been read, false otherwise and @a i is
	 * undefined.
	 */
	bool tryReadInt(int *i)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeInt))
			return false;
		index+=sizeof(YBTypeInt);
		*i = (int) readRawInt();
		index = oldIndex;
		return true;
	}

	/**
	 * Peeks a double precision value from the bottle.
	 * @param f is a pointer to the value read from the bottle.
	 * @return true if something has been read, false otherwise and @a i is
	 * undefined.
	 */
	bool tryReadFloat(double *f)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeDouble))
			return false;
		index+=sizeof(YBTypeDouble);
		*f = readRawFloat();
		index = oldIndex;
		return true;
	}

	/**
	 * Peeks a string from the bottle.
	 * @param s is a pointer to the string read from the bottle.
	 * @return true if something has been read, false otherwise and @a i is
	 * undefined.
	 */
	bool tryReadText(char *s)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeString))
			return false;
		index+=sizeof(YBTypeString);
		readRawText(s);
		index = oldIndex;
		return true;
	}

	/**
	 * Peeks a string from the bottle without checking whether it's been read.
	 * @return the pointer to the string read from the bottle, NULL if
	 * the string can't be read at the current position.
	 */
	const char *tryReadText()
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeString))
			return NULL;
		index+=sizeof(YBTypeString);
		const char *addr = readRawText();
		index = oldIndex;
		assert(addr!=NULL);
		return addr;
	}

	/**
	 * Peeks a YBVocab from the bottle.
	 * @param v is the reference to the YBVocab.
	 * @return true if the return value has been assigned, false otherwise.
	 */
	bool tryReadVocab(YBVocab &v)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeVocab))
			return false;
		index+=sizeof(YBTypeVocab);
		v = YBVocab(readRawText());
    	index = oldIndex;
		return true;
	}

	/**
	 * Reads an integer from the bottle. It tries reading and if successful
	 * updates the internal pointer to the next element in the bottle.
	 * @param v is a pointer to the read value.
	 * @return true if something has been read and the value assigned to @a v, 
	 * false otherwise.
	 */
	bool readInt(int *v)
	{
		if (tryReadInt(v))
		{
			moveOn();
			return true;
		}
		else
			return false;
	}

	/**
	 * Reads an integer without checking whether it can be read.
	 * @return the read value, 0 if it can't read from the current position. 0 is
	 * not discriminative of a failure.
	 */
	int readInt() {
	  int v = 0;
	  readInt(&v);
	  return v;
	}
	
	/**
	 * Reads a vector of double precision floating point from the bottle.
	 * @param v is a pointer to the vector that will be filled.
	 * @param n is the expected size of the vector.
	 * @return true if the bottle contains a vector of size n at the current
	 * position, false otherwise.
	 */
	bool readDoubleVector(double *v, int n)
	  {
	    int oldIndex = index;
	    lastReadSeq = 0;
	    if (!assertType(YBTypeDoubleVector))
	      return false;
	    index += sizeof(YBTypeDoubleVector);
	    int tmpI = readRawInt();
	    index += sizeof(int);
	    if (n!=tmpI)
	      {
		index = oldIndex;
		return false;
	      }
	    tmpI = n*sizeof(double);
	    ACE_OS::memcpy((char *) v, readRawBlock(tmpI), tmpI);
	    index+=tmpI;
	    return true;
	  }

	/**                                                                                          
         * Reads a vector of integers from the bottle.                        
         * @param v is a pointer to the vector that will be filled.                                  
         * @param n is the expected size of the vector.                                              
         * @return true if the bottle contains a vector of size n at the current                     
         * position, false otherwise.                                                                
         */
        bool readIntVector(int *v, int n)
          {
            int oldIndex = index;
            lastReadSeq = 0;
            if (!assertType(YBTypeIntVector))
              return false;
            index += sizeof(YBTypeIntVector);
            int tmpI = readRawInt();
            index += sizeof(int);
            if (n!=tmpI)
              {
                index = oldIndex;
                return false;
              }
            tmpI = n*sizeof(int);
	    ACE_OS::memcpy((char *) v, readRawBlock(tmpI), tmpI);
            index+=tmpI;
            return true;
          }

	/**
	 * Reads a double precision floating point value from the bottle. 
	 * It tries reading and if successful
	 * updates the internal pointer to the next element in the bottle.
	 * @param v is a pointer to the read value.
	 * @return true if something has been read and the value assigned to @a v, 
	 * false otherwise.
	 */
	bool readFloat(double *v)
	{
		if (tryReadFloat(v))
		{
			moveOn();
			return true;
		}
		else
			return false;
	}
	
	/**
	 * Reads a string from the bottle.
	 * Yikes! no way to bound size of buffer -
	 * invitation to disaster.
	 * @param s is a pointer to the buffer that will contain the string.
	 * @return true if something has been read and the value assigned to @a s, 
	 * false otherwise.
	 */
	bool readText(char *s)
	{
		if (tryReadText(s))
		{
			moveOn();
			return true;
		}
		else
			return false;
	}

	/**
	 * Reads a string from the bottle.
	 * @return the string from the bottle, NULL if it can't be read in the current 
	 * position.
	 */
	const char *readText()
	{
	  const char *addr = tryReadText();
	  if (addr!=NULL) {
	    moveOn();
	  }
	  return addr;
	}

	/**
	 * Moves the internal pointer to the beginning of the bottle.
	 */
	void rewind()
	{ index = 0;}

	/**
	 * Empties the bottle.
	 */
	void reset()
	{
		rewind();
		top = 0;
	}
  
	/**
	 * Checks whether there's more to read from the bottle.
	 * @return 1 if there's more to be read, 0 otherwise.
	 */
	int more() const
	{
	  return (index<top-1);
	}

	/**
	 * Increases the "index". The next "tryRead" will work on the remainder of
	 * the buffer. Subsequent calls to moveOn() do nothing unless another
	 * tryRead has been called before.
	 */
	void moveOn()
	{
		_moveOn(lastReadSeq);
		lastReadSeq = 0;
	}

	// internal move on.
	void _moveOn(int l)
	{
		index += l;
		if (index >top)
			index = top;
	}
	
	/**
	 * Prints the bottle to stdout (no formatting).
	 */
	void dump();

	/**
	 * Prints the bottle to stdout (formatted output).
	 */
	virtual void display();

protected:
	char *readRawBlock(int len)
	{
		assert(getSize()-index>=len);
		char *result = (&(text)[index]);
		lastReadSeq += len;
		return result;
    }

	void readRawBlock(char *buf, int len)
    {
		char *src = readRawBlock(len);
		ACE_OS::memcpy(buf,src,len);
    }
 
	void writeRawBlock(char *buf, int l)
    {
		text.resize(index+l);
		ACE_OS::memcpy(&text[index],buf,l);
		index+=l;
		top = index;
		len = top;
    }
  
	char readRawChar()
    {
		char result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }
	
	void writeRawChar(char result)
	{
		writeRawBlock((char*)(&result),sizeof(result));
	}

	NetInt32 readRawInt()
    {
		NetInt32 result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }

	void writeRawInt(NetInt32 result)
    {
		writeRawBlock((char*)(&result),sizeof(result));
    }

	double readRawFloat()
    {
		double result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }

	void writeRawFloat(double result)
    {
		writeRawBlock((char*)(&result),sizeof(result));
    }
  
	const char *readRawText()
    {
		NetInt32 len = readRawInt();
		index += sizeof(NetInt32);
		const char *tmp = readRawBlock(len);
		index -= sizeof(NetInt32);
		return tmp;
    }
	
	void readRawText(char *s)
    {
      //printf("*** please supply a buffer size to YARPBottle::readRawText and related fns\n");
		NetInt32 len = readRawInt();
		index += sizeof(int);
		ACE_OS::memcpy(s, readRawBlock(len), len);
		index -= sizeof(int);
    }

	void writeRawText(const char *text)
    {
		writeRawInt(ACE_OS::strlen(text)+1);
		writeRawBlock((char*)(&text[0]),ACE_OS::strlen(text)+1);      
    }
	
	bool assertType(int ch)
    {
		// first of all check if bottle is !empty
		if (!more())
			return false;
		int compare;
		compare = readRawInt();
		return (ch==compare);
    }

	YARPVector<char> text;
	BottleId id;
	NetInt32 len;
	NetInt32 top;
	int index;
	// this is used to "remove" the last read sequence from bottle
	int lastReadSeq;
};

// circular dependency for now - don't want end user to forget
// about the Content.  This can be a very hard bug to track down.
#include <yarp/YARPBottleContent.h>

#endif

