 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MUTEXSDL
#define H_OBJSMOOTH_MUTEXSDL

namespace smooth
{
	namespace Threads
	{
		class MutexSDL;
	};
};

#include "../mutexbackend.h"

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

namespace smooth
{
	namespace Threads
	{
		const Short	 MUTEX_SDL	= 3;

		class MutexSDL : public MutexBackend
		{
			protected:
				SDL_mutex	*mutex;
				Bool		 myMutex;
			public:
						 MutexSDL(Void * = NIL);
						~MutexSDL();

				Void		*GetSystemMutex() const;

				Int		 Lock();
				Int		 Release();
		};
	};
};

#endif
