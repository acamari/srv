BINDIR?=/usr/local/bin
MANDIR?=/usr/local/man/man
MANDOCBIN?=mandoc
PROG=srv

.include <bsd.prog.mk>

manhtml: ${MAN}
	for m in $?; do \
		${MANDOCBIN} -T html \
		    -O style=http://man.openbsd.org/mandoc.css,man=http://man.openbsd.org/%N.%S \
		    $$m > $$m.html; \
	done

manmarkdown: ${MAN}
	for m in $?; do \
		${MANDOCBIN} -T markdown $$m > $$m.md; \
	done

release: manhtml manmarkdown
