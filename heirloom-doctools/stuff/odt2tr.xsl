<?xml version="1.0" encoding="utf-8"?>
<!--
	Sccsid @(#)odt2tr.xsl	1.3 (gritter) 10/3/06

	A simplistic OpenDocument to troff converter in form of
	an XSLT stylesheet. See the usage instructions below.
-->

<!--
	Copyright (c) 2006 Gunnar Ritter
	
	This software is provided 'as-is', without any express or
	implied warranty. In no event will the authors be held liable
	for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for
	any purpose, including commercial applications, and to
	alter it and redistribute it freely, subject to the following
	restrictions:

	1. The origin of this software must not be misrepresented; you
	   must not claim that you wrote the original software. If you
	   use this software in a product, an acknowledgment in the
	   product documentation would be appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and
	   must not be misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	   distribution.
-->

<!--
	The main purpose of this stylesheet is to extract texts
	from office documents for typesetting, a typical task
	when producing a book: Most authors use word processors
	and hand in (be it proprietary or open format) office
	documents. When converting them to troff, some formatting
	elements (italics, headings, footnotes, etc.) should be
	preserved, although the general layout is a decision of
	the typesetter. The stylesheet thus makes an attempt to
	save the former but discards the latter. This is not
	always possible - an office document is organized as a
	set of "styles" which have only restricted significance
	as generic markup elements. Since the styles are typically
	useless for typesetting, they are discarded, but some
	of their markup information is directly included in the
	generated troff document.

	Since OpenOffice can read many word processor formats,
	the first conversion step usually is to open the author's
	document in OpenOffice and to save it as OpenDocument text
	(.odt).

	Now use cpio or unzip to extract the .odt file (which is
	actually a zip archive). This will produce, amongst others,
	a file named content.xml. Then (using xsltproc from libxml2,
	or perhaps another XSLT processor), execute:

		xsltproc odt2tr.xsl content.xml >converted.tr

	No macro set is supplied with the generated troff document,
	but the generated macros are roughly compatible with -mm,
	which makes it possible to obtain a preview by

		tbl converted.tr | troff -mm | dpost >converted.ps

	It is normally necessary (and part of the process for which
	the stylesheet is intended) to further edit the output.

	The generated troff document contains the following markup:

	\fX		The following fonts are expected: R, I,
			B, BI, and SC (for small capitals). All
			other font information is discarded.

	.H		A heading of with four arguments: The first
			argument is the heading level (1, 2, 3, ...);
			the second argument contains the heading text.
			The third argument contains markup for the
			heading, and the third contains markup reset
			instructions. These are separated from the
			text to ease the creation of table of contents
			entries.

	.P		A paragraph. Actually, almost any <text:p>
			tag results in a .P call, although <text:p>
			may also have been generated in place of a
			heading. There is no way to automatically
			distinguish between them, so some handwork
			is to be expected.

	.IP		An indented paragraph. The indent amount is
			added as a comment but is not passed. This
			is because it is the typesetter's, not the
			author's, decision.

			Temporary indents result in a comment after
			a .P or .IP call but there is no code emitted
			for them.

	.ML		Begin a marked list. The single argument is
			the mark character.

	.AL		Begin a numbered list.

	.LE		End a list (bulleted or numbered).

	.LI		Start a list item.

	.LP		Paragraph (any <text:p> tag) inside a list.

	\*F		The numbering for the next footnote.

	.FS/.FE		Footnote text start/end macros.

	.TS/.TE/T{/T}	Table structure for tbl. All table markup is
			discarded; all columns are left-aligned in
			the generated document. It is expected that
			the typesetter will apply the exact format
			by hand.

	Of course, it is easy to modify the stylesheet if other macro
	names are more suitable.
-->
<stylesheet
	xmlns="http://www.w3.org/1999/XSL/Transform"
	xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
	xmlns:date="http://exslt.org/dates-and-times"
	xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
	xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
	xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
	xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
	version="1.0">

<output method="text" omit-xml-declaration="yes" encoding="utf-8"/>

<strip-space elements="*"/>
<preserve-space elements="text:h text:p text:span"/>

<template name="parastyle">
<choose>
<when test="number(translate(//style:style[@style:name = current()/@text:style-name]/style:paragraph-properties/@fo:margin-left, 'cimnptx', '       ')) > 0">
.IP \" indent <value-of select="//style:style[@style:name = current()/@text:style-name]/style:paragraph-properties/@fo:margin-left"/><text>&#10;</text>
</when>
<otherwise>
.P<text>&#10;</text>
</otherwise>
</choose>
<choose>
<when test="//style:style[@style:name = current()/@text:style-name]/style:paragraph-properties/@fo:text-indent">.\" temporary indent <value-of select="//style:style[@style:name = current()/@text:style-name]/style:paragraph-properties/@fo:text-indent"/><text>&#10;</text>
</when>
</choose>
</template>

<template name="textstyle">
<choose>
<when test="//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-variant='small-caps'">\f(SC</when>
<when test="//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-style = 'italic' and //style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-weight='bold'">\f(BI</when>
<when test="//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-style = 'italic'">\fI</when>
<when test="//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-style = 'bold'">\fB</when>
</choose>
<!-- discard font sizes
<choose>
<when test="contains(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, 'pt')">\s[<value-of select="substring-before(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, 'p')"/>]</when>
<when test="contains(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, '%')">\s[\n(PS*<value-of select="substring-before(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, '%')"/>/100]</when>
</choose>--></template>

<template name="endtextstyle">
<choose>
<when test="//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-variant='small-caps' or //style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-style = 'italic' or //style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-weight='bold'">\fR</when>
</choose>
<!-- discard font sizes
<choose>
<when test="contains(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, 'pt')">\s[\n(PS]</when>
<when test="contains(//style:style[@style:name = current()/@text:style-name]/style:text-properties/@fo:font-size, '%')">\s[\n(PS]</when>
</choose>--></template>

<template name="liststyle">
<choose>
<when test="//text:list-style[@style:name = current()/@text:style-name]/text:list-level-style-bullet">
.ML <value-of select="//text:list-style[@style:name = current()/@text:style-name]/text:list-level-style-bullet[1]/@text:bullet-char"/>
</when>
<when test="//text:list-style[@style:name = current()/@text:style-name]/text:list-level-style-number">
.AL</when>
</choose>
</template>

<template match="text:span"><call-template name="textstyle"/><apply-templates/><call-template name="endtextstyle"/></template>

<template match="text:p[text()]">
<call-template name="parastyle"/>
<call-template name="textstyle"/>
<apply-templates/><call-template name="endtextstyle"/></template>

<template match="text:list-item/text:p[text()]">
.LP
<call-template name="textstyle"/>
<apply-templates/><call-template name="endtextstyle"/></template>

<template match="text:list">
<call-template name="liststyle"/>
<apply-templates/>
.LE</template>

<template match="text:list-item">
.LI<apply-templates/></template>

<template match="text:h">
.H <value-of select="@text:outline-level"/> "<apply-templates/>" "<call-template name="textstyle"/>" "<call-template name="endtextstyle"/>"</template>

<template match="text:note">\*F\c
<apply-templates/>
</template>

<template match="text:note-citation">.\" footnote <apply-templates/>
</template>

<template match="text:note-body">
.FS
<apply-templates mode="footnote"/>
.FE
</template>

<template match="text:p[1]" mode="footnote">
<apply-templates/>
</template>

<template match="table:table">
.TS
<apply-templates/>
.TE</template>

<template match="table:table-column">
<call-template name="tablekey">
<with-param name="n">
<value-of select="@table:number-columns-repeated"/>
</with-param>
</call-template>
<if test="not(following-sibling::table:table-column)">.</if>
</template>

<template name="tablekey">
<text>l </text>
<if test="$n > 1">
<call-template name="tablekey">
<with-param name="n">
<value-of select="$n - 1"/>
</with-param>
</call-template>
</if>
</template>

<template match="table:table-row">
<text>&#10;</text><apply-templates/>
</template>

<template match="table:table-cell">T{
<apply-templates/>
T}<if test="following-sibling::table:table-cell"><text>&#9;</text></if></template>

<template match="table:table-cell/text:p[1][text()]">
<call-template name="textstyle"/>
<apply-templates/><call-template name="endtextstyle"/>
</template>

<template match="/">.\" Converted by odt2tr.xsl 1.3 (gritter) 10/3/06 on <value-of select="date:date-time()"/><apply-templates/>
<text>&#10;</text></template>

</stylesheet>
