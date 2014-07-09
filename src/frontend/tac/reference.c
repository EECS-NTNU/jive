/*
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/frontend/basic_block.h>
#include <jive/frontend/cfg.h>
#include <jive/frontend/tac/reference.h>
#include <jive/frontend/tac/variable.h>

#include <stdio.h>

jive_reference_code::~jive_reference_code() noexcept {}

jive_reference_code::jive_reference_code(struct jive_basic_block * basic_block,
	jive_variable_code * variable)
	: jive_three_address_code(basic_block, {variable})
{}

std::string
jive_reference_code::debug_string() const
{
	std::string label("ref ");
	if (dynamic_cast<jive_variable_code*>(operands[0]) != NULL)
		label.append(operands[0]->debug_string());
	else {
		char tmp[32];
		snprintf(tmp, sizeof(tmp), "%p", operands[0]);
		label.append(tmp);
	}
	return label;
}

jive_three_address_code *
jive_reference_code_create(struct jive_basic_block * basic_block, jive_variable_code * variable)
{
	jive_reference_code * ref = new jive_reference_code(basic_block, variable);
	return ref;
}
