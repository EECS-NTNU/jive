/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/label.h>

#include <stdio.h>

#include <jive/common.h>
#include <jive/context.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/region.h>

/* label, abstract base type */

const jive_label_class JIVE_LABEL = {
	.parent = 0,
	.fini = 0,
};

static void
jive_label_init_(jive_label * self, const jive_label_class * cls, jive_label_flags flags)
{
	self->class_ = cls;
	self->flags = flags;
}

/* special "current" label */

const jive_label_class JIVE_LABEL_CURRENT = {
	.parent = &JIVE_LABEL,
	.fini = 0,
};

const jive_label jive_label_current = {
	.class_ = &JIVE_LABEL_CURRENT,
	.flags = jive_label_flags_none,
};

/* special "fpoffset" label */

const jive_label_class JIVE_LABEL_FPOFFSET = {
	.parent = &JIVE_LABEL,
	.fini = 0,
};

const jive_label jive_label_fpoffset = {
	.class_ = &JIVE_LABEL_FPOFFSET,
	.flags = jive_label_flags_none,
};

/* special "spoffset" label */

const jive_label_class JIVE_LABEL_SPOFFSET = {
	.parent = &JIVE_LABEL,
	.fini = 0,
};

const jive_label jive_label_spoffset = {
	.class_ = &JIVE_LABEL_SPOFFSET,
	.flags = jive_label_flags_none,
};

/* internal labels */

static void
jive_label_internal_init_(jive_label_internal * self, const jive_label_class * cls, jive_label_flags flags, jive_graph * graph)
{
	jive_label_init_(&self->base, cls, flags);
	self->asmname = 0;
	self->graph = graph;
}

static void
jive_label_internal_register_(jive_label_internal * self)
{
	JIVE_LIST_PUSH_BACK(self->graph->labels, self, graph_label_list);
	jive_label_notifier_slot_call(&self->graph->on_label_create, self);
}

static void
jive_label_internal_fini_(jive_label * self_)
{
	jive_label_internal * self = (jive_label_internal *) self_;
	if (self->asmname)
		jive_context_free(self->graph->context, self->asmname);
	JIVE_LIST_REMOVE(self->graph->labels, self, graph_label_list);
}

const jive_label_internal_class JIVE_LABEL_INTERNAL_ = {
	.base = {
		.parent = &JIVE_LABEL,
		.fini = 0,
	}
};

/* node labels */

const jive_label_internal_class JIVE_LABEL_NODE_ = {
	.base = {
		.parent = &JIVE_LABEL_INTERNAL,
		.fini = jive_label_internal_fini_,
	}
};

jive_label_node *
jive_label_node_create_dangling(jive_graph * graph)
{
	jive_label_node * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_NODE, jive_label_flags_none, graph);
	self->node = 0;
	
	jive_label_internal_register_(&self->base);
	return self;
}

jive_label *
jive_label_node_create(jive_node * node)
{
	jive_graph * graph = node->graph;
	
	jive_label_node * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_NODE, jive_label_flags_none, graph);
	self->node = node;
	
	jive_label_internal_register_(&self->base);
	
	return &self->base.base;
}

/* region labels */

const jive_label_internal_class JIVE_LABEL_REGION_START_ = {
	.base = {
		.parent = &JIVE_LABEL_INTERNAL,
		.fini = jive_label_internal_fini_,
	}
};

jive_label_region *
jive_label_region_start_create_dangling(jive_graph * graph)
{
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_START, jive_label_flags_none, graph);
	self->region = 0;
	
	jive_label_internal_register_(&self->base);
	return self;
}

jive_label *
jive_label_region_start_create(jive_region * region)
{
	jive_graph * graph = region->graph;
	
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_START, jive_label_flags_none, graph);
	self->region = 0;
	
	self->region = region;
	
	jive_label_internal_register_(&self->base);
	return &self->base.base;
}

jive_label *
jive_label_region_start_create_exported(jive_region * region, const char * name)
{
	jive_graph * graph = region->graph;
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_START, jive_label_flags_none, graph);
	self->region = region;
	self->base.asmname = jive_context_strdup(graph->context, name);
	self->base.base.flags |= jive_label_flags_global;
	
	jive_label_internal_register_(&self->base);
	return &self->base.base;
}

/* region labels */

const jive_label_internal_class JIVE_LABEL_REGION_END_ = {
	.base = {
		.parent = &JIVE_LABEL_INTERNAL,
		.fini = jive_label_internal_fini_,
	}
};

jive_label_region *
jive_label_region_end_create_dangling(jive_graph * graph)
{
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_END, jive_label_flags_none, graph);
	
	jive_label_internal_register_(&self->base);
	return self;
}

jive_label *
jive_label_region_end_create(jive_region * region)
{
	jive_graph * graph = region->graph;
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_END, jive_label_flags_none, graph);
	
	self->region = region;
	
	jive_label_internal_register_(&self->base);
	return &self->base.base;
}

jive_label *
jive_label_region_end_create_exported(jive_region * region, const char * name)
{
	jive_graph * graph = region->graph;
	jive_label_region * self = jive_context_malloc(graph->context, sizeof(*self));
	jive_label_internal_init_(&self->base, &JIVE_LABEL_REGION_END, jive_label_flags_none, graph);
	self->region = region;
	self->base.asmname = jive_context_strdup(graph->context, name);
	self->base.base.flags |= jive_label_flags_global;
	
	jive_label_internal_register_(&self->base);
	return &self->base.base;
}

/* external labels */

static void
jive_label_external_fini_(jive_label * self_)
{
	jive_label_external * self = (jive_label_external *) self_;
	jive_label_external_fini(self);
}

const jive_label_class JIVE_LABEL_EXTERNAL = {
	.parent = &JIVE_LABEL,
	.fini = jive_label_external_fini_,
};


void
jive_label_external_init(
	jive_label_external * self,
	struct jive_context * context,
	const char * name)
{
	self->base.class_ = &JIVE_LABEL_EXTERNAL;
	self->base.flags = jive_label_flags_external;
	self->context = context;
	self->asmname = jive_context_strdup(context, name);
}

void
jive_label_external_fini(jive_label_external * self)
{
	jive_context_free(self->context, self->asmname);
}
