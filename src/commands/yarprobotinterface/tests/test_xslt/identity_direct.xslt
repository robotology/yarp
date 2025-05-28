<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="node()">
        <xsl:copy>
            <xsl:apply-templates select="node()"/>
        </xsl:copy>
    </xsl:template>

    <xsl:template match="//*[@prop='group']">
        <group name="{name()}"> <xsl:apply-templates select="node()"/>    </group>
    </xsl:template>

        <xsl:template match="//*[@prop='param']">
        <param name="{name()}"> <xsl:apply-templates select="node()"/>    </param>
    </xsl:template>

</xsl:stylesheet>
