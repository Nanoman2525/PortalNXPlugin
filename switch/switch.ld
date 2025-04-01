OUTPUT_ARCH(aarch64)

PHDRS
{
	text PT_LOAD FLAGS(5) /* Read | Execute */;
	rodata PT_LOAD FLAGS(4) /* Read */;
	data PT_LOAD FLAGS(6) /* Read | Write */;
	dyn PT_DYNAMIC;
}

SECTIONS
{
	PROVIDE_HIDDEN(__start__ = 0x0);

	. = __start__;
	__text_start__ = .;

	/* App text */
	.text : {
		KEEP (*(.crt0))
		*(.text)
		. = ALIGN(8);
	} :text

	/* Procedure linkage table */
	.plt : {
		*(.plt .plt.*)
		*(.iplt)
		. = ALIGN(8);
	} :text

	/* rodata section alignment */
	. = ALIGN(0x1000);
	PROVIDE_HIDDEN(__rodata_start__ = .);

	.nx-module-name : { KEEP (*(.nx-module-name)) } :rodata

	/* App rodata */
	.rodata : {
		*(.rodata .rodata.*)
		. = ALIGN(8);
	} :rodata

	/* Relocation lookup data */
	.hash : { *(.hash) } :rodata
	.gnu.hash : { *(.gnu.hash) } :rodata
	.dynsym : { *(.dynsym .dynsym.*) } :rodata
	.dynstr : { *(.dynstr .dynstr.*) } :rodata

	__rel_dyn_start__ = .;
	.rel.dyn : { *(.rel.dyn) } :rodata
	__rel_dyn_end__ = .;

	__rela_dyn_start__ = .;
	.rela.dyn : { *(.rela.dyn) } :rodata
	__rela_dyn_end__ = .;

	__rel_plt_start__ = .;
	.rel.plt : { *(.rel.plt) } :rodata
	__rel_plt_end__ = .;

	__rela_plt_start__ = .;
	.rela.plt : { *(.rela.plt) } :rodata
	__rela_plt_end__ = .;

	/* All exception handling sections */
 	.gcc_except_table : { *(.gcc_except_table .gcc_except_table.*) } :rodata
	.eh_frame_hdr : {
		PROVIDE_HIDDEN(__eh_frame_hdr_start__ = .);
		*(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*)
		PROVIDE_HIDDEN(__eh_frame_hdr_end__ = .);
	} :rodata
	.eh_frame : { KEEP (*(.eh_frame)) } :rodata

	/* Keep build-id in rodata */
	.note.gnu.build-id : { *(.note.gnu.build-id) } :rodata

	/* data section alignment */
	. = ALIGN(0x1000);
	PROVIDE_HIDDEN(__data_start__ = .);

	.data : {
		*(.data .data.*)
		. = ALIGN(8);
	} :data

	.data.rela.ro : {
 		*(.data.rela.ro.local*)
 		*(.data.rela.ro .data.rela.ro.*)
 	} :data

	.data.rel.ro : {
 		*(.data.rel.ro.local*)
 		*(.data.rel.ro .data.rel.ro.*)
 	} :data

	.got : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) } :data

	.dynamic : {
	 	PROVIDE_HIDDEN(__dynamic_start__ = .);
	 	*(.dynamic)
	} :data :dyn

	. = ALIGN(8);

	.preinit_array : {
		PROVIDE_HIDDEN(__preinit_array_start__ = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN(__preinit_array_end__ = .);
	} :data

	.init_array : {
		PROVIDE_HIDDEN(__init_array_start__ = .);
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP(*(.init_array .ctors))
		PROVIDE_HIDDEN(__init_array_end__ = .);
	} :data

	.fini_array : {
		PROVIDE_HIDDEN(__fini_array_start__ = .);
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP(*(.fini_array .dtors))
		PROVIDE_HIDDEN(__fini_array_end__ = .);
	} :data

	.tdata : {
		*(.tdata .tdata.* .gnu.linkonce.td.*)
		. = ALIGN(8);
	} :data


 	/* BSS section */
 	. = ALIGN(0x1000);

	.tbss : {
		*(.tbss .tbss.*)
		*(.tcommon)
		. = ALIGN(8);
	} :data

	.bss : {
		PROVIDE_HIDDEN(__bss_start__ = .);
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		PROVIDE_HIDDEN(__bss_end__ = .);
		. = ALIGN(8);
	} :data

	PROVIDE_HIDDEN(__end__ = .);

	/* arg buffer stuff */
	. = ALIGN(0x1000);
	PROVIDE_HIDDEN(__argdata__ = .);

	/DISCARD/ : {
		/* No need of the interpreter */
		*(.interp)
	}
}
