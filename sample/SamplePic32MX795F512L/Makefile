

update:
	echo Updating...
	mkdir -p NR-Clib/lib/PIC32/32MX795F512L/
	mkdir -p NR-Clib/include/
	svn update
	if (test -d ../NR-Clib/); then \
		cd ../NR-Clib/;\
		svn update;\
		cd ..;\
	else \
		cd ..;\
		svn checkout https://nr-sdk.googlecode.com/svn/trunk/firmware/library/NR-Clib/development NR-Clib;\
	fi;
	cd ../NR-Clib;svn update
	if (test -d ../NR-Clib);					then cd ../NR-Clib/src;make pic32MX795F512L;		fi
	if (test -d ../NR-Clib/include/); 			then rsync -raupE --exclude=.svn*					../NR-Clib/include/* 							NR-Clib/include;			fi
	if (test -d ../NR-Clib/lib/); 				then rsync -raupE --exclude=.svn* --exclude=*.r*   	../NR-Clib/lib/PIC32/32MX795F512L/*			NR-Clib/lib/PIC32/32MX795F512L/;				fi
	svn commit -m=""
	echo Done Updating...
