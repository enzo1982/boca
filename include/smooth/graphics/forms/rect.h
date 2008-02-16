 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_RECT_
#define _H_OBJSMOOTH_RECT_

namespace smooth
{
	namespace GUI
	{
		class Rect;
	};
};

#include "form.h"
#include "point.h"
#include "size.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Rect : public Form
		{
			constants:
				static Int	 Outlined;
				static Int	 Filled;
				static Int	 Rounded;
				static Int	 Inverted;
				static Int	 Dotted;
			public:
#ifdef __WIN32__
				operator	 RECT() const;
				Rect &operator	 =(const RECT &);
#endif
				Int		 left;
				Int		 top;
				Int		 right;
				Int		 bottom;

						 Rect()						{ left = 0; top = 0; right = 0; bottom = 0; }
						 Rect(const Point &iPos, const Size &iSize)	{ left = iPos.x; top = iPos.y; right = left + iSize.cx; bottom = top + iSize.cy; }

				Rect operator	 +(const Point &) const;
				Rect operator	 -(const Point &) const;

				Rect operator	 +(const Size &) const;
				Rect operator	 -(const Size &) const;

				Rect operator	 *(const Float) const;
				Rect operator	 /(const Float) const;

				Bool operator	 ==(const Rect &) const;
				Bool operator	 !=(const Rect &) const;

				static Bool	 DoRectsOverlap(const Rect &, const Rect &);
				static Rect	 OverlapRect(const Rect &, const Rect &);
		};
	};
};

#endif
