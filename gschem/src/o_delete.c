/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2011 gEDA Contributors (see ChangeLog for details)
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <config.h>

#include <stdio.h>

#include "gschem.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/*! \brief Delete an object.
 *  \par Function Description
 *  This function erases the object \a object before deleting it. It
 *  deals with connection and object connected to it.
 *
 *  \param [in] w_current The GSCHEM_TOPLEVEL object.
 *  \param [in] object    The object to delete.
 */
void o_delete (GSCHEM_TOPLEVEL *w_current, OBJECT *object)
{
  TOPLEVEL *toplevel = w_current->toplevel;

  g_return_if_fail (object != NULL);

  o_selection_remove (toplevel, toplevel->page_current->selection_list, object);
  s_page_remove (toplevel, toplevel->page_current, object);
  g_run_hook_object (w_current, "%remove-objects-hook", object);
  s_delete_object (toplevel, object);

  toplevel->page_current->CHANGED = 1;
}

/*! \brief Delete objects from the selection.
 *  \par Function Description
 *  This function deletes the objects selected on the current page of
 *  toplevel \a w_current.
 *
 *  \param [in] w_current The GSCHEM_TOPLEVEL object.
 */
void o_delete_selected (GSCHEM_TOPLEVEL *w_current)
{
  TOPLEVEL *toplevel = w_current->toplevel;
  SELECTION *selection = toplevel->page_current->selection_list;
  GList *to_remove;
  GList *iter;
  OBJECT *obj;
  unsigned int locked_num = 0;

  g_return_if_fail (o_select_selected (w_current));

  to_remove = g_list_copy (geda_list_get_glist (selection));

  for (iter = to_remove; iter != NULL; iter = g_list_next (iter)) {
    obj = (OBJECT *) iter->data;
    if (obj->selectable == FALSE)
      locked_num++;
  }

  if (locked_num > 0) {
    GList *non_locked = NULL;
    gint resp;
    GtkWidget *dialog = gtk_message_dialog_new (NULL,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
        ngettext ("Delete locked object?", "Delete %u locked objects?",
          locked_num), locked_num);
    gtk_dialog_add_buttons (GTK_DIALOG (dialog),
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_YES, GTK_RESPONSE_YES,
        GTK_STOCK_NO, GTK_RESPONSE_NO, NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_NO);

    resp = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    switch (resp) {
    case GTK_RESPONSE_YES:  /* Remove all */
      break;
    case GTK_RESPONSE_NO:   /* Remove non locked */
      for (iter = to_remove; iter != NULL; iter = g_list_next (iter)) {
        obj = (OBJECT *) iter->data;
        if (obj->selectable == TRUE)
          non_locked = g_list_append (non_locked, iter->data);
      }
      g_list_free (to_remove);
      to_remove = non_locked;
      break;
    default:                /* Cancel */
      g_list_free (to_remove);
      return;
    }
  }

  for (iter = to_remove; iter != NULL; iter = g_list_next (iter)) {
    obj = (OBJECT *) iter->data;
    o_selection_remove (toplevel, selection, obj);
    s_page_remove (toplevel, toplevel->page_current, obj);
  }

  g_run_hook_object_list (w_current, "%remove-objects-hook", to_remove);

  for (iter = to_remove; iter != NULL; iter = g_list_next (iter)) {
    obj = (OBJECT *) iter->data;
    s_delete_object (toplevel, obj);
  }

  g_list_free (to_remove);

  w_current->inside_action = 0;
  o_undo_savestate (w_current, UNDO_ALL);
  i_update_menus (w_current);
}
