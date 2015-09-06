 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_PROTOCOL_
#define H_BOCA_PROTOCOL_

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	enum MessageType
	{
		MessageTypeMessage = 0,
		MessageTypeWarning = 1,
		MessageTypeError   = 2
	};

	class BOCA_DLL_EXPORT Protocol
	{
		private:
			/* Managed class, therefore private constructor/destructor
			 */
			static Array<Protocol *>		 protocols;

			Threads::Mutex				 mutex;

			String					 name;
			UnsignedInt64				 startTicks;

			Array<String>				 messages;

			Array<String>				 warnings;
			Array<String>				 errors;

								 Protocol(const String &);
								~Protocol();
		public:
			Int					 Write(const String &, MessageType = MessageTypeMessage);

			/* Returns a new or existing instance of Protocol
			 */
			static Protocol				*Get(const String &);

			/* Destroys a given instance of Protocol
			 */
			static Bool				 Free(const String &);

			/* Returns all existing instances of Protocol
			 */
			static const Array<Protocol *>		&Get();

			/* Destroys all existing instances of Protocol
			 */
			static Void				 Free();
		accessors:
			const String				&GetName() const	{ return name; }
			String					 GetProtocolText() const;

			const Array<String>			&GetWarnings() const	{ return warnings; }
			const Array<String>			&GetErrors() const	{ return errors; }
		signals:
			static Signal0<Void>			 onUpdateProtocolList;
			static Signal1<Void, const String &>	 onUpdateProtocol;
	};
};

#endif
