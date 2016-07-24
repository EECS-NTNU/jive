/*
 * Copyright 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_EVALUATOR_EVAL_H
#define JIVE_EVALUATOR_EVAL_H

#include <memory>
#include <vector>

struct jive_graph;

namespace jive {
namespace evaluator {

class literal;

const std::unique_ptr<const literal>
eval(
	const struct jive_graph * graph,
	const std::string & name,
	const std::vector<const literal*> & literals);

}
}

#endif
