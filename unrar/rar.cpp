#include "rar.hpp"

#include "example.h"
#include <fstream>
#include <string>
#include <vector>

#include <emscripten.h>
#include <emscripten/bind.h>


int load_rar_file() {
	// Copy the data array to a file
  std::ofstream out_file("example.rar", std::ifstream::binary);
	for (int i=0; i<rar_file_data_length; ++i) {
		//std::cout << "    rar_file_data[" << i << "]: " << (rar_file_data[i]) << std::endl;
		out_file << (rar_file_data[i]);
	}
	out_file.close();

	// Print the size of the rar file from the file system
	std::ifstream size_file("example.rar", std::ifstream::ate | std::ifstream::binary);
	std::cout << "size_file.tellg: " << size_file.tellg() << std::endl;
	size_file.close();

	// Add our own args, instead of using argc/argv
	std::vector<char*> args;
	args.push_back("./this.program");
	args.push_back("x");
	args.push_back("-y");
	args.push_back("example.rar");

	// Print all the args
	std::cout << "args: " << args.size() << std::endl;
	for (int i=0; i<args.size(); ++i) {
		std::cout << "    args[" << i << "]: " << args[i] << std::endl;
	}

#ifdef _UNIX
  setlocale(LC_ALL,"");
#endif

  InitConsole();
  ErrHandler.SetSignalHandlers(true);

#ifdef SFX_MODULE
  wchar ModuleName[NM];
#ifdef _WIN_ALL
  GetModuleFileName(NULL,ModuleName,ASIZE(ModuleName));
#else
  CharToWide(args[0].c_str(),ModuleName,ASIZE(ModuleName));
#endif
#endif

#ifdef _WIN_ALL
  SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT|SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);


#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
  // Must be initialized, normal initialization can be skipped in case of
  // exception.
  bool ShutdownOnClose=false;
#endif

  try
  {

    CommandData *Cmd=new CommandData;
#ifdef SFX_MODULE
    wcscpy(Cmd->Command,L"X");
    char *Switch=args.size()>1 ? args[1].c_str():NULL;
    if (Switch!=NULL && Cmd->IsSwitch(Switch[0]))
    {
      int UpperCmd=etoupper(Switch[1]);
      switch(UpperCmd)
      {
        case 'T':
        case 'V':
          Cmd->Command[0]=UpperCmd;
          break;
        case '?':
          Cmd->OutHelp(RARX_SUCCESS);
          break;
      }
    }
    Cmd->AddArcName(ModuleName);
    Cmd->ParseDone();
    Cmd->AbsoluteLinks=true; // If users runs SFX, he trusts an archive source.
#else // !SFX_MODULE
    Cmd->ParseCommandLine(true,args.size(), (args.data()));
    if (!Cmd->ConfigDisabled)
    {
      Cmd->ReadConfig();
      Cmd->ParseEnvVar();
    }
    Cmd->ParseCommandLine(false,args.size(),args.data());
#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
    ShutdownOnClose=Cmd->Shutdown;
#endif

    uiInit(Cmd->Sound);
    InitConsoleOptions(Cmd->MsgStream,Cmd->RedirectCharset);
    InitLogOptions(Cmd->LogName,Cmd->ErrlogCharset);
    ErrHandler.SetSilent(Cmd->AllYes || Cmd->MsgStream==MSG_NULL);

    Cmd->OutTitle();
    Cmd->ProcessCommand();
    delete Cmd;
  }
  catch (RAR_EXIT ErrCode)
  {
    ErrHandler.SetErrorCode(ErrCode);
  }
  catch (std::bad_alloc&)
  {
    ErrHandler.MemoryErrorMsg();
    ErrHandler.SetErrorCode(RARX_MEMORY);
  }
  catch (...)
  {
    ErrHandler.SetErrorCode(RARX_FATAL);
  }

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
  if (ShutdownOnClose && ErrHandler.IsShutdownEnabled())
    Shutdown();
#endif
/*
	if (rename ("page 005.png", "page005.png") != 0) {
		perror ("!!! failed to rename file");
		return EXIT_FAILURE;
	}
*/

/*
	// Print the size of a file in the file system
	FILE *fp;
	fp = fopen("fuck", "wb");
	if (fp == NULL) {
		perror ("!!! Failed to open file");
	  return EXIT_FAILURE;
	}

	char x[]="ABCDEFGHIJ";
	fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp);
	fclose(fp);

	fp = fopen("fuck", "rb");
	if (fp == NULL) {
		perror ("!!! Failed to open file");
	  return EXIT_FAILURE;
	}

	unsigned char buffer; // note: 1 byte
	fread(&buffer, 1, 1, fp);
	std::cout << "!!! buffer: " << (int) buffer << std::endl;
	fclose(fp);
*/

  ErrHandler.MainExit=true;
  return ErrHandler.GetErrorCode();
}

int open_uncompressed_file() {
	// Print all the entries in the file system
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (".")) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    printf ("!!! %s\n", ent->d_name);
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("!!! Failed to open directory");
	  return EXIT_FAILURE;
	}


	// https://www.sitepoint.com/getting-started-emscripten-transpiling-c-c-javascript-html5/
	// FIXME: This gives Permission denied.
	// Make sure unrar closes the file handle
	FILE *fp = fopen("page 004.png", "r");
	if (fp == NULL) {
		perror ("!!! Failed to open file");
		return EXIT_FAILURE;
	}

	unsigned char buffer; // note: 1 byte
	fread(&buffer, 1, 1, fp);
	std::cout << "!!! buffer: " << (int) buffer << std::endl;
	fclose(fp);

	return 0;
}

EMSCRIPTEN_BINDINGS(Wrappers) {
	emscripten::function("load_rar_file", &load_rar_file);
	emscripten::function("open_uncompressed_file", &open_uncompressed_file);
};

void on_main_loop() {

}

#if !defined(RARDLL)
int main(int argc, char *argv[])
{
	emscripten_set_main_loop(on_main_loop, 0, true);

	ErrHandler.MainExit=true;
	return ErrHandler.GetErrorCode();
}
#endif
