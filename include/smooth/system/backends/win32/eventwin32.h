 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_EVENTWIN32_
#define _H_OBJSMOOTH_EVENTWIN32_

namespace smooth
{
	namespace System
	{
		class EventWin32;
	};
};

#include "../eventbackend.h"

namespace smooth
{
	namespace System
	{
		const Int	 EVENT_WIN32 = 1;

		class EventWin32 : public EventBackend
		{
			public:
					 EventWin32();
					~EventWin32();

				Int	 ProcessNextEvent(Bool);
		};
	};
};

#endif
