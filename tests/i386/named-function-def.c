/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <assert.h>
#include <stdio.h>
#include <locale.h>

#include <jive/vsdg.h>
#include <jive/view.h>
#include <jive/util/buffer.h>
#include <jive/vsdg/label.h>
#include <jive/vsdg/objdef.h>
#include <jive/arch/codegen.h>
#include <jive/arch/label-mapper.h>
#include <jive/types/function/fctlambda.h>
#include <jive/backend/i386/instructionset.h>
#include <jive/backend/i386/registerset.h>
#include <jive/backend/i386/machine.h>
#include <jive/backend/i386/classifier.h>
#include <jive/backend/i386/instrmatch.h>
#include <jive/backend/i386/subroutine.h>
#include <jive/types/bitstring/arithmetic.h>

#include <jive/regalloc.h>
#include <jive/regalloc/shaped-graph.h>

static int test_main(void)
{
	setlocale(LC_ALL, "");
	jive_context * ctx = jive_context_create();
	jive_graph * graph = jive_graph_create(ctx);
	
	JIVE_DECLARE_BITSTRING_TYPE(bits32, 32);
	jive_lambda * lambda = jive_lambda_begin(graph,
		2, (const jive_type *[]){bits32, bits32}, (const char *[]){"arg1", "arg2"});

	jive_output * sum = jive_bitsum(lambda->narguments, lambda->arguments);

	jive_node * abstract_fn = jive_lambda_end(lambda, 1, &bits32, &sum)->node;
	
	jive_subroutine * i386_fn = jive_i386_subroutine_convert(graph->root_region, abstract_fn);
	jive_linker_symbol add_int32_symbol;
	jive_node * fn_name = jive_objdef_node_create(
		jive_subroutine_objdef(i386_fn),
		"add_int32",
		&add_int32_symbol);

	jive_graph_export(graph, fn_name->outputs[0]);
	jive_graph_prune(graph);
	
	jive_view(graph, stdout);
	
	jive_regselector regselector;
	jive_regselector_init(&regselector, graph, &jive_i386_reg_classifier);
	jive_regselector_process(&regselector);
	jive_i386_match_instructions(graph, &regselector);
	jive_regselector_fini(&regselector);
	
	jive_graph_prune(graph);
	//jive_view(graph, stdout);
	
	jive_shaped_graph * shaped_graph = jive_regalloc(graph);
	jive_shaped_graph_destroy(shaped_graph);
	
	//jive_view(graph, stdout);
	
	jive_buffer buffer;
	jive_buffer_init(&buffer, ctx);
	jive_symbol_name_pair symtab[] = {{&add_int32_symbol, "add_int32"}};
	jive_label_name_mapper * name_mapper = jive_label_name_mapper_simple_create(ctx, symtab, 1);
	jive_graph_generate_assembler(graph, name_mapper, &buffer);
	jive_label_name_mapper_destroy(name_mapper);
	fwrite(buffer.data, buffer.size, 1, stdout);
	jive_buffer_fini(&buffer);
	
	jive_graph_destroy(graph);
	assert(jive_context_is_empty(ctx));
	
	jive_context_destroy(ctx);
	
	return 0;
}


JIVE_UNIT_TEST_REGISTER("i386/named-function-def", test_main);
