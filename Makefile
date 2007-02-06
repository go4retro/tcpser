SRC=src
SRCS = $(SRC)/tcpmdm.c $(SRC)/bridge.c $(SRC)/debug.c $(SRC)/getcmd.c $(SRC)/ip.c $(SRC)/init.c $(SRC)/modem_core.c $(SRC)/nvt.c $(SRC)/serial.c $(SRC)/util.c $(SRC)/phone_book.c $(SRC)/shared.c $(SRC)/tcpser.c $(SRC)/line.c $(SRC)/dce.c
SEROBJS = $(SRC)/bridge.o $(SRC)/debug.o $(SRC)/getcmd.o $(SRC)/ip.o $(SRC)/init.o $(SRC)/modem_core.o $(SRC)/nvt.o $(SRC)/serial.o $(SRC)/util.o $(SRC)/phone_book.o $(SRC)/shared.o $(SRC)/tcpser.o $(SRC)/dce.o $(SRC)/line.o
MDMOBJS = $(SRC)/debug.o $(SRC)/ip.o $(SRC)/serial.o $(SRC)/util.o $(SRC)/tcpmdm.o
CC = gcc
DEF = 
CFLAGS = -O $(DEF) -Wall
LDFLAGS = -lpthread
DEPEND = makedepend $(DEF) $(CFLAGS)

all:	tcpser tcpmdm

#.o.c:
#	$(CC) $(CFLAGS) -c $*.c

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

tcpmdm: $(MDMOBJS)
	$(CC) $(LDFLAGS) -o $@ $(MDMOBJS)

tcpser: $(SEROBJS)
	$(CC) $(LDFLAGS) -o $@ $(SEROBJS)

depend: $(SRCS)
	$(DEPEND) $(SRCS)

clean:
	-rm tcpser tcpmdm *.bak $(SRC)/*~ $(SRC)/*.o $(SRC)/*.bak core


# DO NOT DELETE THIS LINE -- make depend depends on it.

src/tcpmdm.o: /usr/include/sys/time.h /usr/include/features.h
src/tcpmdm.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/tcpmdm.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/tcpmdm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/tcpmdm.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/tcpmdm.o: /usr/include/bits/time.h /usr/include/sys/select.h
src/tcpmdm.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
src/tcpmdm.o: /usr/include/stdlib.h /usr/include/stdio.h /usr/include/libio.h
src/tcpmdm.o: /usr/include/_G_config.h /usr/include/wchar.h
src/tcpmdm.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/tcpmdm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/tcpmdm.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/tcpmdm.o: /usr/include/sys/param.h /usr/include/limits.h
src/tcpmdm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/tcpmdm.o: /usr/include/linux/limits.h /usr/include/linux/param.h
src/tcpmdm.o: /usr/include/asm/param.h /usr/include/sys/types.h
src/tcpmdm.o: /usr/include/termios.h /usr/include/bits/termios.h
src/tcpmdm.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
src/tcpmdm.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
src/tcpmdm.o: /usr/include/bits/ioctl-types.h /usr/include/sys/ttydefaults.h
src/tcpmdm.o: src/serial.h src/ip.h src/debug.h /usr/include/string.h
src/tcpmdm.o: /usr/include/errno.h /usr/include/bits/errno.h
src/tcpmdm.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
src/tcpmdm.o: src/tcpser.h
src/bridge.o: /usr/include/stdio.h /usr/include/features.h
src/bridge.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/bridge.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/bridge.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/bridge.o: /usr/include/_G_config.h /usr/include/wchar.h
src/bridge.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/bridge.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/bridge.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
src/bridge.o: /usr/include/sys/types.h /usr/include/time.h
src/bridge.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
src/bridge.o: /usr/include/limits.h
src/bridge.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/bridge.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
src/bridge.o: /usr/include/asm/sockios.h /usr/include/unistd.h
src/bridge.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
src/bridge.o: /usr/include/stdlib.h /usr/include/sys/param.h
src/bridge.o: /usr/include/linux/limits.h /usr/include/linux/param.h
src/bridge.o: /usr/include/asm/param.h /usr/include/sys/time.h
src/bridge.o: /usr/include/bits/time.h /usr/include/sys/select.h
src/bridge.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
src/bridge.o: /usr/include/pthread.h /usr/include/sched.h
src/bridge.o: /usr/include/bits/sched.h /usr/include/signal.h
src/bridge.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/initspin.h
src/bridge.o: /usr/include/bits/sigthread.h src/util.h src/debug.h
src/bridge.o: /usr/include/string.h /usr/include/errno.h
src/bridge.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/bridge.o: /usr/include/asm/errno.h src/nvt.h src/modem_core.h src/line.h
src/bridge.o: src/shared.h src/dce.h src/ip.h src/serial.h src/getcmd.h
src/bridge.o: src/bridge.h
src/debug.o: /usr/include/stdlib.h /usr/include/features.h
src/debug.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/debug.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/debug.o: /usr/include/time.h /usr/include/bits/types.h
src/debug.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
src/debug.o: /usr/include/pthread.h /usr/include/sched.h
src/debug.o: /usr/include/bits/sched.h /usr/include/signal.h
src/debug.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
src/debug.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
src/debug.o: /usr/include/stdio.h /usr/include/libio.h
src/debug.o: /usr/include/_G_config.h /usr/include/wchar.h
src/debug.o: /usr/include/bits/wchar.h /usr/include/gconv.h
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
src/getcmd.o: /usr/include/ctype.h /usr/include/endian.h
src/getcmd.o: /usr/include/bits/endian.h /usr/include/string.h src/getcmd.h
src/ip.o: /usr/include/sys/types.h /usr/include/features.h
src/ip.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/ip.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/ip.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/ip.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
src/ip.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
src/ip.o: /usr/include/limits.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/ip.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
src/ip.o: /usr/include/asm/sockios.h /usr/include/netinet/in.h
src/ip.o: /usr/include/stdint.h /usr/include/bits/wchar.h
src/ip.o: /usr/include/bits/in.h /usr/include/endian.h
src/ip.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
src/ip.o: /usr/include/arpa/inet.h /usr/include/netdb.h
src/ip.o: /usr/include/bits/netdb.h /usr/include/unistd.h
src/ip.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
src/ip.o: /usr/include/stdlib.h src/debug.h /usr/include/stdio.h
src/ip.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
src/ip.o: /usr/include/gconv.h
src/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/ip.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/ip.o: /usr/include/string.h /usr/include/errno.h
src/ip.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/ip.o: /usr/include/asm/errno.h src/ip.h
src/init.o: /usr/include/stdio.h /usr/include/features.h
src/init.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/init.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/init.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/init.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/init.o: /usr/include/_G_config.h /usr/include/wchar.h
src/init.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/init.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/init.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/init.o: /usr/include/stdlib.h /usr/include/unistd.h
src/init.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
src/init.o: src/debug.h /usr/include/string.h /usr/include/errno.h
src/init.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/init.o: /usr/include/asm/errno.h src/phone_book.h src/init.h
src/init.o: src/modem_core.h src/nvt.h src/line.h src/shared.h src/dce.h
src/modem_core.o: /usr/include/unistd.h /usr/include/features.h
src/modem_core.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/modem_core.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
src/modem_core.o: /usr/include/bits/wordsize.h
src/modem_core.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/modem_core.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
src/modem_core.o: /usr/include/stdlib.h src/getcmd.h src/debug.h
src/modem_core.o: /usr/include/stdio.h /usr/include/libio.h
src/modem_core.o: /usr/include/_G_config.h /usr/include/wchar.h
src/modem_core.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/modem_core.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/modem_core.o: /usr/include/bits/stdio_lim.h
src/modem_core.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
src/modem_core.o: /usr/include/errno.h /usr/include/bits/errno.h
src/modem_core.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
src/modem_core.o: src/modem_core.h src/nvt.h src/line.h src/shared.h
src/modem_core.o: src/dce.h
src/nvt.o: /usr/include/string.h /usr/include/features.h
src/nvt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/nvt.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h src/ip.h
src/nvt.o: src/nvt.h
src/serial.o: /usr/include/sys/file.h /usr/include/features.h
src/serial.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/serial.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
src/serial.o: /usr/include/sys/types.h /usr/include/bits/types.h
src/serial.o: /usr/include/bits/wordsize.h
src/serial.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/serial.o: /usr/include/bits/typesizes.h /usr/include/time.h
src/serial.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
src/serial.o: /usr/include/bits/confname.h /usr/include/termios.h
src/serial.o: /usr/include/bits/termios.h /usr/include/stdio.h
src/serial.o: /usr/include/libio.h /usr/include/_G_config.h
src/serial.o: /usr/include/wchar.h /usr/include/bits/wchar.h
src/serial.o: /usr/include/gconv.h
src/serial.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/serial.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/serial.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
src/serial.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
src/serial.o: /usr/include/bits/ioctl-types.h /usr/include/sys/ttydefaults.h
src/serial.o: src/debug.h /usr/include/string.h /usr/include/errno.h
src/serial.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/serial.o: /usr/include/asm/errno.h src/serial.h
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
src/phone_book.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
src/phone_book.o: src/phone_book.h
src/shared.o: src/modem_core.h src/nvt.h src/line.h src/shared.h src/dce.h
src/tcpser.o: /usr/include/stdio.h /usr/include/features.h
src/tcpser.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/tcpser.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/tcpser.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/tcpser.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/tcpser.o: /usr/include/_G_config.h /usr/include/wchar.h
src/tcpser.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/tcpser.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/tcpser.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/tcpser.o: /usr/include/stdlib.h /usr/include/unistd.h
src/tcpser.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
src/tcpser.o: /usr/include/sys/time.h /usr/include/time.h
src/tcpser.o: /usr/include/bits/time.h /usr/include/sys/select.h
src/tcpser.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
src/tcpser.o: /usr/include/sys/param.h /usr/include/limits.h
src/tcpser.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
src/tcpser.o: /usr/include/linux/limits.h /usr/include/linux/param.h
src/tcpser.o: /usr/include/asm/param.h /usr/include/sys/types.h
src/tcpser.o: /usr/include/pthread.h /usr/include/sched.h
src/tcpser.o: /usr/include/bits/sched.h /usr/include/signal.h
src/tcpser.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/initspin.h
src/tcpser.o: /usr/include/bits/sigthread.h src/init.h src/modem_core.h
src/tcpser.o: src/nvt.h src/line.h src/shared.h src/dce.h src/ip.h
src/tcpser.o: src/bridge.h src/phone_book.h src/util.h src/debug.h
src/tcpser.o: /usr/include/string.h /usr/include/errno.h
src/tcpser.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/tcpser.o: /usr/include/asm/errno.h src/tcpmdm.h
src/line.o: src/debug.h /usr/include/stdio.h /usr/include/features.h
src/line.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/line.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/line.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
src/line.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/line.o: /usr/include/_G_config.h /usr/include/wchar.h
src/line.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/line.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/line.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/line.o: /usr/include/string.h /usr/include/errno.h
src/line.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/line.o: /usr/include/asm/errno.h src/modem_core.h src/nvt.h src/line.h
src/line.o: src/shared.h src/dce.h src/phone_book.h src/ip.h src/bridge.h
src/dce.o: /usr/include/termios.h /usr/include/features.h
src/dce.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/dce.o: /usr/include/bits/termios.h /usr/include/sys/ioctl.h
src/dce.o: /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h
src/dce.o: /usr/include/asm/ioctl.h /usr/include/bits/ioctl-types.h
src/dce.o: /usr/include/sys/ttydefaults.h /usr/include/unistd.h
src/dce.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
src/dce.o: /usr/include/bits/wordsize.h
src/dce.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
src/dce.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
src/dce.o: src/debug.h /usr/include/stdio.h /usr/include/libio.h
src/dce.o: /usr/include/_G_config.h /usr/include/wchar.h
src/dce.o: /usr/include/bits/wchar.h /usr/include/gconv.h
src/dce.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
src/dce.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/dce.o: /usr/include/string.h /usr/include/errno.h
src/dce.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
src/dce.o: /usr/include/asm/errno.h src/serial.h src/modem_core.h src/nvt.h
src/dce.o: src/line.h src/shared.h src/dce.h
