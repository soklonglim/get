/*
 * get.c :
 *
 *
 */

#include "get.h"

/*
 * get username from given uid
 */
char* get_username(uid_t uid){
	struct passwd *pws = getpwuid(uid);
	return pws->pw_name;
}

/*
 * get file's owner uid
 */
uid_t get_owner_uid(char* fname){
	struct stat info;
	stat(fname, &info);
	return info.st_uid;
}

/*
 * Set EUID/UID to access unprivilege files
 */
bool do_setuid(uid_t uid){
	if(seteuid(0)==-1 || seteuid(uid)==-1) { 
		if(DB) { perror("do_seteuid"); }
		return false; 
	}
	if(DB) { printf("EUID : <%d>.\n", uid); }
	return true;
}

/*
 * Reset EUID/UID to access unprivilege files
 */
bool undo_setuid(uid_t uid){
	if(seteuid(0)==-1 || seteuid(uid)==-1) {
		if(DB) { perror("undo_seteuid"); }
		return false; 
	}
	if(DB) { printf("EUID : <%d>.\n", uid); }
	return true;
}

/*
 * check if file exist or readable
 */
bool is_exist(char* fname){
	int fd = open(fname, O_NOFOLLOW);
	if(fd==-1){
		printf("Cannot open <%s>.\n", fname); 
		return false; 
	}
	return true;
}

/*
 * compare owner of <source> and <source>.access
 */
bool match_ownership(const char* fname, const char *access){
	int fd1 = open(fname, O_NOFOLLOW);
	if(fd1==-1){
		printf("Cannot open <%s>.\n", fname); 
		return false; 
	}
	int fd2 = open(access, O_NOFOLLOW);
	if(fd2==-1){
		printf("Cannot open <%s>.\n", access); 
		return false; 
	}
	struct stat st1, st2; 
	fstat(fd1, &st1); fstat(fd2, &st2);
	if(st1.st_uid!=st2.st_uid) { 
		printf("Ownership does not match.\n"); 
		return false; 
	} 
	return true;
}

/*
 * check access control
 * -1 == file not found OR access denied
 *  1 == read access (r)
 *  2 == read/write access (b)
 */
int check_access_file(const char* fname){ 
	const char* ext = ".access";
	char* name = (char*)malloc(sizeof(char)*256);
	strcpy(name, fname); strcat(name, ext);
	
	if(!is_exist(name)) { if(DB) { printf("%s not found.\n", name); } return -1; }
	if(!match_ownership(fname, name)) { return -1; }

	FILE* f = fopen(name, "r");
	if(!f) { if(DB) { perror(name); } return -1; }

	char* line = NULL; size_t len = 0; 
	char* usr = get_username(getuid());
	while(getline(&line, &len, f) != -1){
		if(line[0]=='#') { continue; }
		char* user = (char*)malloc(sizeof(char)*64); char permission = '\0';
		sscanf(line, "%s %c", user, &permission);

		// need to convert username to ruid or euid
		if(!strcmp(usr, user)) { 
			if(permission=='r'){
				return 1;
			} else if(permission=='b'){
				return 2;
			} else {
				break;
			}
		}
	}
	return -1;
}


/*
 * copy file from source to destination
 * true  == operation completed
 * false == operation fail
 */
bool copy_file(char* source, char* destination){
	if(!is_exist(source)) { return false; }

	FILE *s_fd = fopen(source, "r");
	if(!s_fd) { perror(source); return false; }
	FILE *d_fd = fopen(destination, "w");
	if(!s_fd) { perror(source); return false; }

	char c = fgetc(s_fd);
	while(c!=EOF){
		fputc(c, d_fd);
		c = fgetc(s_fd);
	}
	fclose(s_fd); fclose(d_fd);

	return true;
}

/*
 * handle the get operation 
 * true  == operation completed
 * false == operation fail
 */ 
bool perform_get_request(char* source, char* destination){
	if(DB) { printf("source<%s>\ndestination<%s>\n", source, destination); }
	
	// check ACL
	if(check_access_file(source)==-1) { 
		if(DB) { printf("No access file or access permission fail.\n"); } 
		return false; 
	}

	// get uid, euid, suid of the user
	uid_t ruid = -1; uid_t euid = -1; uid_t suid = -1; 
	getresuid(&ruid, &euid, &suid);	
	if(DB) { printf("ruid<%d>, euid<%d>, suid<%d>\n", ruid, euid, suid); }

	if(!copy_file(source, destination)) { return false; }
	seteuid(euid);
	return true;
}

/*
 * Main function
 */
int main(int argc, char** argv) {

  if(argc<3) {
    printf("\nWarning: Too few arguments.\n  $ ./get <source> <destination> \n\n");
    return(1);
  }

  if(DB) { printf("### Start! ###\n"); }

  if(perform_get_request(argv[1], argv[2])) { 
	if(DB) { printf("Operation Succeeded\n"); }
  } else {
	if(DB) { printf("Operation Failed\n"); }
  }

  if(DB) { printf("### Completed! ###\n"); }
  return(0);
}
