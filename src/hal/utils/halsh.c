#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tcl.h>
#include "halcmd.h"

Tcl_Interp *target_interp = NULL;
static int pending_cr = 0;

static void halError(Tcl_Interp *interp, int result) {
    Tcl_SetResult(interp, strerror(-result), TCL_VOLATILE);
}

static int refcount = 0;

static void shutdown(void) {
    if(refcount > 0) {
        refcount --;
        if(refcount == 0) halcmd_shutdown();
    }
}

static int init() {
    int result = HAL_SUCCESS;
    if(refcount == 0) {
        result = halcmd_startup(0);
        atexit(shutdown);
    }
    if(result == HAL_SUCCESS) {
        refcount ++;
    }
    return result;
}
static void halExit(ClientData d) {
    shutdown();
}

static int halCmd(ClientData cd, Tcl_Interp *interp, int argc, const char **argv) {
    int result;
    Tcl_ResetResult(interp);

    if(argc < 2) {
        Tcl_AppendResult(interp,
                "wrong # args: should be \"", argv[0], " command ...\"", NULL);
        return TCL_ERROR;
    }
    target_interp = interp;
    pending_cr = 0;
    result = halcmd_parse_cmd((char **)argv+1);
    target_interp = NULL;

    if(result == HAL_SUCCESS) return TCL_OK;
    halError(interp, result);
    return TCL_ERROR;
}

int Hal_Init(Tcl_Interp *interp) {
    int result = init();
    if(result != HAL_SUCCESS) {
	Tcl_ResetResult(interp);
	halError(interp, result);
	return TCL_ERROR;
    }

    Tcl_CreateCommand(interp, "hal", halCmd, 0, halExit);

    return TCL_OK;
}

#ifndef BUFFERLEN
#define BUFFERLEN 1024
#endif

void halcmd_output(const char *format, ...) {
    char buf[BUFFERLEN + 1];
    va_list ap;
    int len;

    va_start(ap, format);
    vsnprintf(buf, BUFFERLEN, format, ap);
    va_end(ap);

    if(pending_cr) 
	Tcl_AppendResult(target_interp, "\n", NULL);
    len = strlen(buf);
    if(buf[len-1] == '\n') {
	buf[len-1] = 0;
	pending_cr = 1;
    } else {
	pending_cr = 0;
    }
    Tcl_AppendResult(target_interp, buf, NULL);
}

void halcmd_error(const char *format, ...) {
    char buf[BUFFERLEN + 1];
    va_list ap;
    int len;

    va_start(ap, format);
    vsnprintf(buf, BUFFERLEN, format, ap);
    va_end(ap);

    if(pending_cr) 
	Tcl_AppendResult(target_interp, "\n", NULL);
    len = strlen(buf);
    if(buf[len-1] == '\n') {
	buf[len-1] = 0;
	pending_cr = 1;
    } else {
	pending_cr = 0;
    }
    Tcl_AppendResult(target_interp, buf, NULL);
}

void halcmd_warning(const char *format, ...) {
    char buf[BUFFERLEN + 1];
    va_list ap;
    int len;

    va_start(ap, format);
    vsnprintf(buf, BUFFERLEN, format, ap);
    va_end(ap);

    if(pending_cr) 
	Tcl_AppendResult(target_interp, "\n", NULL);
    len = strlen(buf);
    if(buf[len-1] == '\n') {
	buf[len-1] = 0;
	pending_cr = 1;
    } else {
	pending_cr = 0;
    }
    Tcl_AppendResult(target_interp, buf, NULL);
}

void halcmd_info(const char *format, ...) {
    char buf[BUFFERLEN + 1];
    va_list ap;
    int len;

    va_start(ap, format);
    vsnprintf(buf, BUFFERLEN, format, ap);
    va_end(ap);

    if(pending_cr) 
	Tcl_AppendResult(target_interp, "\n", NULL);
    len = strlen(buf);
    if(buf[len-1] == '\n') {
	buf[len-1] = 0;
	pending_cr = 1;
    } else {
	pending_cr = 0;
    }
    Tcl_AppendResult(target_interp, buf, NULL);
}

