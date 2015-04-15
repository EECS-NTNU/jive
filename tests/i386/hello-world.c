/*
 * Copyright 2010 2011 2012 2013 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <sys/wait.h>

#include <jive/arch/address.h>
#include <jive/arch/call.h>
#include <jive/arch/codegen.h>
#include <jive/arch/dataobject.h>
#include <jive/arch/label-mapper.h>
#include <jive/arch/memlayout-simple.h>
#include <jive/arch/memorytype.h>
#include <jive/arch/stackslot.h>
#include <jive/arch/subroutine/nodes.h>
#include <jive/backend/i386/call.h>
#include <jive/backend/i386/classifier.h>
#include <jive/backend/i386/instrmatch.h>
#include <jive/backend/i386/instructionset.h>
#include <jive/backend/i386/machine.h>
#include <jive/backend/i386/registerset.h>
#include <jive/backend/i386/subroutine.h>
#include <jive/regalloc.h>
#include <jive/regalloc/shaped-graph.h>
#include <jive/types/bitstring.h>
#include <jive/types/record/rcdgroup.h>
#include <jive/util/buffer.h>
#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/label.h>
#include <jive/vsdg/objdef.h>
#include <jive/vsdg/splitnode.h>

static jive::output *
make_string(jive_graph * graph, const char * txt)
{
	static const jive::bits::type bits8(8);
	static std::vector<const jive::value::type*> string_elements;
	size_t len = strlen(txt), n;
	
	for (n = 0; n < len; n++)
		string_elements.push_back(&bits8);
	static std::shared_ptr<const jive::rcd::declaration> string_decl(
		new jive::rcd::declaration(string_elements));

	jive::output * chars[len];
	for (n = 0; n < len; n++) {
		size_t k;
		char bits[8];
		for (k = 0; k < 8; k++)
			bits[k] = '0' + ((txt[n] >> k) & 1);
		chars[n] = jive_bitconstant(graph, 8, bits);
	}
	
	jive::output * tmp = jive_group_create(string_decl, len, chars);
	
	jive::memlayout_mapper_simple layout_mapper(4);
	return jive_dataobj(tmp, &layout_mapper);
}

static const char hello_world[] = "Hello, world!\n";

static int test_main(void)
{
	setlocale(LC_ALL, "");
	jive_graph * graph = jive_graph_create();
	
	jive_subroutine i386_fn = jive_i386_subroutine_begin(
		graph,
		0, NULL,
		0, NULL);
	jive_region * fn_region = i386_fn.region;
	jive_node * enter = fn_region->top;
	
	jive_linker_symbol hello_world_symbol;
	jive_label_external hello_world_label;
	jive_label_external_init(&hello_world_label, "hello_world", &hello_world_symbol);
	jive::output * str_name = jive_objdef_create(
		make_string(graph, hello_world),
		"hello_world",
		&hello_world_symbol);
	jive_graph_export(graph, str_name);
	
	jive_linker_symbol write_symbol;
	jive_label_external write_label;
	jive_label_external_init(&write_label, "write", &write_symbol);
	
	jive_linker_symbol main_symbol;
	
	jive::immediate imm(0, &hello_world_label.base);
	
	jive_test_state_type state_type;
	
	jive::output * state = jive_node_add_output(enter, &state_type);
	
	jive_node * load_str_addr = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instr_int_load_imm,
		0, &imm);
	jive_node_add_input(load_str_addr, &state_type, state);
	
	imm = strlen(hello_world);
	jive_node * load_str_len = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instr_int_load_imm,
		0, &imm);
	jive_node_add_input(load_str_len, &state_type, state);
	
	imm = 1;
	jive_node * load_fd = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instr_int_load_imm,
		0, &imm);
	jive_node_add_input(load_fd, &state_type, state);
	
	const jive_resource_class * gpr = &jive_i386_regcls_gpr.base;
	const jive::base::type * gpr_type = jive_resource_class_get_type(gpr);
	jive::mem::type memory_type;
	
	jive_node * arg1 = jive_splitnode_create(
		fn_region,
		gpr_type, load_fd->outputs[0], gpr,
		&memory_type, jive_callslot_class_get(4, 4, 0));
	jive_node * arg2 = jive_splitnode_create(
		fn_region,
		gpr_type, load_str_addr->outputs[0], gpr,
		&memory_type, jive_callslot_class_get(4, 4, 4));
	jive_node * arg3 = jive_splitnode_create(
		fn_region,
		gpr_type, load_str_len->outputs[0], gpr,
		&memory_type, jive_callslot_class_get(4, 4, 8));
	
	imm = jive::immediate(0, &write_label.base);
	jive_node * call_write = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instr_call,
		0, &imm);
	jive_node_add_input(call_write, &memory_type, arg1->outputs[0])->required_rescls
		= jive_callslot_class_get(4, 4, 0);
	jive_node_add_input(call_write, &memory_type, arg2->outputs[0])->required_rescls
		= jive_callslot_class_get(4, 4, 4);
	jive_node_add_input(call_write, &memory_type, arg3->outputs[0])->required_rescls
		= jive_callslot_class_get(4, 4, 8);
	
	/* mark call as affecting global state */
	jive::output * memstate = jive_subroutine_simple_get_global_state(i386_fn);
	const jive::base::type * memtype = &memstate->type();
	jive_node_add_input(call_write, memtype, memstate);
	jive_subroutine_simple_set_global_state(
		i386_fn, jive_node_add_output(call_write, memtype));
	
	jive_node * main_fn = jive_subroutine_end(i386_fn);
	
	jive::output * fn_name = jive_objdef_create(
		main_fn->outputs[0],
		"main",
		&main_symbol);
	jive_graph_export(graph, fn_name);
	jive_graph_prune(graph);
	
	//jive_view(graph, stdout);
	
	jive_graph_prune(graph);
	//jive_view(graph, stdout);
	
	jive_shaped_graph * shaped_graph = jive_regalloc(graph);
	jive_shaped_graph_destroy(shaped_graph);
	
	//jive_view(graph, stdout);
	
	jive_buffer buffer;
	jive_symbol_name_pair symtab[] = {
		{&hello_world_symbol, "hello_world"},
		{&main_symbol, "main"},
		{&write_symbol, "write"},
	};
	jive_label_name_mapper * name_mapper = jive_label_name_mapper_simple_create(symtab, 3);
	jive_graph_generate_assembler(graph, name_mapper, &buffer);
	jive_label_name_mapper_destroy(name_mapper);
	
	FILE * gcc_pipe = popen("gcc -m32 -x assembler -", "w");
	fwrite(&buffer.data[0], buffer.data.size(), 1, stdout);
	fwrite(&buffer.data[0], buffer.data.size(), 1, gcc_pipe);
	int gcc_status = pclose(gcc_pipe);
	assert(WIFEXITED(gcc_status));
	assert(WEXITSTATUS(gcc_status) == 0);
	
	jive_graph_destroy(graph);
	
	FILE * program_pipe = popen("./a.out", "r");
	char verify_buffer[256];
	size_t verify_count = fread(verify_buffer, 1, sizeof(verify_buffer), program_pipe);
	pclose(program_pipe);
	
	assert(verify_count == strlen(hello_world));
	assert(memcmp(verify_buffer, hello_world, verify_count) == 0);
	
	return 0;
}


JIVE_UNIT_TEST_REGISTER("i386/hello-world", test_main);
