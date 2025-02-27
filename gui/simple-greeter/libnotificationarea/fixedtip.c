/* Metacity fixed tooltip routine */

/* 
 * Copyright (C) 2001 Havoc Pennington
 * Copyright (C) 2003-2006 Vincent Untz
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "fixedtip.h"

/* Signals */
enum
{
  CLICKED,
  LAST_SIGNAL
};

static guint fixedtip_signals[LAST_SIGNAL] = { 0 };

struct _NaFixedTipPrivate
{
  GtkWidget      *parent;
  GtkWidget      *label;
  GtkOrientation  orientation;
};

G_DEFINE_TYPE (NaFixedTip, na_fixed_tip, GTK_TYPE_WINDOW)

static gboolean
button_press_handler (GtkWidget      *fixedtip,
                      GdkEventButton *event,
                      gpointer        data)
{
  if (event->button == 1 && event->type == GDK_BUTTON_PRESS)
    g_signal_emit (fixedtip, fixedtip_signals[CLICKED], 0);

  return FALSE;
}

static gboolean
expose_handler (GtkWidget *fixedtip)
{
  GtkRequisition req;

  gtk_widget_size_request (fixedtip, &req);

  gtk_paint_flat_box (gtk_widget_get_style (fixedtip), gtk_widget_get_window (fixedtip),
                      GTK_STATE_NORMAL, GTK_SHADOW_OUT, 
                      NULL, fixedtip, "tooltip",
                      0, 0, req.width, req.height);

  return FALSE;
}

static void
na_fixed_tip_class_init (NaFixedTipClass *class)
{
  fixedtip_signals[CLICKED] =
    g_signal_new ("clicked",
		  G_OBJECT_CLASS_TYPE (class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (NaFixedTipClass, clicked),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  g_type_class_add_private (class, sizeof (NaFixedTipPrivate));
}

/* Did you already see this code? Yes, it's gtk_tooltips_ force_window() ;-) */
static void
na_fixed_tip_init (NaFixedTip *fixedtip)
{
  GtkWidget *label;

  fixedtip->priv = G_TYPE_INSTANCE_GET_PRIVATE (fixedtip, NA_TYPE_FIXED_TIP,
                                                NaFixedTipPrivate);

  gtk_window_set_type_hint (GTK_WINDOW (fixedtip),
                            GDK_WINDOW_TYPE_HINT_TOOLTIP);

  gtk_widget_set_app_paintable (GTK_WIDGET (fixedtip), TRUE);
  gtk_window_set_resizable (GTK_WINDOW (fixedtip), FALSE);
  gtk_widget_set_name (GTK_WIDGET (fixedtip), "gtk-tooltips");
  gtk_container_set_border_width (GTK_CONTAINER (fixedtip), 4);

  label = gtk_label_new (NULL);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);
  gtk_widget_show (label);
  gtk_container_add (GTK_CONTAINER (fixedtip), label);
  fixedtip->priv->label = label;

  g_signal_connect (fixedtip, "expose_event",
                    G_CALLBACK (expose_handler), NULL);

  gtk_widget_add_events (GTK_WIDGET (fixedtip), GDK_BUTTON_PRESS_MASK);
  
  g_signal_connect (fixedtip, "button_press_event",
                    G_CALLBACK (button_press_handler), NULL);

  fixedtip->priv->orientation = GTK_ORIENTATION_HORIZONTAL;
}

static void
na_fixed_tip_position (NaFixedTip *fixedtip)
{
  GdkScreen      *screen;
  GtkRequisition  req;
  int             root_x;
  int             root_y;
  int             parent_width;
  int             parent_height;
  int             screen_width;
  int             screen_height;

  screen = gtk_widget_get_screen (fixedtip->priv->parent);
  gtk_window_set_screen (GTK_WINDOW (fixedtip), screen);

  gtk_widget_size_request (GTK_WIDGET (fixedtip), &req);

  gdk_window_get_origin (gtk_widget_get_window (fixedtip->priv->parent), &root_x, &root_y);
  gdk_drawable_get_size (GDK_DRAWABLE (gtk_widget_get_window (fixedtip->priv->parent)),
                         &parent_width, &parent_height);

  screen_width = gdk_screen_get_width (screen);
  screen_height = gdk_screen_get_height (screen);

  /* pad between panel and message window */
#define PAD 5
  
  if (fixedtip->priv->orientation == GTK_ORIENTATION_VERTICAL)
    {
      if (root_x <= screen_width / 2)
        root_x += parent_width + PAD;
      else
        root_x -= req.width + PAD;
    }
  else
    {
      if (root_y <= screen_height / 2)
        root_y += parent_height + PAD;
      else
        root_y -= req.height + PAD;
    }

  /* Push onscreen */
  if ((root_x + req.width) > screen_width)
    root_x = screen_width - req.width;

  if ((root_y + req.height) > screen_height)
    root_y = screen_height - req.height;
  
  gtk_window_move (GTK_WINDOW (fixedtip), root_x, root_y);
}

static void
na_fixed_tip_parent_size_allocated (GtkWidget     *parent,
                                    GtkAllocation *allocation,
                                    NaFixedTip    *fixedtip)
{
  na_fixed_tip_position (fixedtip);
}

static void
na_fixed_tip_parent_screen_changed (GtkWidget  *parent,
                                    GdkScreen  *new_screen,
                                    NaFixedTip *fixedtip)
{
  na_fixed_tip_position (fixedtip);
}

GtkWidget *
na_fixed_tip_new (GtkWidget      *parent,
                  GtkOrientation  orientation)
{
  NaFixedTip *fixedtip;

  g_return_val_if_fail (parent != NULL, NULL);

  fixedtip = g_object_new (NA_TYPE_FIXED_TIP, NULL);

  /* It doesn't work if we do this in na_fixed_tip_init(), so do it here */
  gtk_window_set_type_hint (GTK_WINDOW (fixedtip), GTK_WINDOW_POPUP);

  fixedtip->priv->parent = parent;

#if 0
  //FIXME: would be nice to be able to get the toplevel for the tip, but this
  //doesn't work
  GtkWidget  *toplevel;
  
  toplevel = gtk_widget_get_toplevel (parent);
  /*
  if (toplevel && GTK_WIDGET_TOPLEVEL (toplevel) && GTK_IS_WINDOW (toplevel))
    gtk_window_set_transient_for (GTK_WINDOW (fixedtip), GTK_WINDOW (toplevel));
    */
#endif

  fixedtip->priv->orientation = orientation;

  //FIXME: would be nice to move the tip when the notification area moves
  g_signal_connect_object (parent, "size-allocate",
                           G_CALLBACK (na_fixed_tip_parent_size_allocated),
                           fixedtip, 0);
  g_signal_connect_object (parent, "screen-changed",
                           G_CALLBACK (na_fixed_tip_parent_screen_changed),
                           fixedtip, 0);

  na_fixed_tip_position (fixedtip);

  return GTK_WIDGET (fixedtip);
}

void
na_fixed_tip_set_markup (GtkWidget  *widget,
                         const char *markup_text)
{
  NaFixedTip *fixedtip;

  g_return_if_fail (NA_IS_FIXED_TIP (widget));

  fixedtip = NA_FIXED_TIP (widget);

  gtk_label_set_markup (GTK_LABEL (fixedtip->priv->label),
                        markup_text);

  na_fixed_tip_position (fixedtip);
}

void
na_fixed_tip_set_orientation (GtkWidget      *widget,
                              GtkOrientation  orientation)
{
  NaFixedTip *fixedtip;

  g_return_if_fail (NA_IS_FIXED_TIP (widget));

  fixedtip = NA_FIXED_TIP (widget);

  if (orientation == fixedtip->priv->orientation)
    return;

  fixedtip->priv->orientation = orientation;

  na_fixed_tip_position (fixedtip);
}
