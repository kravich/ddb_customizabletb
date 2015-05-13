#ifndef TRACE_H
#define TRACE_H

#include <gtk/gtk.h>
#include <stdio.h>

#if GTK_CHECK_VERSION(3,0,0)
#define TRACE_HEADER "[ddb_customizabletb_gtk3] "
#else
#define TRACE_HEADER "[ddb_customizabletb_gtk2] "
#endif

#define trace(fmt,...) fprintf(stderr, TRACE_HEADER fmt, ##__VA_ARGS__)

#endif