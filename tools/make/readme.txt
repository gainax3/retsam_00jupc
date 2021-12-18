GNU make 3.80.1 (2007/10/29 version, 2 eval patches applied)

- About This Package

Version 3.81 of GNU make no longer supports MS-DOS path names. As a result, builds cannot be performed in development environments using the DOS format for path names.

While the previous version of make (version 3.80) supports DOS format path names, that version is no longer available for download on the official Cygwin site.

We have created a new make.exe (version 3.80) that uses the cygwin1.dll dynamic link library from the current Cygwin environment (which includes make version 3.81). This was done by downloading the source code for make version 3.80 from the Cygwin site and re-building it in the current Cygwin environment using configure and make.

This package includes both this rebuilt version 3.80 of make and its source code. It is distributed under the GNU General Public License.

(Added on 2007/10/25)
A patch has been applied to resolve a bug that caused the message "virtual memory exhausted" to appear and stop the 'make' process if the eval function was called with a long argument.

(Added on 2007/10/29)
A patch has been applied to resolve a bug that caused the error "missing 'endif'" to occur when the eval function was used within an if ... endif block.
 

- Using the Tool

Replace the executable file make.exe with the make.exe file in $Cygwin\bin.
(Note that the cygwin1.dll dynamic link library, which is necessary to execute make, is not included. Consequently, the make.exe executable cannot be used alone.)

Please download the Cygwin package from the following URL:
http://cygwin.com/


- List of Files Included in this Package

COPYING                    GNU General Public License (in English)
make-3.80-1-src.tar.bz2    Source code
make.exe                   Executable file
readme.txt                 This file
patch/
  eval-conditional.patch  Patch to fix a bug with the eval function (added on 2007/10/29)
  eval-crash.diff         Patch to fix a bug with the eval function (added on 2007/10/25)
  readme.txt              Information about the patches listed above (added on 2007/10/29)


- How to Build

	% bzcat make-3.80-1-src.tar.bz2 | tar xf -
	% cd make-3.80-1
	% patch -p0 < ../patch/eval-conditional.patch
	patching file read.c
	Hunk #1 succeeded at 272 with fuzz 2.
	Hunk #2 succeeded at 419 with fuzz 1 (offset 3 lines).
	Hunk #3 succeeded at 435 (offset 3 lines).
	Hunk #4 succeeded at 449 with fuzz 1 (offset 3 lines).
	Hunk #5 succeeded at 831 (offset 20 lines).
	Hunk #6 succeeded at 857 (offset 20 lines).
	patching file tests/scripts/functions/eval
	% patch -p0 < ../patch/eval-crash.diff
	patching file variable.h
	patching file expand.c
	Hunk #1 succeeded at 564 (offset 19 lines).
	patching file function.c
	Hunk #1 succeeded at 1281 with fuzz 2 (offset 85 lines).
	% ./configure --disable-nls
	   [Output omitted]
	% make
	   [Output omitted]
	% strip -s make.exe
	% ./make check
	   [Output omitted]
	   NOTE: One of the tests (features/default_names) may fail,
             but the end result will be "Regression PASSED".


- Digest Value of make.exe in This Package

	% sha1sum make.exe
	8e496949832a4dc6c11332518cbe3b558b827148 *make.exe