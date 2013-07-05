/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/objdef.h>

#include <string.h>

#include <jive/context.h>
#include <jive/util/buffer.h>
#include <jive/vsdg/anchortype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/label.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/region.h>

static void
jive_objdef_node_init_(
	jive_objdef_node * self,
	jive_region * region,
	jive_output * obj,
	const char * name,
	const struct jive_linker_symbol * symbol)
{
	const jive_type * type = jive_output_get_type(obj);
	jive_node_init_(&self->base, region,
		1, &type, &obj,
		0, NULL);
	
	if (obj->node->ninputs < 1 || obj->node->inputs[0]->class_ != &JIVE_ANCHOR_INPUT) {
		jive_context_fatal_error(region->graph->context, "Type mismatch: object definitions can only be applied to region anchor nodes");
	}
	
	jive_region * objregion = obj->node->inputs[0]->origin->node->region;
	
	self->attrs.name = jive_context_strdup(region->graph->context, name);
	self->attrs.symbol = symbol;
	self->attrs.start = jive_label_region_start_create_exported(objregion, name);
	self->attrs.end = jive_label_region_end_create(objregion);
}

static void
jive_objdef_node_fini_(jive_node * self_)
{
	jive_objdef_node * self = (jive_objdef_node *) self_;
	
	jive_context * context = self->base.graph->context;
	
	jive_context_free(context, self->attrs.name);
	
	/* note: cannot free the labels here -- they might still be referenced! */
	/*
	jive_label_fini(self->attrs.start);
	jive_context_free(context, self->attrs.start);
	jive_label_fini(self->attrs.end);
	jive_context_free(context, self->attrs.end);
	*/
	
	jive_node_fini_(&self->base);
}

static void
jive_objdef_node_get_label_(const jive_node * self_, struct jive_buffer * buffer)
{
	const jive_objdef_node * self = (const jive_objdef_node *) self_;
	jive_buffer_putstr(buffer, self->attrs.name);
}

const jive_node_attrs *
jive_objdef_node_get_attrs_(const jive_node * self_)
{
	const jive_objdef_node * self = (const jive_objdef_node *) self_;
	return &self->attrs.base;
}

static bool
jive_objdef_node_match_attrs_(const jive_node * self, const jive_node_attrs * attrs)
{
	return false;
}


static jive_node *
jive_objdef_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[])
{
	const jive_objdef_node_attrs * attrs = (const jive_objdef_node_attrs *) attrs_;
	
	jive_objdef_node * other = jive_context_malloc(region->graph->context, sizeof(*other));
	other->base.class_ = &JIVE_OBJDEF_NODE;
	jive_objdef_node_init_(other, region, operands[0], attrs->name, attrs->symbol);
	
	return &other->base;
}

const jive_node_class JIVE_OBJDEF_NODE = {
	.parent = &JIVE_NODE,
	.name = "OBJDEF",
	.fini = jive_objdef_node_fini_, /* override */
	.get_default_normal_form = jive_node_get_default_normal_form_, /* inherit */
	.get_label = jive_objdef_node_get_label_, /* override */
	.get_attrs = jive_objdef_node_get_attrs_, /* override */
	.match_attrs = jive_objdef_node_match_attrs_, /* override */
	.create = jive_objdef_node_create_, /* override */
	.get_aux_rescls = jive_node_get_aux_rescls_, /* inherit */
};

jive_node *
jive_objdef_node_create(
	jive_output * output,
	const char * name,
	const struct jive_linker_symbol * symbol)
{
	jive_region * region = output->node->region;
	jive_objdef_node * self = jive_context_malloc(region->graph->context, sizeof(*self));
	self->base.class_ = &JIVE_OBJDEF_NODE;
	jive_objdef_node_init_(self, region, output, name, symbol);
	
	return &self->base;
}
