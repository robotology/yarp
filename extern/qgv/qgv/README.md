Interactive Qt graphViz display
===============================

Features :
----------

* Use cgraph lib (C++ API wrapper)
* Support edges : label on edge, simple arrow both direction
* Support Nodes : box or ellipse shape, image on Node
* Support subGraphs
* Interactions with node and edge (context menu and double click)
* Zoom/move on graph
* Tested only on dot engine
* Only 6 headers/sources files to add


Screen capture :
----------------

![Screen capture](http://i39.tinypic.com/2gy1z0h.png)

* Windows binairies demo here : https://github.com/nbergont/qgv/releases

Installation :
--------------

* Download qgv sources from GIT : git clone https://github.com/nbergont/qgv.git
* Download graphViz librairie : http://www.graphviz.org/Download.php
* Configure GRAPHVIZ_PATH in QGraphViz.pro
* Open with Qt Creator & compile

TODO :
------

* Support more attributes of Graphviz
* Add more comments
* Support head/tail label on edge
* Fully support layout from dot language (may be works...)
* Dynamicaly move node and redraw layout ? (I dont know if it's possible) : not possible

Good lecture on subject :
-------------------------

* Steve Dodier-Lazaro : <http://www.mupuf.org/blog/2010/07/08/how_to_use_graphviz_to_draw_graphs_in_a_qt_graphics_scene/>
* Arvin Schnell : <http://arvin.schnell-web.net/qgraph/>
* 

Other similar projects (you must look before lose time with GraphViz) :
-----------------------
* Constraint-based diagram editor : https://github.com/mjwybrow/dunnart  (Probably best one ! Awesome work ...)
* OGDF - Open Graph Drawing Framework : http://www.ogdf.net/doku.php (Very good library, better than GraphViz !)

