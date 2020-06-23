/*
 * Copyright 2020 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testnodes.h"
#include "testtypes.h"

#include <assert.h>

#include <jive/rvsdg.h>

static int
test_main()
{
	using namespace jive;

	test::valuetype vt;

	jive::graph graph;
	auto import1 = graph.add_import({vt, "import1"});

	auto n1 = test::structural_node_create(graph.root(), 1);
	auto n2 = test::structural_node_create(graph.root(), 2);

	/* Test type error check for adding argument to wrong input */

	auto structi1 = n1->add_input(vt, import1);

	bool input_error_handler_called = false;
	try {
		argument::create(n2->subregion(0), structi1, vt);
	} catch (jive::compiler_error & e) {
		input_error_handler_called = true;
	}

	assert(input_error_handler_called);

	/* Test type error check for adding result to wrong output */

	auto argument = argument::create(n1->subregion(0), structi1, vt);
	auto structo1 = n1->add_output(vt);

	bool output_error_handler_called = false;
	try {
		result::create(n2->subregion(0), argument, structo1, vt);
	} catch (jive::compiler_error & e) {
		output_error_handler_called = true;
	}

	assert(output_error_handler_called);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("rvsdg/test-regionmismatch", test_main)
