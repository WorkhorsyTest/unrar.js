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

int run(std::vector<char*> &args) {
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

void unrar_set_g_data_size(size_t size) {
	g_data.resize(size);
}

void unrar_set_g_data_value(size_t index, uint8_t value) {
	g_data[index] = value;

	if (index == g_data.size()-1) {
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
	}
}

int unrar_extract_all_files() {
	std::vector<char*> args;
	args.push_back("./this.program");
	args.push_back("x");
	args.push_back("-y");
	args.push_back("example.rar");

	return run(args);
}

int unrar_extract_file() {
	std::vector<char*> args;
	args.push_back("./this.program");
	args.push_back("e");
	args.push_back("example.rar");
	args.push_back("page 004.png");

	return run(args);
}

void set_return_data_size(int size) {
	EM_ASM_({
		js_set_return_data_size($0);
	}, size);
}

void set_return_data_value(int i, int data) {
	EM_ASM_({
		js_set_return_data_value($0, $1);
	}, i, data);
}

void after_cb() {
	EM_ASM_({
		if (js_after_cb) {
			js_after_cb();
		}
	}, 0);
}

int unrar_list_files() {
	std::vector<char*> args;
	args.push_back("./this.program");
	args.push_back("lb");
	args.push_back("example.rar");

	return run(args);
}

int unrar_list_fs_files() {
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

int unrar_open_extracted_file() {
	std::string file_name = "page 004.png";

	// Make the file readable
	if (chmod(file_name.c_str(), S_IRUSR|S_IRGRP|S_IROTH) == -1) {
		perror ("!!! Failed to chown");
	  return EXIT_FAILURE;
	}

	// Open the file
	FILE *fp = fopen(file_name.c_str(), "r");
	if (fp == NULL) {
		perror ("!!! Failed to open file");
		return EXIT_FAILURE;
	}

	// Set the returned file size
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	set_return_data_size(sz);

	// Send the file, one byte at a time
	unsigned char buffer;
	int i = 0;
	while (fread(&buffer, 1, 1, fp)) {
		set_return_data_value(i, (int) buffer);
		i++;
	}
	fclose(fp);

	after_cb();

	return 0;
}

EMSCRIPTEN_BINDINGS(Wrappers) {
	emscripten::function("unrar_set_g_data_size", &unrar_set_g_data_size);
	emscripten::function("unrar_set_g_data_value", &unrar_set_g_data_value);
	emscripten::function("unrar_extract_all_files", &unrar_extract_all_files);
	emscripten::function("unrar_extract_file", &unrar_extract_file);
	emscripten::function("unrar_list_files", &unrar_list_files);
	emscripten::function("unrar_list_fs_files", &unrar_list_fs_files);
	emscripten::function("unrar_open_extracted_file", &unrar_open_extracted_file);
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
