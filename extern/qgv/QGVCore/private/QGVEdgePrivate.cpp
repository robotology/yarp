#include "QGVEdgePrivate.h"

QGVEdgePrivate::QGVEdgePrivate(Agedge_t *edge)
{
	setEdge(edge);
}

void QGVEdgePrivate::setEdge(Agedge_t *edge)
{
	_edge = edge;
}

Agedge_t* QGVEdgePrivate::edge() const
{
	return _edge;
}
