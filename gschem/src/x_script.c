/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2000 Ales V. Hvezda
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
/*! \todo CLEAN up line length in this file */
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void script_selection_ok(GtkWidget *w, TOPLEVEL *w_current)
{
  int len;
  const char *string;

  /* who frees this? */
  string = gtk_file_selection_get_filename(
                                           GTK_FILE_SELECTION(w_current->sowindow));

  if(string != NULL) {
    len = strlen(string);

    if (string[len - 1] != G_DIR_SEPARATOR) {
      s_log_message(_("Executing guile script [%s]\n"), string);
      g_read_file(string);
    }
  }
  /* would like to move this earlier! */
  gtk_grab_remove(w_current->sowindow);
  gtk_widget_destroy(GTK_WIDGET (w_current->sowindow));
  w_current->sowindow = NULL;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void script_selection_cancel (GtkWidget *w, TOPLEVEL *w_current)
{
  gtk_grab_remove(w_current->sowindow);
  gtk_widget_destroy (GTK_WIDGET (w_current->sowindow));
  w_current->sowindow = NULL;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void setup_script_selector (TOPLEVEL *w_current)
{
  if (!w_current->sowindow) {
    w_current->sowindow =
    gtk_file_selection_new(_("Script Execute..."));
    gtk_window_position(GTK_WINDOW(w_current->sowindow),
                        GTK_WIN_POS_MOUSE);
    /* added 4/6/98 */
    gtk_file_selection_hide_fileop_buttons(
                                           GTK_FILE_SELECTION(w_current->sowindow));
    gtk_signal_connect(GTK_OBJECT (w_current->sowindow),
                       "destroy",
                       GTK_SIGNAL_FUNC(destroy_window),
                       &w_current->sowindow);

#if 0 /* this was causing the dialog box to not die */
    gtk_signal_connect(GTK_OBJECT(w_current->sowindow),
                       "delete_event",
                       GTK_SIGNAL_FUNC(destroy_window),
                       &w_current->sowindow);
#endif

    /*! \todo consistant function names for connect
     * connect_object */
    gtk_signal_connect (GTK_OBJECT (
                                    GTK_FILE_SELECTION (w_current->sowindow)->ok_button),
                        "clicked",
                        GTK_SIGNAL_FUNC(script_selection_ok),
                        w_current);

    gtk_signal_connect(GTK_OBJECT(
                                  GTK_FILE_SELECTION(w_current->sowindow)->
                                  cancel_button),
                       "clicked",
                       GTK_SIGNAL_FUNC(script_selection_cancel),
                       w_current);

  }

  if (!GTK_WIDGET_VISIBLE (w_current->sowindow)) {
    gtk_widget_show (w_current->sowindow);
    gtk_grab_add (w_current->sowindow);
  }
}