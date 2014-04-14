
#include "sst_config.h"
#include "bootshared.h"
#include <limits.h>

void update_env_var(const char* name, const int verbose) {
        char* current_ld_path = getenv(name);
        size_t new_env_size = strlen(BOOST_LIBDIR) + 1;
        new_env_size += (NULL == current_ld_path) ? 0 : strlen(current_ld_path);

#ifdef HAVE_DRAMSIM
        new_env_size += (strlen(DRAMSIM_LIBDIR) + 2) + sizeof(char) * 1;
#endif

#ifdef HAVE_M5
        new_env_size += (strlen(M5_LIBDIR) + 2) + sizeof(char) * 1;
#endif

#ifdef HAVE_LIBPHX
        new_env_size += (strlen(LIBPHX_LIBDIR) + 2) + sizeof(char) * 1;
#endif

        // Add 2 characters, we need one for the path seperator and one of the NULL?
        new_env_size += sizeof(char) * 2;

        char* updated_environment = (char*) malloc(sizeof(char) * new_env_size);

        if(NULL == current_ld_path) {
                sprintf(updated_environment, "%s", BOOST_LIBDIR);
        } else {
                sprintf(updated_environment, "%s:%s", current_ld_path, BOOST_LIBDIR);
        }

#ifdef HAVE_DRAMSIM
        char* temp_dram_copy = (char*) malloc(sizeof(char) * (strlen(updated_environment) + 1));
        sprintf(temp_dram_copy, "%s", updated_environment);
        sprintf(updated_environment, "%s:%s", temp_dram_copy, DRAMSIM_LIBDIR);
        free(temp_dram_copy);
#endif

#ifdef HAVE_M5
        char* temp_m5_copy = (char*) malloc(sizeof(char) * (strlen(updated_environment) + 1));
        sprintf(temp_m5_copy, "%s", updated_environment);
        sprintf(updated_environment, "%s:%s", temp_m5_copy, M5_LIBDIR);
        free(temp_m5_copy);
#endif

#ifdef HAVE_LIBPHX
        char* temp_phx_copy = (char*) malloc(sizeof(char) * (strlen(updated_environment) + 1));
        sprintf(temp_phx_copy, "%s", updated_environment);
        sprintf(updated_environment, "%s:%s", temp_phx_copy, LIBPHX_LIBDIR);
        free(temp_phx_copy);
#endif

        // Override the exiting LD_LIBRARY_PATH with our updated variable
        setenv(name, updated_environment, 1);
}

void boot_sst_configure_env(const int verbose) {
	update_env_var("LD_LIBRARY_PATH", verbose);
        update_env_var("DYLD_LIBRARY_PATH", verbose);
}

void boot_sst_executable(const char* binary, const int verbose, char* argv[]) {
	char* real_binary_path = (char*) malloc(sizeof(char) * PATH_MAX);

	if(strcmp(SST_INSTALL_PREFIX, "NONE") == 0) {
		sprintf(real_binary_path, "/usr/local/libexec/%s", binary);
	} else {
		sprintf(real_binary_path, "%s/libexec/%s", SST_INSTALL_PREFIX, binary);
	}

	if(verbose) {
		char** check_env = environ;
		while(NULL != check_env && NULL != check_env[0]) {
			printf("SST Environment Variable: %s\n", check_env[0]);
			check_env++;
		}
	}

	if(verbose) {
		printf("Launching SST executable (%s)...\n", real_binary_path);
	}
	
	// Flush standard out incase binary crashes
	fflush(stdout);

	int launch_sst = execve(real_binary_path, argv, environ);

	if(launch_sst == -1) {
		switch(errno) {
		case E2BIG:
			fprintf(stderr, "Unable to launch SST, the argument list is too long.\n");
			break;

		case EACCES:
			fprintf(stderr, "Unable to launch SST, part of the path does not have the appropriate read/search access permissions, check you can read the install location or the path is not an executable, did you install correctly?\n");
			break;

		case EFAULT:
			fprintf(stderr, "Unable to launch SST, the executable is corrupted. Please check your installation.\n");
			break;

		case EIO:
			fprintf(stderr, "Unable to launch SST, an error occured in the I/O system reading the executable.\n");
			break;

		case ENAMETOOLONG:
			fprintf(stderr, "Unable to launch SST, the path to the executable exceeds the operating system maximum. Try installing to a shorter path.\n");
			break;

		case ENOENT:
			fprintf(stderr, "Unable to launch SST, the executable cannot be found. Did you install it correctly?\n");
			break;

		case ENOMEM:
			fprintf(stderr, "Unable to run SST, the program requested more virtual memory than is allowed in the machine limits. You may need to contact the system administrator to have this limit increased.\n");
			break;

		case ENOTDIR:
			fprintf(stderr, "Unable to launch SST, one part of the path to the executable is not a directory. Check the path and install prefix.\n");
			break;

		case ETXTBSY:
			fprintf(stderr, "Unable to launch SST, the executable file is open for writing/reading by another process.\n");
			break;
		}
	}
}
