#include "rar.hpp"

#include <fstream>
#include <string>
#include <vector>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

std::vector<uint8_t> g_data;

void set_g_data_size(size_t size) {
	g_data.resize(size);
}

void set_g_data_value(size_t index, uint8_t value) {
	g_data[index] = value;
}

int extract_files() {
	// Copy the data array to a file
  std::ofstream out_file("example.rar", std::ifstream::binary);
	for (int i=0; i<g_data.size(); ++i) {
		//std::cout << "    rar_file_data[" << i << "]: " << (rar_file_data[i]) << std::endl;
		out_file << (g_data[i]);
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

  ErrHandler.MainExit=true;
  return ErrHandler.GetErrorCode();
}

int list_extracted_files() {
	// Print all the entries in the file system
	DIR *dir;
	struct dirent *ent;
	struct stat buf;

	if ((dir = opendir (".")) != NULL) {
	  // print all the files and directories within directory
	  while ((ent = readdir (dir)) != NULL) {
			stat(ent->d_name, &buf);
			printf ("!!! %s: ", ent->d_name);
			printf( (S_ISDIR(buf.st_mode)) ? "d" : "-");
			printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
			printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
			printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
			printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
			printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
			printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
			printf( (buf.st_mode & S_IROTH) ? "r" : "-");
			printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
			printf( (buf.st_mode & S_IXOTH) ? "x" : "-");
			printf("\n");
	  }
	  closedir (dir);
	} else {
	  // could not open directory
	  perror ("!!! Failed to open file/directory");
	  return EXIT_FAILURE;
	}

	return 0;
}

int open_extracted_file() {
	// Make the file readable
	if (chmod("page 004.png", S_IRUSR|S_IRGRP|S_IROTH) == -1) {
		perror ("!!! Failed to chown");
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
	emscripten::function("set_g_data_size", &set_g_data_size);
	emscripten::function("set_g_data_value", &set_g_data_value);
	emscripten::function("extract_files", &extract_files);
	emscripten::function("list_extracted_files", &list_extracted_files);
	emscripten::function("open_extracted_file", &open_extracted_file);
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
