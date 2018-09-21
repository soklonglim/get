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
 * check if file exist or readable
 */
bool is_exist(char* fname){
	if(open(fname, O_RDONLY | O_NOFOLLOW)!=-1){
		return true;
	}
	return false;
}

/*
 * check access control
 * -1 == file not found OR access denied
 *  1 == read access (r)
 *  2 == write access (w)
 *  3 == read/write access (b)
 */
int check_access_file(const char* fname){
	const char* ext = ".access";
	char* name = (char*)malloc(sizeof(char)*256);
	strcpy(name, fname); strcat(name, ext);
	if(DB) { printf(".access<%s>\n", name); }

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
			} else if(permission=='w'){
				return 2;
			} else if(permission=='b'){
				return 3;
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
	if(!is_exist(source) || check_access_file(source)==-1){ return false; }
	return copy_file(source, destination);
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
  
  perform_get_request(argv[1], argv[2]);
  // printf("username<%s>\n", get_username(getuid()));
  // printf("is_exist<%d>\n", is_exist(argv[1]));
  // perform_get_request(argv[1], argv[2]);
  // printf("AccessControl<%d>\n", check_access_file(argv[1]));

  if(DB) { printf("### Completed! ###\n"); }
  return(0);
}
