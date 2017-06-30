//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include "Basics.h"
#include "ReaderConstants.h"

namespace Microsoft { namespace MSR { namespace CNTK {

class BufferedFileReader
{
public:
    BufferedFileReader(size_t maxSize, FILE* file);

    // File offset that correspond to the current position.
    inline size_t GetFileOffset() const { return m_fileOffset + m_index; }

    // Returns the character at the current buffer position.
    inline char Peek()
    {
        if (m_done)
            RuntimeError("Buffer is empty.");

        assert(m_index < m_size);

        return m_buffer[m_index];
    }

    ~BufferedFileReader() 
    {
        delete[] m_buffer;
    }

    // Advances the current position to the next character.
    // Returns true, unless the EOF has been reached.
    inline bool Pop() 
    {
        if (m_done)
            return false;

        assert(m_index < m_size);

        if (m_buffer[m_index] == g_eol)
            m_lineNumber++;

        if (++m_index == m_size)
            Refill();

        return !m_done;
    }

    // Return the character at the current position and advances the position 
    // to the next character. Returns false when no more characters are available
    // (i.e, upon reaching the EOF).
    inline bool TryGetNext(char& c)
    {
        if (m_done)
            return false;

        c = Peek();
        Pop();  // move to the next character
        return true;
    }

    // Moves the current position to the next line (the position following an EOL delimiter).
    // Returns true, unless the EOF has been reached.
    bool TryMoveToNextLine();

    // Reads the current line into the provided string reference (omitting the trailing EOL).
    // Returns false upon reaching the EOF.
    bool TryReadLine(string& str);
    
    // Returns the current line number.
    inline size_t CurrentLineNumber() const { return m_lineNumber; }

    // Returns true if no more data is available (reached EOF).
    inline bool Empty() const { return m_done; }

private:
    // Read up to m_maxSize bytes from file into the buffer.
    inline void Refill();

    // Maximum allowed buffer size. 
    // Also, it defines the maximum number of bytes that we'll attempt to read at one time.
    const size_t m_maxSize{ 0 };

    // Buffer.
    char* m_buffer;

    // current buffer size (number of valid chars in the buffer, m_size is always LE m_maxSize)
    size_t m_size{ 0 };

    // Current position in the buffer.
    size_t m_index{ 0 };

    // File offset at which the buffer was fill out in the last call to Refill();
    size_t m_fileOffset{ 0 };

    // Flag indicating whether there is more data (set to true once the EOF is reached).
    bool m_done{ false };

    // Current line number;
    size_t m_lineNumber{ 0 };

    FILE* m_file{ nullptr };
};

}}}