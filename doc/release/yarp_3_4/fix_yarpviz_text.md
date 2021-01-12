fix_yarpviz_text {#yarp_3_4}
-------------------

### Tools

#### `yarpviz`

* To solve the unaesthetic text crop for subgraphs, nodes and connections labels, the `MainWindow::drawGraph` function now adds a number of white spaces proportional to the text length at the beginning and the end of labels texts. With this simple adjustments labels are now fully readable regardless of their length.
* Unfortunately, connections arrows labels, if too long can be partially hidden by the destination port ellipse. To correct this, a modification of the `YARP_priv_qgvcore` will probably be required.
