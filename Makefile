SRC=src
SRCS = $(SRC)/main.c $(SRC)/bridge.c $(SRC)/debug.c $(SRC)/getcmd.c $(SRC)/ip.c $(SRC)/init.c $(SRC)/modem_core.c $(SRC)/nvt.c $(SRC)/serial.c $(SRC)/util.c $(SRC)/phone_book.c $(SRC)/shared.c
OBJS = $(SRC)/main.o $(SRC)/bridge.o $(SRC)/debug.o $(SRC)/getcmd.o $(SRC)/ip.o $(SRC)/init.o $(SRC)/modem_core.o $(SRC)/nvt.o $(SRC)/serial.o $(SRC)/util.o $(SRC)/phone_book.o $(SRC)/shared.o
CC = gcc
DEF = 
CFLAGS = -O $(DEF) -ggdb
LDFLAGS = -s -lpthread
DEPEND = makedepend $(DEF) $(CFLAGS)

all:	tcpser

#.o.c:
#	$(CC) $(CFLAGS) -c $*.c

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

tcpser: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

depend: $(SRCS)
	$(DEPEND) $(SRCS)

clean:
	-rm tcpser $(SRC)/*~ $(SRC)/*.o $(SRC)/*.bak core


# DO NOT DELETE THIS LINE -- make depend depends on it.

src/main.o: /usr/include/stdio.h /usr/include/features.h
src/main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/main.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/main.o: /usr/include/_G_config.h /usr/include/wchar.h
src/main.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/main.o: /usr/include/sys/param.h /usr/include/limits.h
src/main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/main.o: /usr/include/linux/limits.h /usr/include/linux/param.h
src/main.o: /usr/include/asm/param.h /usr/include/sys/types.h
src/main.o: /usr/include/time.h /usr/include/pthread.h /usr/include/sched.h
src/main.o: /usr/include/bits/sched.h /usr/include/signal.h
src/main.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
src/main.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
src/main.o: src/init.h src/modem_core.h src/modem_data.h src/nvt.h src/ip.h
src/main.o: src/bridge.h src/phone_book.h src/util.h src/debug.h
src/main.o: /usr/include/string.h /usr/include/errno.h
src/main.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/main.o: /usr/include/asm/errno.h src/main.h
src/bridge.o: /usr/include/stdio.h /usr/include/features.h
src/bridge.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/bridge.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/bridge.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/bridge.o: /usr/include/_G_config.h /usr/include/wchar.h
src/bridge.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/bridge.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/bridge.o: /usr/include/sys/param.h /usr/include/limits.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/bridge.o: /usr/include/linux/limits.h /usr/include/linux/param.h
src/bridge.o: /usr/include/asm/param.h /usr/include/sys/types.h
src/bridge.o: /usr/include/time.h /usr/include/sys/select.h
src/bridge.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
src/bridge.o: /usr/include/bits/time.h /usr/include/pthread.h
src/bridge.o: /usr/include/sched.h /usr/include/bits/sched.h
src/bridge.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
src/bridge.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
src/bridge.o: src/debug.h /usr/include/string.h /usr/include/errno.h
src/bridge.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/bridge.o: /usr/include/asm/errno.h src/nvt.h src/modem_core.h
src/bridge.o: src/modem_data.h src/ip.h src/serial.h src/getcmd.h
src/bridge.o: src/bridge.h
src/debug.o: /usr/include/time.h /usr/include/bits/types.h
src/debug.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/debug.o: /usr/include/gnu/stubs.h /usr/include/bits/wordsize.h
src/debug.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/debug.o: /usr/include/bits/typesizes.h /usr/include/stdio.h
src/debug.o: /usr/include/libio.h /usr/include/_G_config.h
src/debug.o: /usr/include/wchar.h /usr/include/bits/wchar.h
src/debug.o: /usr/include/gconv.h
src/debug.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/debug.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/debug.o: src/debug.h /usr/include/string.h /usr/include/errno.h
src/debug.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/debug.o: /usr/include/asm/errno.h
src/getcmd.o: /usr/include/stdio.h /usr/include/features.h
src/getcmd.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/getcmd.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/getcmd.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/getcmd.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/getcmd.o: /usr/include/_G_config.h /usr/include/wchar.h
src/getcmd.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/getcmd.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/getcmd.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/getcmd.o: src/getcmd.h
src/ip.o: /usr/include/sys/socket.h /usr/include/features.h
src/ip.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/ip.o: /usr/include/sys/uio.h /usr/include/sys/types.h
src/ip.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/ip.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/ip.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
src/ip.o: /usr/include/limits.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/ip.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
src/ip.o: /usr/include/asm/sockios.h /usr/include/netinet/in.h
src/ip.o: /usr/include/stdint.h /usr/include/bits/wchar.h
src/ip.o: /usr/include/bits/in.h /usr/include/endian.h
src/ip.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
src/ip.o: /usr/include/arpa/inet.h /usr/include/netdb.h
src/ip.o: /usr/include/bits/netdb.h src/debug.h /usr/include/stdio.h
src/ip.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
src/ip.o: /usr/include/gconv.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/ip.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/ip.o: /usr/include/string.h /usr/include/errno.h
src/ip.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/ip.o: /usr/include/asm/errno.h src/modem_core.h src/modem_data.h
src/ip.o: src/nvt.h src/phone_book.h src/ip.h src/bridge.h
src/init.o: /usr/include/stdio.h /usr/include/features.h
src/init.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/init.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/init.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/init.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/init.o: /usr/include/_G_config.h /usr/include/wchar.h
src/init.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/init.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/init.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/init.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
src/init.o: /usr/include/bits/confname.h src/debug.h /usr/include/string.h
src/init.o: /usr/include/errno.h /usr/include/bits/errno.h
src/init.o: /usr/include/linux/errno.h /usr/include/asm/errno.h src/init.h
src/init.o: src/modem_core.h src/modem_data.h src/nvt.h
src/modem_core.o: src/getcmd.h src/debug.h /usr/include/stdio.h
src/modem_core.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/modem_core.o: /usr/include/gnu/stubs.h
src/modem_core.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/modem_core.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/modem_core.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/modem_core.o: /usr/include/_G_config.h /usr/include/wchar.h
src/modem_core.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/modem_core.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/modem_core.o: /usr/include/bits/stdio_lim.h
src/modem_core.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
src/modem_core.o: /usr/include/errno.h /usr/include/bits/errno.h
src/modem_core.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
src/modem_core.o: src/modem_data.h src/nvt.h src/modem_core.h
src/nvt.o: src/ip.h src/modem_data.h src/nvt.h
src/serial.o: /usr/include/termios.h /usr/include/features.h
src/serial.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/serial.o: /usr/include/bits/termios.h /usr/include/stdio.h
src/serial.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/serial.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/serial.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/serial.o: /usr/include/_G_config.h /usr/include/wchar.h
src/serial.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/serial.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/serial.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/serial.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
src/serial.o: /usr/include/sys/types.h /usr/include/time.h
src/serial.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
src/serial.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
src/serial.o: /usr/include/bits/ioctl-types.h /usr/include/sys/ttydefaults.h
src/serial.o: /usr/include/sys/select.h /usr/include/bits/select.h
src/serial.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
src/serial.o: /usr/include/linux/serial.h src/modem_data.h src/nvt.h
src/serial.o: src/modem_core.h src/bridge.h src/debug.h /usr/include/string.h
src/serial.o: /usr/include/errno.h /usr/include/bits/errno.h
src/serial.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
src/serial.o: src/serial.h
src/util.o: /usr/include/unistd.h /usr/include/features.h
src/util.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/util.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
src/util.o: /usr/include/bits/wordsize.h
src/util.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/util.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
src/util.o: /usr/include/stdio.h /usr/include/libio.h
src/util.o: /usr/include/_G_config.h /usr/include/wchar.h
src/util.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/util.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/util.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/util.o: src/util.h
src/phone_book.o: /usr/include/stdio.h /usr/include/features.h
src/phone_book.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/phone_book.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/phone_book.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/phone_book.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/phone_book.o: /usr/include/_G_config.h /usr/include/wchar.h
src/phone_book.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/phone_book.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/phone_book.o: /usr/include/bits/stdio_lim.h
src/phone_book.o: /usr/include/bits/sys_errlist.h src/phone_book.h
src/shared.o: src/modem_core.h src/modem_data.h src/nvt.h
