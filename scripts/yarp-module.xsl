<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" />
<xsl:variable name="newline"><xsl:text></xsl:text></xsl:variable>
<xsl:template match="/">
  /**
  *
  * @ingroup <xsl:value-of select="module/doxygen-group" />
  * \defgroup <xsl:value-of select="module/name" /><xsl:text> </xsl:text><xsl:value-of select="module/name" />
  * <xsl:value-of select="module/description" />
  * Version:<xsl:value-of select="module/version" />
  * \author <xsl:for-each select="module/authors/author">
               <xsl:choose>
                 <xsl:when test="position() = last()">
                 <xsl:value-of select="text()"/> <xsl:value-of select="@email"/>
                 </xsl:when>
                 <xsl:otherwise>
                 <xsl:value-of select="text()"/> <xsl:value-of select="@email"/>,
                 </xsl:otherwise>
               </xsl:choose>
     </xsl:for-each>
  * \n
  * \copyright <xsl:value-of select="module/copypolicy" />
  * \section intro_sec Description
  *
  <xsl:copy-of select="module/description-long/node()" disable-output-escaping="yes" />
  * \section parameters_sec Parameters
  <xsl:for-each select="module/arguments/param">
  *- --<xsl:value-of select="text()"/>: <xsl:value-of select="@desc"/>
  </xsl:for-each>
  * \section inputports_sec Input Ports
  * <xsl:for-each select="module/data/input">
  *- <xsl:value-of select="port"/>
    <xsl:text> [</xsl:text>
    <xsl:value-of select="type"/><xsl:text>] </xsl:text>
    <xsl:text> [default carrier:</xsl:text>
    <xsl:value-of select="port/@carrier"/>
    <xsl:text>]: </xsl:text>
    <xsl:copy-of select="description/node()" disable-output-escaping="yes" />
  *
  </xsl:for-each>
  * \section outputports_sec Output Ports
  * <xsl:for-each select="module/data/output">
  *- <xsl:value-of select="port"/>
    <xsl:text> [</xsl:text>
    <xsl:value-of select="type"/><xsl:text>] </xsl:text>
    <xsl:text> [default carrier:</xsl:text>
    <xsl:value-of select="port/@carrier"/>
    <xsl:text>]: </xsl:text>
    <xsl:copy-of select="description/node()" disable-output-escaping="yes" />
  *
  </xsl:for-each>
  *
  * \section services_sec Services
  * <xsl:for-each select="module/services/server">
  *- <xsl:value-of select="port"/>
  * <xsl:text> [rpc-server]: </xsl:text>
  * <xsl:copy-of select="description/node()" disable-output-escaping="yes" />
  * <xsl:text>. This service is described in </xsl:text><xsl:value-of select="type"/> (<xsl:value-of select="idl"/>)
  * </xsl:for-each>
  *
  * <xsl:for-each select="module/services/client">
  *- <xsl:value-of select="port"/>
  *  <xsl:text> [rpc-client]: </xsl:text>
  * <xsl:copy-of select="description/node()" disable-output-escaping="yes" />
  * <xsl:text>. This service is described in </xsl:text><xsl:value-of select="type"/> (<xsl:value-of select="idl"/>)
  *
  * </xsl:for-each>
  *
  *
  **/
</xsl:template>

</xsl:stylesheet>
