/*
 * Copyright 2017 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_SIMPLE_H
#define JIVE_VSDG_SIMPLE_H

#include <jive/vsdg/operation.h>

namespace jive {

class simple_op : public operation {
public:
	virtual
	~simple_op();
};

}

#endif