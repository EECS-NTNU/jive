#include <jive/vsdg/graph-private.h>
#include <jive/vsdg/region-private.h>
#include <jive/vsdg/traverser-private.h>

static inline void
_jive_graph_init(jive_graph * self, jive_context * context)
{
	self->context = context;
	self->resources.first = self->resources.last = 0;
	self->top.first = self->top.last = 0;
	self->bottom.first = self->bottom.last = 0;
	self->resources_fully_assigned = false;
	
	self->root_region = jive_context_malloc(context, sizeof(*self->root_region));
	_jive_region_init(self->root_region, self, 0);
	
	self->ntraverser_slots = 0;
	self->traverser_slots = 0;
	
	jive_node_notifier_slot_init(&self->on_node_create, context);
	jive_node_notifier_slot_init(&self->on_node_destroy, context);
	jive_node_notifier_slot_init(&self->on_node_shape, context);
}

static void
_jive_graph_fini(jive_graph * self)
{
	/* TODO: destroy all regions, nodes, resources etc. */
	jive_node_notifier_slot_fini(&self->on_node_create);
	jive_node_notifier_slot_fini(&self->on_node_destroy);
	jive_node_notifier_slot_fini(&self->on_node_shape);
}

jive_graph *
jive_graph_create(jive_context * context)
{
	jive_graph * graph;
	graph = jive_context_malloc(context, sizeof(*graph));
	_jive_graph_init(graph, context);
	return graph;
}

void
jive_graph_destroy(jive_graph * self)
{
	_jive_graph_fini(self);
	jive_context_free(self->context, self);
}

bool
jive_graph_has_active_traversers(const jive_graph * self)
{
	size_t n;
	for(n=0; n<self->ntraverser_slots; n++)
		if (self->traverser_slots[n].traverser) return true;
	return false;
}

void
jive_graph_prune(jive_graph * self)
{
	jive_node * node;
	node = self->bottom.first;
	while(node) {
		jive_node * next = node->graph_bottom_list.next;
		
		if (!node->reserved) jive_node_destroy(node);
		
		node = next;
	}
}

void
jive_graph_notify_input_change(jive_graph * graph, jive_input * input, jive_output * old_origin, jive_output * new_origin)
{
	/* TODO */
}

void
jive_graph_notify_input_create(jive_graph * graph, jive_input * input)
{
	/* TODO */
}

void
jive_graph_notify_input_destroy(jive_graph * graph, jive_input * input)
{
	/* TODO */
}

void
jive_graph_notify_output_create(jive_graph * graph, jive_output * output)
{
	/* TODO */
}

void
jive_graph_notify_output_destroy(jive_graph * graph, jive_output * output)
{
	/* TODO */
}

