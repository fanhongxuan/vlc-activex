How to build the Webplugins under Visual Studio 2008:
-----------------------------------------------------

1) Download "msinttypes" package from
http://code.google.com/p/msinttypes/ and extract to a folder.

2) Download "Gecko SDK" (at least 8.0) from
https://developer.mozilla.org/en/Gecko_SDK and extract to a folder.

3) Download "vlc" (at this moment only nightly builds available)
from: http://nightlies.videolan.org/build/win32/last/ and
extract/install to a folder.

4) In VS, add path to "msinttypes", "Gecko SDK" include(xulrunner-sdk\include)
and vlc SDK include (vlc-1.2.0-pre1\sdk\include) to VC Include directories
(Tools > Options > Projects and Solutions > VC++ Directories
> Include files).

5) In VS, add path to vlc SDK Libs (vlc-1.2.0-pre1\sdk\lib) to VC Library
directories (Tools > Options > Projects and Solutions > VC++ Directories
> Library files);

6) Prepare MinGW/MSys build enveronment as described in
http://wiki.videolan.org/Win32CompileMSYSNew,
then run "autogen.sh" and "configure";
OR
6) Just download prebuild versions of axvlc_rc.rc and npvlc_rc.rc files from
http://code.google.com/p/vc-axnp-vlc/downloads/list and extract at the
root folder of project;

7) Open build\vc2008\axnp.sln in Visual Studio and enjoy.
