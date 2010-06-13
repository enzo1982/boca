 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_WMA_READER
#define H_BOCA_WMA_READER

#include <boca.h>
#include "wmsdk/wmsdk.h"

using namespace smooth::Threads;

namespace BoCA
{
	class WMAReader : public IWMReaderCallback,
			  public IWMReaderCallbackAdvanced
	{
		private:
			LONG				 m_cRef;

			BOOL				 m_fEOF;
			QWORD				 m_qwTime;
			DWORD				 m_dwAudioOutputNum;

			HANDLE				 m_hAsyncEvent;

			IWMReaderAdvanced		*m_pReaderAdvanced;

			Bool				 active;
			Bool				 error;

			String				 errorString;

			Buffer<UnsignedByte>		*samplesBuffer;
			Mutex				*samplesBufferMutex;
		public:
							 WMAReader();
							~WMAReader();

			HRESULT STDMETHODCALLTYPE	 QueryInterface(REFIID, void **);
			ULONG STDMETHODCALLTYPE		 AddRef();
			ULONG STDMETHODCALLTYPE		 Release();

			HRESULT STDMETHODCALLTYPE	 OnStatus(WMT_STATUS, HRESULT, WMT_ATTR_DATATYPE, BYTE *, void *);
			HRESULT STDMETHODCALLTYPE	 OnSample(DWORD, QWORD, QWORD, DWORD, INSSBuffer *, void *);
			HRESULT STDMETHODCALLTYPE	 OnTime(QWORD, void *);

			HRESULT STDMETHODCALLTYPE	 OnStreamSample(WORD wStreamNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer *pSample, void *pvContext)
							{ return S_OK; }

			HRESULT STDMETHODCALLTYPE	 OnStreamSelection(WORD wStreamCount, WORD *pStreamNumbers, WMT_STREAM_SELECTION *pSelections, void *pvContext)
							{ return S_OK; }

			HRESULT STDMETHODCALLTYPE	 OnOutputPropsChanged(DWORD dwOutputNum, WM_MEDIA_TYPE *pMediaType, void *pvContext)
							{ return S_OK; }

			HRESULT STDMETHODCALLTYPE	 AllocateForStream(WORD wStreamNum, DWORD cbBuffer, INSSBuffer **ppBuffer, void *pvContext)
							{ return E_NOTIMPL; }

			HRESULT STDMETHODCALLTYPE	 AllocateForOutput(DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer **ppBuffer, void *pvContext)
							{ return E_NOTIMPL; }

			Void				 SetAsyncEvent(HRESULT hrAsync);
		accessors:
			Bool				 IsActive();
			Void				 SetActive(Bool);

			Bool				 IsError();
			const String			&GetErrorString();

			HANDLE				 GetAsyncEventHandle() const;

			Void				 SetReaderAdvanced(IWMReaderAdvanced *);
			Void				 SetAudioOutputNum(DWORD);

			Void				 SetSamplesBuffer(Buffer<UnsignedByte> *, Mutex *);
	};
};

#endif