/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 6 (PPL6).
 * Web: http://www.pfp.de/ppl/
 *
 * $Author: pafe $
 * $Revision: 1.3 $
 * $Date: 2010/06/16 21:49:15 $
 * $Id: genresource.cpp,v 1.3 2010/06/16 21:49:15 pafe Exp $
 *
 *******************************************************************************
 * Copyright (c) 2010, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "ppl6.h"


void DisplayHeader()
{
	ppl6::Cppl6Core *core=ppl6::PPLInit();
	ppl6::CString copyright=core->GetCopyright();
	ppl6::CString version=core->GetVersion();
	ppl6::CString s,l;
	s.Setf("PPL6 Resource Generator, Version %s",(const char*)version);
	l.Repeat((char*)"=",s.Len());
	printf ("\n%s\n%s\n",(const char*)s,(const char*)l);
	printf ("%s\n\n",(const char*)copyright);
}

void help()
{
	DisplayHeader();
	printf ("Syntax: pplgenresource -b BASISPFAD -c CONFIGFILE -t TARGETFILE -l LABEL\n");
	printf ("   -b BASISPFAD   Basispfad für die Resourcen\n"
			"   -c CONFIGFILE  Datei mit der Resourcen-Konfiguration\n"
			"   -c help        Zeigt Hilfe zum Format der Konfigurationsdatei an\n"
			"   -t TARGETFILE  Ziel-Header-Datei. Achtung: Datei wird ueberschrieben!\n"
			"   -l LABEL       Programmlabel im Targetfile\n"
			"   -h | --help    Zeigt diese Hilfe an\n\n");

}

void helpConfig()
{
	DisplayHeader();
	printf ("Format der Konfigurationsdatei:\n\n"
			"-------------------------------------------------------------------------------\n"
			"[setup]\n"
			"# Ein optionaler Abschnitt, in dem folgende Parameter definiert werden koennen:\n"
			"#\n"
			"# Basispfad für die Resourcen, ersetzt die Option -b\n"
			"path=PFAD\n"
			"\n"
			"# Zieldatei, identisch wie Option -t\n"
			"targetfile=FILE\n"
			"\n"
			"# Name des Labels im Headerfile. identisch mit -l\n"
			"label=NAME\n"
			"\n"
			"[prefix]\n"
			"# Dieser Abschnitt enthält beliebigen Text, der im Resource-Headerfile am Anfang\n"
			"# eingefügt wird\n"
			"/* Copyright by ...*/\n"
			"\n"
			"# Es folgend die Sektionen für die einzelnen Dateien. Name oder Nummerierung\n"
			"# der Sektionen spielt keine Rolle, müssen aber eindeutig sein.\n"
			"[0]\n"
			"# Erste Resource\n"
			"\n"
			"# Eine eindeutige ID fuer diese Resource, mit der sie spaeter\n"
			"# refereniert wird. Dies muss eine Zahl zwischen 0 und 65535 sein\n"
			"ID=[0-65535]\n"
			"\n"
			"# Eindeutiger Name der Resource, ueber die sie spaeter\n"
			"# referenziert wird. Der Name darf beliebige Zeichen enthalten\n"
			"Name=NAME\n"
			"\n"
			"# Der File-Name enthält den Pfad und Namen der einzulesenden\n"
			"# Datei, relativ zum Basispfad.\n"
			"File=FILE\n"
			"\n"
			"# Optionale vorgegebene Komprimierung. Standardmaessig testet das Programm,\n"
			"# welcher Komprimieralgorythmus am besten geeignet ist (Zlib, Bzip2,\n"
			"# unkomprimiert). Mit diesem Parameter kann der gewünschte Algorythmus\n"
			"# vorgegeben werden. Derzeit wird jedoch nur \"none\" unterstützt\n"
			"# Compression=none\n"
			"\n[1]\n# Naechste Resource\n...\n"
			"-------------------------------------------------------------------------------\n"
			);
}


int main(int argc, char **argv)
{
	if (!ppl6::PPLInit()) {
		ppl6::PrintError();
		return 1;
	}
	if (argc<2) {
		help();
		return 0;
	}
	if (ppl6::getargv(argc,argv,"-h")!=NULL || ppl6::getargv(argc,argv,"--help")!=NULL) {
		help();
		return 0;
	}
	char *pfad=ppl6::getargv(argc,argv,"-b");
	char *configfile=ppl6::getargv(argc,argv,"-c");
	char *targetfile=ppl6::getargv(argc,argv,"-t");
	char *label=ppl6::getargv(argc,argv,"-l");
	ppl6::CString Config=configfile;
	Config.Trim();
	Config.LCase();
	if (Config=="help") {
		helpConfig();
		return 0;
	}

	int r=ppl6::CResource::GenerateResourceHeader(pfad, configfile, targetfile,label);
	if (r) return 0;
	ppl6::PrintError();
	return 1;

}
