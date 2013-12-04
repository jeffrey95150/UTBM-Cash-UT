CC=gcc -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
#CFLAGS_DEBUG=`pkg-config --cflags gtk+-3.0` -g -DDEBUG
LIBS=`pkg-config --libs gtk+-3.0` `mysql_config --cflags --libs` -std=c99 -DAUTOCONNECT
EXEC_NAME=cashut
#EXEC_NAME_DEBUG=${EXEC_NAME}_debug

OBJS= 	main.o \
	main_callbacks.o \
	main_mysql.o \
	cashut_main.o \
	cashut_main_callbacks.o \
	main_gestion_liste.o

RESOURCES=window_connexion.inc window_cashut.inc

cashut: $(RESOURCES) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

#cashut_debug: $(RESOURCES) $(OBJS)
#	$(CC) $(CFLAGS_DEBUG) -o $@ $(OBJS) $(LIBS)

%.inc: glade/%.glade
	@echo Converting $< to $@
	@echo /\* autogenerated file, do not modify! \*/> $@
	@echo /\* source file: $< \*/>> $@
	@echo const gchar \* $(subst .,_,$(<F))=>> $@
	@sed -e 's/"/\\"/g' -e 's/.*/"&"/' $< >> $@
	@echo \;>> $@

#%.inc: resources/%.png
#	@echo Converting $< to $@
#	@gdk-pixbuf-csource --raw --name=$(subst .,_,$(<F)) $< > $@ 

main.o: main.c window_connexion.inc main_callbacks.h main_mysql.h structures.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

main_callbacks.o: main_callbacks.c main_callbacks.h main_mysql.h cashut_main.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

main_mysql.o: main_mysql.c main_mysql.h main_callbacks.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

cashut_main.o: cashut_main.c cashut_main.h cashut_main_callbacks.h main_mysql.h main_gestion_liste.h structures.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

cashut_main_callbacks.o: cashut_main_callbacks.c cashut_main_callbacks.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

main_gestion_liste.o: main_gestion_liste.c main_gestion_liste.h structures.h main_mysql.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

clean:
	${RM} *.o *.inc ${EXEC_NAME} ${EXEC_NAME_DEBUG}


