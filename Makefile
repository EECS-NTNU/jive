CPPFLAGS+=-Iinclude -iquote include/private
CFLAGS+=-Wall -Werror -Wfatal-errors -g

# LIBJIVE_SRC=src/context.c src/graph.c src/graphdebug.c src/bitstring.c src/fixed.c src/loadstore.c src/instruction.c src/subroutine.c src/buffer.c src/passthrough.c src/regalloc/cut.c src/regalloc/shape.c src/regalloc/assign.c src/regalloc/util.c src/regalloc/spill.c src/regalloc.c src/arithmetic-select.c src/target.c

LIBJIVE_SRC = src/context.c \
	src/vsdg/basetype.c src/vsdg/statetype.c src/vsdg/valuetype.c src/vsdg/controltype.c \
	src/vsdg/crossings.c src/vsdg/resource-interference.c src/vsdg/gate-interference.c src/vsdg/regcls-count.c \
	src/vsdg/cut.c src/vsdg/region.c src/vsdg/control.c \
	src/vsdg/node.c src/vsdg/graph.c src/vsdg/traverser.c src/vsdg/notifiers.c \
	src/util/textcanvas.c src/util/buffer.c \
	src/view/nodeview.c src/view/regionview.c src/view/reservationtracker.c src/view/graphview.c src/view.c \
	\
	src/bitstring/type.c \
	\
	src/regalloc/shaping-traverser.c src/regalloc/active-place-tracker.c src/regalloc/shape.c src/regalloc/color.c src/regalloc/regreuse.c src/regalloc/auxnodes.c src/regalloc/fixup.c src/regalloc.c \
	\
	src/arch/instruction.c src/arch/registers.c \
	\
	src/backend/i386/registerset.c\
	src/backend/i386/instructionset.c\
	src/backend/i386/machine.c\

#src/ra-common.c src/ra-graphcut-cache.c src/ra-shape.c src/regalloc.c

# LIBJIVE_SRC += src/i386/machine.c src/i386/instructions.c src/i386/abi.c src/i386/encoding.c src/i386/match.c src/i386/target.c

all: check libjive.a libjive.so

libjive.a: $(patsubst %.c, %.la, $(LIBJIVE_SRC))
libjive.so: $(patsubst %.c, %.lo, $(LIBJIVE_SRC))

doc:
	doxygen doxygen.conf

clean:
	find . -name *.o -o -name *.lo -o -name *.la -o -name *.so -o -name *.a | xargs rm -rf
	rm -rf $(TESTPROGS)

include tests/Makefile.sub

%.la: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $^

%.lo: %.c
	$(CC) -c -DPIC -fPIC $(CFLAGS) $(CPPFLAGS) -o $@ $^

%.a:
	rm -f $@
	ar clqv $@ $^
	ranlib $@

%.so:
	$(CC) -shared -o $@ $^

.PHONY: doc
