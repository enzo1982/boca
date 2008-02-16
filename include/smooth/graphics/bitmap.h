 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BITMAP_
#define _H_OBJSMOOTH_BITMAP_

namespace smooth
{
	namespace GUI
	{
		class Surface;
		class Bitmap;
		class BitmapBackend;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "forms/size.h"
#include "forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Bitmap
		{
			private:
				BitmapBackend		*backend;
			public:
							 Bitmap(Void * = NIL);
							 Bitmap(Int, Int, Int);
							 Bitmap(const int);
							 Bitmap(const Bitmap &);
				virtual			~Bitmap();

				Int			 GetBitmapType() const;

				const Size		&GetSize() const;
				Int			 GetDepth() const;

				UnsignedByte		*GetBytes() const;
				Int			 GetLineAlignment() const;

				Bool			 CreateBitmap(Int, Int, Int);
				Bool			 DeleteBitmap();

				Bool			 SetSystemBitmap(Void *);
				Void			*GetSystemBitmap() const;

				Int			 GrayscaleBitmap();
				Int			 InvertColors();
				Int			 ReplaceColor(const Color &, const Color &);

				Int			 BlitFromSurface(Surface *, const Rect &, const Rect &);
				Int			 BlitToSurface(const Rect &, Surface *, const Rect &);

				Bool			 SetPixel(const Point &, const Color &);
				Color			 GetPixel(const Point &) const;

				Bitmap &operator	 =(const int);
				Bitmap &operator	 =(const Bitmap &);

				Bool operator		 ==(const int) const;
				Bool operator		 !=(const int) const;
		};
	};
};

#endif
