all: expr expr_sus expr_s42

expr: expr.o
	$(LD) $(LDFLAGS) expr.o $(LCOMMON) $(LWCHAR) $(LIBS) -o expr

expr.o: expr.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(IWCHAR) $(ICOMMON) $(LARGEF) -c expr.c

expr_sus: expr_sus.o
	$(LD) $(LDFLAGS) expr_sus.o $(LUXRE) $(LCOMMON) $(LWCHAR) $(LIBS) -o expr_sus

expr_sus.o: expr.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(IWCHAR) $(ICOMMON) $(IUXRE) $(LARGEF) -DSUS -c expr.c -o expr_sus.o

expr_s42: expr_s42.o
	$(LD) $(LDFLAGS) expr_s42.o $(LUXRE) $(LCOMMON) $(LWCHAR) $(LIBS) -o expr_s42

expr_s42.o: expr.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(IWCHAR) $(ICOMMON) $(IUXRE) $(LARGEF) -DS42 -c expr.c -o expr_s42.o

install: all
	$(UCBINST) -c expr $(ROOT)$(SV3BIN)/expr
	$(STRIP) $(ROOT)$(SV3BIN)/expr
	$(UCBINST) -c expr_sus $(ROOT)$(SUSBIN)/expr
	$(STRIP) $(ROOT)$(SUSBIN)/expr
	$(UCBINST) -c expr_s42 $(ROOT)$(S42BIN)/expr
	$(STRIP) $(ROOT)$(S42BIN)/expr
	$(MANINST) -c -m 644 expr.1 $(ROOT)$(MANDIR)/man1/expr.1

clean:
	rm -f expr expr.o expr.c expr_sus expr_sus.o \
		expr_s42 expr_s42.o core log *~
