<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="@*|node()">
        <xsl:copy>
            <xsl:apply-templates select="@*|node()"/>
        </xsl:copy>
    </xsl:template>

    <xsl:template match="group">
        <xsl:element name="{@name}">
                 <xsl:attribute name="prop">group</xsl:attribute>
             <xsl:apply-templates />
         </xsl:element>
    </xsl:template>

    <xsl:template match="param">
        <xsl:element name="{@name}">
                 <xsl:attribute name="prop">param</xsl:attribute>
             <xsl:apply-templates />
         </xsl:element>
    </xsl:template>


</xsl:stylesheet>
