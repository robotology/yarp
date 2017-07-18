#include "QGVNodePrivate.h"

QGVNodePrivate::QGVNodePrivate(Agnode_t *node)
{
		setNode(node);
}

void QGVNodePrivate::setNode(Agnode_t *node)
{
	_node = node;
}

Agnode_t* QGVNodePrivate::node() const
{
	return _node;
}
