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



#include "ppl7.h"


void DisplayHeader()
{
	//ppl6::Cppl6Core *core=ppl6::PPLInit();
	//ppl6::CString copyright=core->GetCopyright();
	//ppl6::CString version=core->GetVersion();
	ppl7::String s,l;
	s.setf("PPL7 Resource Generator, Version %d.%d.%d",PPL7_VERSION_MAJOR,PPL7_VERSION_MINOR,PPL7_VERSION_BUILD);
	l.repeat("=",s.len());
	printf ("\n%s\n%s\n",(const char*)s,(const char*)l);
	printf ("%s\n\n",PPL7_COPYRIGHT);
}

void help()
{
	DisplayHeader();
	printf ("Syntax: pplgenresource -b BASISPFAD -c CONFIGFILE -t TARGETFILE -l LABEL\n");
	printf ("   -b BASISPFAD   Basispfad für die Resourcen\n"
			"   -c CONFIGFILE  Datei mit der Resourcen-Konfiguration\n"
			"   -c help        Zeigt Hilfe zum Format der Konfigurationsdatei an\n"
			"                  -c HELP zeigt Hilfe zum Format der Konfigurationsdatei an\n"
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
	if (argc<2) {
		help();
		return 0;
	}
	if (ppl7::HaveArgv(argc,argv,"-h") || ppl7::HaveArgv(argc,argv,"--help")) {
		help();
		return 0;
	}
	ppl7::String pfad=ppl7::GetArgv(argc,argv,"-b");
	ppl7::String configfile=ppl7::GetArgv(argc,argv,"-c");
	ppl7::String targetfile=ppl7::GetArgv(argc,argv,"-t");
	ppl7::String label=ppl7::GetArgv(argc,argv,"-l");

	ppl7::String Config=configfile;
	Config.trim();
	Config.lowerCase();
	if (Config=="help") {
		helpConfig();
		return 0;
	}

	try {
		//GenerateResourceHeader(pfad, configfile, targetfile,label);

	} catch (const ppl7::Exception &exp) {
		exp.print();
		return 1;
	}
	return 0;
}
