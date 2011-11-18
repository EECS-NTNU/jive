#include <assert.h>
#include <stdio.h>
#include <locale.h>
#include <sys/wait.h>

#include <jive/vsdg.h>
#include <jive/view.h>
#include <jive/util/buffer.h>
#include <jive/arch/dataobject.h>
#include <jive/arch/memory.h>
#include <jive/arch/memlayout-simple.h>
#include <jive/arch/stackslot.h>
#include <jive/regalloc/auxnodes.h>
#include <jive/vsdg/label.h>
#include <jive/vsdg/objdef.h>
#include <jive/vsdg/record.h>
#include <jive/backend/i386/instructionset.h>
#include <jive/backend/i386/registerset.h>
#include <jive/backend/i386/machine.h>
#include <jive/backend/i386/classifier.h>
#include <jive/backend/i386/instrmatch.h>
#include <jive/backend/i386/subroutine.h>
#include <jive/bitstring.h>

#include <jive/regalloc.h>
#include <jive/regalloc/shaped-graph.h>

static const jive_value_type ** string_elements;
static jive_record_declaration string_decl;

static jive_output *
make_string(jive_graph * graph, const char * txt)
{
	static const jive_bitstring_type bits8 = {{{&JIVE_BITSTRING_TYPE}}, 8};
	size_t len = strlen(txt), n;
	
	string_elements = malloc(sizeof(*string_elements) * len);
	for (n = 0; n < len; n++) {
		string_elements[n] = &bits8.base;
	}
	
	string_decl.nelements = len;
	string_decl.elements = string_elements;
	
	jive_output * chars[len];
	for (n = 0; n < len; n++) {
		size_t k;
		char bits[8];
		for (k = 0; k < 8; k++)
			bits[k] = '0' + ((txt[n] >> k) & 1);
		chars[n] = jive_bitconstant(graph, 8, bits);
	}
	
	jive_output * tmp = jive_group_create(&string_decl, len, chars);
	
	jive_memlayout_mapper_simple layout_mapper;
	jive_memlayout_mapper_simple_init(&layout_mapper, graph->context, 32);
	jive_output * dataobj = jive_dataobj(tmp, &layout_mapper.base.base);
	jive_memlayout_mapper_simple_fini(&layout_mapper);
	
	return dataobj;
}

static const char hello_world[] = "Hello, world!\n";

int main()
{
	setlocale(LC_ALL, "");
	jive_context * ctx = jive_context_create();
	jive_graph * graph = jive_graph_create(ctx);
	
	jive_subroutine * i386_fn =
		jive_i386_subroutine_create(graph->root_region,
		0, NULL,
		0, NULL);
	jive_region * fn_region = i386_fn->region;
	jive_node * enter = &i386_fn->enter->base;
	jive_node * leave = &i386_fn->leave->base;
	
	jive_node * str_name = jive_objdef_node_create(make_string(graph, hello_world), "hello_world");
	jive_node_reserve(str_name);
	
	jive_label * str_label = jive_objdef_node_cast(str_name)->attrs.start;
	jive_label * str_end_label = jive_objdef_node_cast(str_name)->attrs.end;
	
	jive_label_external write_label;
	jive_label_external_init(&write_label, ctx, "write", (intptr_t) &write);
	
	jive_node * fn_name = jive_objdef_node_create(jive_subroutine_objdef(i386_fn), "main");
	
	jive_immediate imm;
	jive_immediate_init(&imm, 0, str_label, NULL, NULL);
	
	JIVE_DECLARE_STATE_TYPE(state_type);
	
	jive_output * state = jive_node_add_output(enter, state_type);
	
	jive_node * load_str_addr = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instructions[jive_i386_int_load_imm],
		0, &imm);
	jive_node_add_input(load_str_addr, state_type, state);
	
	jive_immediate_init(&imm, 0, str_end_label, str_label, NULL);
	jive_node * load_str_len = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instructions[jive_i386_int_load_imm],
		0, &imm);
	jive_node_add_input(load_str_len, state_type, state);
	
	jive_immediate_init(&imm, 1, NULL, NULL, NULL);
	jive_node * load_fd = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instructions[jive_i386_int_load_imm],
		0, &imm);
	jive_node_add_input(load_fd, state_type, state);
	
	const jive_resource_class * gpr = &jive_i386_regcls[jive_i386_gpr].base;
	const jive_type * gpr_type = jive_resource_class_get_type(gpr);
	JIVE_DECLARE_MEMORY_TYPE(memory_type);
	
	jive_node * arg1 = jive_aux_split_node_create(
		fn_region,
		gpr_type, load_fd->outputs[0], gpr,
		memory_type, jive_callslot_class_get(4, 4, 0));
	jive_node * arg2 = jive_aux_split_node_create(
		fn_region,
		gpr_type, load_str_addr->outputs[0], gpr,
		memory_type, jive_callslot_class_get(4, 4, 4));
	jive_node * arg3 = jive_aux_split_node_create(
		fn_region,
		gpr_type, load_str_len->outputs[0], gpr,
		memory_type, jive_callslot_class_get(4, 4, 8));
	
	jive_immediate_init(&imm, 0, &write_label.base, NULL, NULL);
	jive_node * call_write = jive_instruction_node_create_extended(
		fn_region,
		&jive_i386_instructions[jive_i386_call],
		0, &imm);
	jive_node_add_input(call_write, memory_type, arg1->outputs[0])->required_rescls = jive_callslot_class_get(4, 4, 0);
	jive_node_add_input(call_write, memory_type, arg2->outputs[0])->required_rescls = jive_callslot_class_get(4, 4, 4);
	jive_node_add_input(call_write, memory_type, arg3->outputs[0])->required_rescls = jive_callslot_class_get(4, 4, 8);
	
	jive_node_add_input(leave, state_type, jive_node_add_output(call_write, state_type));
	
	jive_node_reserve(fn_name);
	jive_graph_prune(graph);
	
	//jive_view(graph, stdout);
	
	jive_graph_prune(graph);
	//jive_view(graph, stdout);
	
	jive_shaped_graph * shaped_graph = jive_regalloc(graph, &jive_i386_xfer_factory);
	jive_shaped_graph_destroy(shaped_graph);
	
	//jive_view(graph, stdout);
	
	jive_buffer buffer;
	jive_buffer_init(&buffer, ctx);
	jive_graph_generate_assembler(graph, &buffer);
	
	FILE * gcc_pipe = popen("gcc -x assembler -", "w");
	fwrite(buffer.data, buffer.size, 1, stdout);
	fwrite(buffer.data, buffer.size, 1, gcc_pipe);
	int gcc_status = pclose(gcc_pipe);
	assert(WIFEXITED(gcc_status));
	assert(WEXITSTATUS(gcc_status) == 0);
	
	jive_buffer_fini(&buffer);
	
	jive_graph_destroy(graph);
	jive_label_external_fini(&write_label);
	
	assert(jive_context_is_empty(ctx));
	
	jive_context_destroy(ctx);
	
	FILE * program_pipe = popen("./a.out", "r");
	char verify_buffer[256];
	size_t verify_count = fread(verify_buffer, 1, sizeof(verify_buffer), program_pipe);
	pclose(program_pipe);
	
	assert(verify_count == strlen(hello_world));
	assert(memcmp(verify_buffer, hello_world, verify_count) == 0);
	
	return 0;
}
