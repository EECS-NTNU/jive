#include <jive/arch/registers.h>
#include <jive/bitstring/type.h>

const struct jive_type *
jive_register_class_get_type(const jive_register_class * self)
{
	return jive_resource_class_get_type(&self->base);
}

jive_gate *
jive_register_class_create_gate(const jive_register_class * self, struct jive_graph * graph, const char * name)
{
	return jive_resource_class_create_gate(&self->base, graph, name);
}

static const jive_resource_class_demotion no_demotion[] = {{NULL, NULL}};

const jive_resource_class_class JIVE_REGISTER_RESOURCE = {
	.parent = &JIVE_ABSTRACT_RESOURCE,
	.name = "register",
	.is_abstract = false
};

const jive_resource_class jive_root_register_class = {
	.class_ = &JIVE_ABSTRACT_RESOURCE,
	.name = "register",
	.limit = 0,
	.parent = &jive_root_resource_class,
	.depth = 1,
	.priority = jive_resource_class_priority_lowest,
	.demotions = no_demotion
};
