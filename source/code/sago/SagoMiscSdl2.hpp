/*
Copyright (c) 2016 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SAGOMISCSDL2_HPP
#define SAGOMISCSDL2_HPP

namespace sago {

/**
 * Writes an error message to the screen and aborts the program
 * @param errorMsg The message displayed in a pop-up box to the user.
 */
void SagoFatalError(const char* errorMsg);

/**
 * Writes an error message to the screen and aborts the program
 * @param fmt A printf-style format string
 * @param ... Parameters to the format string
 */
void SagoFatalErrorF(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

}

#endif /* SAGOMISCSDL2_HPP */

