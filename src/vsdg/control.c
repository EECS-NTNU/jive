/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/control.h>

#include <string.h>
#include <stdio.h>

#include <jive/common.h>
#include <jive/context.h>
#include <jive/vsdg/anchortype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/region.h>

jive_node *
jive_control_false_create(jive_region * region)
{
	jive_node * self = jive_context_malloc(region->graph->context, sizeof(*self));
	JIVE_DECLARE_CONTROL_TYPE(control);
	self->class_ = &JIVE_CONTROL_FALSE_NODE;
	jive_node_init_(self, region,
		0, NULL, NULL,
		1, &control);
	
	return self;
}

jive_node *
jive_control_true_create(jive_region * region)
{
	jive_node * self = jive_context_malloc(region->graph->context, sizeof(*self));
	JIVE_DECLARE_CONTROL_TYPE(control);
	self->class_ = &JIVE_CONTROL_TRUE_NODE;
	jive_node_init_(self, region,
		0, NULL, NULL,
		1, &control);
	
	return self;
}

jive_output *
jive_control_false(jive_graph * graph)
{
	return jive_control_false_create(graph->root_region)->outputs[0];
}

jive_output *
jive_control_true(jive_graph * graph)
{
	return jive_control_true_create(graph->root_region)->outputs[0];
}

static jive_node *
jive_control_false_node_create_(jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 0);
	
	return jive_control_false_create(region);
}

static jive_node *
jive_control_true_node_create_(jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 0);
	
	return jive_control_true_create(region);
}


const jive_node_class JIVE_CONTROL_FALSE_NODE = {
	.parent = &JIVE_NODE,
	.name = "FALSE",
	.fini = jive_node_fini_,  /* inherit */
	.get_default_normal_form = jive_node_get_default_normal_form_,  /* inherit */
	.get_label = jive_node_get_label_,  /* inherit */
	.get_attrs = jive_node_get_attrs_,  /* inherit */
	.match_attrs = jive_node_match_attrs_,  /* inherit */
	.create = jive_control_false_node_create_,  /* override */
	.get_aux_rescls = jive_node_get_aux_rescls_  /* inherit */
};

const jive_node_class JIVE_CONTROL_TRUE_NODE = {
	.parent = &JIVE_NODE,
	.name = "TRUE",
	.fini = jive_node_fini_,  /* inherit */
	.get_default_normal_form = jive_node_get_default_normal_form_,  /* inherit */
	.get_label = jive_node_get_label_,  /* inherit */
	.get_attrs = jive_node_get_attrs_,  /* inherit */
	.match_attrs = jive_node_match_attrs_,  /* inherit */
	.create = jive_control_true_node_create_,  /* override */
	.get_aux_rescls = jive_node_get_aux_rescls_  /* inherit */
};
