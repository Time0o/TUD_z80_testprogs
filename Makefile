SRC_DIR=source
x86_OUTDIR=x86
Z80_OUTDIR=z80

_PROGS=hello functioncall memaccess mul div swap lup
x86_PROGS=$(patsubst %,$(x86_OUTDIR)/%.out,$(_PROGS))
Z80_PROGS=$(patsubst %,$(Z80_OUTDIR)/%/prog.out,$(_PROGS))

define create_progmem
  bin=$(patsubst %.out,%.bin,$(1)) ; \
  progmem_int=$(patsubst %.out,%.progmem_int,$(1)) ; \
  progmem_ext=$(patsubst %.out,%.progmem_ext,$(1)) ; \
  echo "compiling $$bin" ; \
  hex2bin -p 00 -l 65536 -T 0xffff $(1) > /dev/null ; \
  hexdump -v -e '"@%04_ax " 1/1 "%02x" "\n"' $$bin > $$progmem_int ; \
  sed -i -r '0,/^.* c9$$/{s/(.*) c9/\1 76/}' $$progmem_int ; \
  csplit $$progmem_int '/^@1000.*$$/' &> /dev/null ; \
  if [ $$? -eq 0 ]; then \
    mv xx00 $$progmem_int ; \
    mv xx01 $$progmem_ext ; \
  fi
endef

all: $(x86_PROGS) $(Z80_PROGS)

$(x86_OUTDIR)/%.out: $(SRC_DIR)/%.c
	gcc -o $@ $<

$(Z80_OUTDIR)/%/prog.out: $(SRC_DIR)/%.c
	@sdcc -DNDEBUG -DZ80 -o $@ -mz80 --no-std-crt0 --callee-saves-bc \
    --code-loc 0x0000 --code-size 0x0a00 --data-loc 0x0a00 $< ; \
	$(call create_progmem, $@)

.PHONY: clean

clean:
	@rm -f $(x86_OUTDIR)/* $(Z80_OUTDIR)/**/*
