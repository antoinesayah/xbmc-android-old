export ARCH_FLAGS=-march=armv7-a -mtune=cortex-a9 -mfloat-abi=softfp -mfpu=neon
export HOST=arm-linux-eabi
export CROSSTOOLS=$(TOOLCHAIN)/bin/arm-linux-androideabi-
export CC=$(CROSSTOOLS)gcc
export CXX=$(CROSSTOOLS)g++
export AR=$(CROSSTOOLS)ar
export LD=$(CROSSTOOLS)ld
export AS=$(CROSSTOOLS)as
export NM=$(CROSSTOOLS)nm
export STRIP=$(CROSSTOOLS)strip
export RANLIB=$(CROSSTOOLS)ranlib
export OBJDUMP=$(CROSSTOOLS)objdump
export CFLAGS=$(ARCH_FLAGS) -isystem$(XBMCPREFIX)/include
export CPPFLAGS=$(CFLAGS)
export CXXFLAGS=$(CFLAGS)
export LDFLAGS=$(ARCH_FLAGS) -L$(XBMCPREFIX)/lib
export LIBS=-lstdc++
export PATH:=${XBMCPREFIX}/bin:${TOOLCHAIN}/bin:$(PATH)
export PREFIX=$(XBMCPREFIX)
