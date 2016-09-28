http://stackoverflow.com/questions/17667978/using-boost-in-android-ndk-with-windows

Android NDK is no longer dependent on Cygwin, so you can build Boost with the NDK from within Windows command prompt (cmd).

In order to make Boost.Build find the NDK, edit boost\tools\build\v2\user-config.jam file and append the following text:

import os ;

androidNDKRoot = C:/android-ndk-r8e ; # put the relevant path
 using gcc : android :
     $(androidNDKRoot)/toolchains/arm-linux-androideabi-4.7/prebuilt/windows/bin/arm-linux-androideabi-g++ :
     <compileflags>--sysroot=$(androidNDKRoot)/platforms/android-9/arch-arm
     <compileflags>-mthumb
     <compileflags>-Os
     <compileflags>-fno-strict-aliasing
     <compileflags>-O2
     <compileflags>-DNDEBUG
     <compileflags>-g
     <compileflags>-lstdc++
     <compileflags>-I$(androidNDKRoot)/sources/cxx-stl/gnu-libstdc++/4.7/include
     <compileflags>-I$(androidNDKRoot)/sources/cxx-stl/gnu-libstdc++/4.7/libs/armeabi/include
     <compileflags>-D__GLIBC__
     <compileflags>-D_GLIBCXX__PTHREADS
     <compileflags>-D__arm__
     <compileflags>-D_REENTRANT
     <archiver>$(androidNDKRoot)/toolchains/arm-linux-androideabi-4.7/prebuilt/windows/bin/arm-linux-androideabi-ar
     <ranlib>$(androidNDKRoot)/toolchains/arm-linux-androideabi-4.7/prebuilt/windows/bin/arm-linux-androideabi-ranlib
         ;
Certainly, instead of c:/android-ndk-r8e you have to put the right location of the NDK on your PC.

Besides, you can select more recent platform API, instead of android-9.

Also note that the NDK supplies several tool-chains, and the above settings point to gcc-4.7. If you prefer to build boost with some other tool-chain, change arm-linux-androideabi-4.7 to the relevant path.

After you've put the configuration in user-config.jam, open cmd, cd to the directory where Boost resides, and invoke bootstrap. Then invoke b2 like this (for example):

b2 --without-python --without-serialization threading=multi link=static runtime-link=static toolset=gcc-android target-os=linux threadapi=pthread --stagedir=android stage

UPDATE: As of 11/2015, older NDK toolchains seem to have issues with the newer Boost versions, causing compiler crash, so consider using a more recent compiler. To do this, just change every 4.7 occurrence in the above script to 4.9. Also, it's worth compiling with a more recent Android API (eg. andoroid-9 -> andoroid-16 or so).