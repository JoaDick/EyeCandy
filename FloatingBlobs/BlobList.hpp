#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2020 Joachim Dick

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/

#if (FLOATING_BLOBS_SIMU)
#include <iostream>
#endif

#include "Blob.hpp"

//------------------------------------------------------------------------------

template <uint8_t BLOB_COUNT>
class BlobList
{
    Blob _allBlobs[BLOB_COUNT];
    Blob *_idleBlobs = nullptr;

public:
    Blob *_firstBlob = nullptr;
    Blob *_lastBlob = nullptr;
    uint8_t _activeBlobs = 0;

    BlobList()
    {
        for (uint16_t i = 0; i < BLOB_COUNT; ++i)
        {
            _allBlobs[i].nr = uint8_t(i);
        }
        reset();
    }

    void reset()
    {
        _activeBlobs = 0;
        _firstBlob = nullptr;
        _lastBlob = nullptr;
        _idleBlobs = &_allBlobs[0];
        for (uint16_t i = 0; i < BLOB_COUNT - 1; ++i)
        {
            _allBlobs[i].nextBlob = &_allBlobs[i + 1];
        }
    }

    Blob *insertBlobRandomly()
    {
        Blob *insertedBlob = nullptr;

        if (_idleBlobs)
        {
            Blob *where = nullptr;
            uint8_t insertIndex = random8(_activeBlobs + 1);
            if (insertIndex)
            {
                where = _firstBlob;
                while (--insertIndex)
                {
                    where = where->nextBlob;
                }
            }

            insertedBlob = insertBlobAfter(where);
        }

        return insertedBlob;
    }

    Blob *insertBlobAfter(Blob *blob)
    {
        Blob *blob2insert = _idleBlobs;
        if (blob2insert)
        {
            _idleBlobs = blob2insert->nextBlob;

            if (blob)
            {
                blob2insert->nextBlob = blob->nextBlob;
                blob->nextBlob = blob2insert;
                if (_lastBlob == blob)
                {
                    _lastBlob = blob2insert;
                }
            }
            else
            {
                // insert blob after beginning (= first blob)
                blob2insert->nextBlob = _firstBlob;
                _firstBlob = blob2insert;
                // is this the first blob?
                if (_lastBlob == nullptr)
                {
                    _lastBlob = blob2insert;
                }
            }
            ++_activeBlobs;
#if (FLOATING_BLOBS_SIMU)
            // std::cout << "active: " << int(_activeBlobs) << " - inserted " << int(blob2insert->nr) << std::endl;
#endif
        }
        // else: no blob to be inserted available

        return blob2insert;
    }

    void removeBlobAfter(Blob *blob)
    {
        Blob *blob2remove;
        if (blob)
        {
            blob2remove = blob->nextBlob;
        }
        else
        {
            // remove blob after beginning (= first blob)
            blob2remove = _firstBlob;
        }

        if (blob2remove)
        {
            if (blob)
            {
                blob->nextBlob = blob2remove->nextBlob;
            }
            else
            {
                _firstBlob = blob2remove->nextBlob;
            }
            if (blob2remove->nextBlob == nullptr)
            {
                _lastBlob = blob;
            }

            blob2remove->nextBlob = _idleBlobs;
            _idleBlobs = blob2remove;
            --_activeBlobs;
#if (FLOATING_BLOBS_SIMU)
            // std::cout << "active: " << int(_activeBlobs) << " - removed " << int(blob2remove->nr) << std::endl;
#endif
        }
        // else: no blob to remove available
    }
};
