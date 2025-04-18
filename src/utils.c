#include <string.h>


int last_line_length(const char *buffer) {
	const char *last_newline = strrchr(buffer, '\n');
	if (last_newline) {
		return (int)strlen(last_newline + 1); // Length after the last '\n'
	} else {
		return (int)strlen(buffer); // No newline found; whole buffer is one line
	}
}


const char* get_last_line(const char *buffer) {
	const char *last_newline = strrchr(buffer, '\n');
	if (last_newline) {
		return last_newline + 1; // Pointer to start of last line
	} else {
		return buffer; // No newline found; whole buffer is one line
	}
}





