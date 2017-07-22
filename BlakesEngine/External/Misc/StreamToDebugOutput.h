#pragma once

#include <iostream>
#include <Windows.h>

class debugStreambuf : public std::streambuf {
public:
    virtual int_type overflow(int_type c = EOF) {
        if (c != EOF) {
            TCHAR buf[] = { (TCHAR)c, '\0' };
            OutputDebugString(buf);
        }
        return c;
    }
};

class Cout2VisualStudioDebugOutput {

    debugStreambuf dbgstream;
    std::streambuf *default_stream;

public:
    Cout2VisualStudioDebugOutput() {
        default_stream = std::cout.rdbuf(&dbgstream);
        default_stream = std::cerr.rdbuf(&dbgstream);
    }

    ~Cout2VisualStudioDebugOutput() {
        std::cout.rdbuf(default_stream);
    }
};