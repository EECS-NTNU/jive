/*
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_FRONTEND_CFG_NODE_PRIVATE_H
#define JIVE_FRONTEND_CFG_NODE_PRIVATE_H

void
jive_cfg_node_init_(struct jive_cfg_node * self, struct jive_cfg * cfg);

void
jive_cfg_node_fini_(struct jive_cfg_node * self);

void
jive_cfg_node_get_label_(const jive_cfg_node * self, struct jive_buffer * buffer);

struct jive_cfg_node *
jive_cfg_node_create_(struct jive_cfg * cfg);

#endif
