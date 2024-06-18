/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: https://github.com/pfedick/PPLGenResource
 *******************************************************************************
 * Copyright (c) 2024, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <ppl7.h>
#include "genresource.h"


/*********************************************************************************
 * Resourcen generieren
 *********************************************************************************/

static void IncludeHelp(ppl7::FileObject &out, const ppl7::String &configfile)
{
	ppl7::DateTime now;
	now.setCurrentTime();
	out.putsf(
		"/*******************************************************************************\n"
		" * PPL7 Resourcen Generator Version %i.%i.%i\n"
		" * %s\n"
		"",PPL7_VERSION_MAJOR,PPL7_VERSION_MINOR,PPL7_VERSION_BUILD,PPL7_COPYRIGHT
	);

	out.putsf(
		" *\n"
		" * File generation: %s\n"
		" * Config: %s\n"
		" *******************************************************************************/\n\n",
		(const char*)now.get(), (const char*)configfile
	);
	out.puts(
		"/* File-Format:\n"
		" *    Byte 0-5: ID \"PPLRES\"                              (6 Byte)\n"
		" *    Byte 6:   0-Byte                                   (1 Byte)\n"
		" *    Byte 7:   Major Version (6)                        (1 Byte)\n"
		" *    Byte 8:   Minor Version (0)                        (1 Byte)\n"
		" *    Byte 9:   Start of Chunks\n"
		" *\n"
		" * Chunk-Format:\n"
		" *    Byte 0:  Size of Chunk incl. Header                (4 Byte)\n"
		" *    Byte 4:  ID of Resource                            (2 Byte)\n"
		" *    Byte 6:  Size of data uncompressed                 (4 Byte)\n"
		" *    Byte 10: Size of data compressed                   (4 Byte)\n"
		" *    Byte 14: Compression-Type (0=none,1=zlib,2=bzip2)  (1 Byte)\n"
		" *    Byte 15: Offset of Data from Chunk-Start           (1 Byte)\n"
		" *    Byte 16: Name of Resource with 0-Byte              (n Bytes)\n"
		" *    Byte n:  Data\n"
		" *\n"
		" * Chunks are repeated as often as required, followed by\n"
		" * a 4 Byte 0-Value (Chunk size 0), which marks the end of file\n"
		" */\n\n"
		);
}

static void BufferOut(ppl7::FileObject &out, const char *buffer, int bytes)
{
	static int c=0;
	static char clear[25]="";
	for (int i=0;i<bytes;i++) {
		uint8_t byte=(uint8_t)buffer[i];
		out.putsf("0x%02x,",byte);
		if(byte>31 && byte<128 && byte!='\\' && byte!='/') clear[c]=byte;
		else clear[c]='.';
		c++;
		clear[c]=0;
		if (c>15) {
			c=0;
			out.putsf(" // %s\n    ",clear);
			clear[0]=0;
		}
	}
	if (!bytes) {
		for (int i=c;i<16;i++) {
			out.puts("     ");
		}
		c=0;
		out.putsf(" // %s\n    ",clear);
		clear[0]=0;
	}
}

static void Output(ppl7::FileObject &ff, int resid, const ppl7::String &name, const ppl7::String &filename, int size_u, const char *buffer, int bytes, int compressiontype)
{
	char *buf=(char*)malloc(64);
	if (!buf) throw ppl7::OutOfMemoryException();

	uint32_t chunksize=bytes+name.size()+17;
	ppl7::Poke32(buf+0,chunksize);
	ppl7::Poke16(buf+4,resid);
	ppl7::Poke32(buf+6,size_u);
	ppl7::Poke32(buf+10,bytes);
	ppl7::Poke8(buf+14,compressiontype);
	ppl7::Poke8(buf+15,17+name.size());
	BufferOut(ff,buf,16);
	BufferOut(ff,name,name.size()+1);
	//BufferOut(ff,filename,strlen(filename)+1);
	BufferOut(ff,buffer,bytes);
	free(buf);
}

static int Compress(ppl7::FileObject &ff, char **buffer, size_t *size, int *type)
{
	ppl7::Compression comp;
	size_t size_u=(size_t)ff.size();
	char *source=(char*)ff.map();			// Komplette Datei mappen

	size_t size_c=size_u+2048;
	size_t size_zlib=size_c;
	size_t size_bz2=size_c;
	char *buf=(char*)malloc(size_c);
	if (!buf) throw ppl7::OutOfMemoryException();
	// Zuerst Zlib
	comp.init(ppl7::Compression::Algo_ZLIB,ppl7::Compression::Level_High);
	comp.compress(buf,&size_zlib,source,size_u);
	// Dann Bzip2
	comp.init(ppl7::Compression::Algo_BZIP2,ppl7::Compression::Level_High);
	comp.compress(buf,&size_bz2,source,size_u);
	// Was war kleiner?
	if (size_u<=size_bz2 && size_u<=size_zlib) {
		free(buf);
		printf ("Using no compression: %u Bytes (Zlib: %u, BZ2: %u)\n",(uint32_t)size_u, (uint32_t)size_zlib, (uint32_t)size_bz2);
		*size=size_u;
		*type=0;
		*buffer=(char*)malloc(size_u);
		memcpy(*buffer,source,size_u);
		return 1;
	}
	if (size_bz2<size_zlib) {
		printf ("Using bzip2: %u Bytes von %u Bytes (zlib: %u Bytes)\n",(uint32_t)size_bz2,(uint32_t)size_u,(uint32_t)size_zlib);
		*buffer=buf;
		*size=size_bz2;
		*type=2;
		return 1;
	}
	size_zlib=size_c;
	comp.init(ppl7::Compression::Algo_ZLIB,ppl7::Compression::Level_High);
	comp.compress(buf,&size_zlib,source,size_u);
	printf ("Using zlib: %u Bytes von %u Bytes (bzip2: %u Bytes)\n",(uint32_t)size_zlib,(uint32_t)size_u,(uint32_t)size_bz2);
	*buffer=buf;
	*size=size_zlib;
	*type=1;
	return 1;
}


void GenerateResourceHeader(const ppl7::String &basispfad, const ppl7::String &configfile, const ppl7::String &targetfile, const ppl7::String &label)
{
	if (configfile.isEmpty()) throw ppl7::IllegalArgumentException();
	ppl7::ConfigParser conf;
	conf.load(configfile);
	ppl7::String BasePath=basispfad;
	ppl7::String TargetFile=targetfile;
	ppl7::String Label=label;
	if (conf.hasSection("setup")) {
		conf.selectSection("setup");
		if (BasePath.isEmpty()) BasePath=conf.get("path");
		if (TargetFile.isEmpty()) TargetFile=conf.get("targetfile");
		if (Label.isEmpty()) Label=conf.get("label");
	}

	if (BasePath.isEmpty()) {
		throw ppl7::MissingArgumentException("basispfad");
	}
	
	if (TargetFile.isEmpty()) {
		throw ppl7::MissingArgumentException("targetfile");
	}

	if (Label.isEmpty()) {
		throw ppl7::MissingArgumentException("label");
	}
	ppl7::String Prefix, Suffix;
	if (conf.hasSection("prefix")) Prefix=conf.getSection("prefix");
	if (conf.hasSection("suffix")) Suffix=conf.getSection("suffix");
	//const char *path=basispfad;

	ppl7::File out;
	out.open(TargetFile,ppl7::File::WRITE);
	printf ("Verarbeite Resourcen in: %s...\n",(const char*)configfile);
	if (Prefix.notEmpty()) out.puts(Prefix);
	IncludeHelp(out, configfile);

	out.puts("/**************************************************************\n");
	out.puts(" * Resourcen:\n");
	out.puts(" *\n");
	int havesection=conf.firstSection();
	while(havesection) {
		//printf ("havesection: %s\n", (const char*)conf.getSectionName());
		//sprintf(section,"%s",(const char*)conf.getSectionName());
		int id=conf.getInt("ID");
		ppl7::String name=conf.get("Name");
		ppl7::String filename=conf.get("File");
		havesection=conf.nextSection();
		if (filename.isEmpty()) continue;
		if (!id) continue;
		if (name.isEmpty()) continue;
		ppl7::File ff;
		ppl7::String filepath;
		filepath.setf("%s/%s",(const char*)BasePath,(const char*)filename);
		try {
			ff.open(filepath,ppl7::File::READ);
			out.putsf(" * %4i: %-20s (%s)\n",id,(const char*)name,(const char*)filename);
		} catch (...) {
			throw;
		}
	}

	out.puts(" **************************************************************/\n\n");
	out.putsf("static unsigned char %s []={\n    ",(const char*)Label);
	char section[12];
	sprintf(section,"PPLRES");
	ppl7::Poke8(section+7,6);
	ppl7::Poke8(section+8,0);
	BufferOut(out,section,9);

	havesection=conf.firstSection();
	while(havesection) {
		//sprintf(section,"%s",conf.GetSectionName());
		int id=conf.getInt("ID");
		ppl7::String name=conf.get("Name");
		ppl7::String filename=conf.get("File");
		ppl7::String compression=conf.get("compression");
		havesection=conf.nextSection();
		if (filename.isEmpty()) continue;
		if (!id) continue;
		if (name.isEmpty()) continue;
		ppl7::String filepath;
		ppl7::File ff;
		filepath.setf("%s/%s",(const char*)BasePath,(const char*)filename);
		ff.open(filepath,ppl7::File::READ);

		char *buffer=NULL;
		size_t size=0;
		int type=0;
		printf ("%s: ",(const char*)filename);
		if (compression.notEmpty()) {
			ppl7::String forcecomp=ppl7::LowerCase(ppl7::Trim(compression));
			if (forcecomp=="none") {
				const char *buffer=ff.map(0,ff.size());
				printf ("Forced no compression: %u Bytes\n",(uint32_t)ff.size());
				Output(out,id,name,filename,ff.size(),buffer,(uint32_t)ff.size(),0);
				//free(buffer);
				continue;
			}
			printf ("Unbekannter Kompressionsalgorithmus für ID %d: >>%s<<\n",id,(const char*)forcecomp);
			throw ppl7::InvalidArgumentsException();
		}
		// static int Compress(ppl7::FileObject &ff, char **buffer, size_t *size, int *type)
		if (Compress(ff,&buffer,&size,&type)) {
			Output(out,id,name,filename,(uint32_t)ff.size(),buffer,size,type);
			free(buffer);
		}
	}
	ppl7::Poke32(section,0);
	BufferOut(out,section,4);
	BufferOut(out,NULL,0);
	out.puts("0\n};\n");
	if (Suffix.notEmpty()) out.puts(Suffix);
	
}

